/*
	A doubly linked list, in the form of an allocator wrapper.
	Use in a similar fashion to malloc/free, only allocations are added to a list, and free's are removed from a list.

	In one C file, define an allocator, then #define LNKLST_IMPLEMENTATION and include this header.
	Eg.

		#include <stdlib.h>
		#define lnklst_platform_alloc(sz)	malloc(sz)
		#define lnklst_platform_free(arg)	free(ptr)
		#define LNKLST_IMPLEMENTATION
		#include "lnklst.h"

	lnklst does NOT check for a NULL returned by malloc(), if you want that you need to wrap malloc() with your own error handler.
	If thread safety is required, you may also define the mutex handling functions/macros, and a type.

	Eg.

		#include <stdlib.h>
		#define lnklst_platform_alloc(sz)	malloc(sz)
		#define lnklst_platform_free(arg)	free(ptr)

		//(optional)
		#include "pthread.h"
		#define lnklst_mutex_lock(arg)		pthread_mutex_lock(arg)		
		#define lnklst_mutex_unlock(arg)	pthread_mutex_unlock(arg)	
		#define lnklst_mutex_init(arg)		pthread_mutex_init(arg, NULL)		
		#define lnklst_mutex_destroy(arg)	pthread_mutex_destroy(arg)	
		#define lnklst_mutex_t				pthread_mutex_t

		#define LNKLST_IMPLEMENTATION
		#include "lnklst.h"
*/

#ifndef _LNKLST_H_
#define _LNKLST_H_

	#include <stddef.h>

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//	opaque struct
//	use a pointer to this structure to track the list
	struct lnklst_struct;

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

//	return a pointer to a new empty linked list
	struct lnklst_struct* lnklst_create(void);

//	free all allocations in the list, and the list itself
	void lnklst_destroy(struct lnklst_struct **lst);

//	allocate memory on the heap, and add it to the list
	void* lnklst_allocate(struct lnklst_struct *lst, size_t size);

//	free memory from the heap and remove it from the list
	void lnklst_free(struct lnklst_struct *lst, void* allocation);

//	return the allocation made prior to *allocation if there was one (or NULL)
	void* lnklst_before(struct lnklst_struct *lst, void *allocation);

//	return the allocation made after *allocation if there was one (or NULL)
	void* lnklst_after(struct lnklst_struct *lst, void *allocation);

//	return the last (most recent) allocation in the list
	void* lnklst_last(struct lnklst_struct *lst);

//	return the first (oldest) allocation in the list
	void* lnklst_first(struct lnklst_struct *lst);

//  return allocation referenced by index, 0-N where 0=the first (oldest) allocation in the list
	void* lnklst_index(struct lnklst_struct *lst, int index);

//  re-link the list to sort the allocations in an order determined by swapfunc()
//  If swapfunc(arg1, arg2) returns arg1-arg2, this will sort in ascending order from index 0(oldest/first) onwards
    void lnklst_sort(struct lnklst_struct *lst, int(*swapfunc)(void*, void*));

//  return a count of the number of allocations in the list
    int lnklst_count(struct lnklst_struct *lst);

#endif
#ifdef LNKLST_IMPLEMENTATION

	#include <stdint.h>
	#include <stdbool.h>


//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#ifndef lnklst_mutex_lock
		#define lnklst_mutex_lock(arg)		((void)0)
	#endif

	#ifndef lnklst_mutex_unlock
		#define lnklst_mutex_unlock(arg)	((void)0)
	#endif

	#ifndef lnklst_mutex_init
		#define lnklst_mutex_init(arg)		((void)0)
	#endif

	#ifndef lnklst_mutex_destroy
		#define lnklst_mutex_destroy(arg)	((void)0)
	#endif

	//serves as a header for allocations, hiding the link in memory before them
	struct header_struct
	{
		struct header_struct 	*before;		//NULL or the address of the header of the allocation made before this one
		struct header_struct    *after;         //NULL or the address of the header of the allocation made after this one
		void					*allocation[];	//does not add to the size of this structure, only addresses memory after the *after member
	};

    //holds the head, and a counter
	struct lnklst_struct
	{
	    struct header_struct    head;
	    int count;
		#ifdef lnklst_mutex_t
			lnklst_mutex_t		mutex;
		#endif
	};

	#define container_of(ptr, type, member)				\
	({													\
		void *__mptr = (void *)(ptr);					\
		((type *)(__mptr - offsetof(type, member)));	\
	})


//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void sort_list(struct lnklst_struct *lst, int(*swapfunc)(void*, void*));
	static void swap_nodes(struct header_struct **x_ptr, struct header_struct **y_ptr);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

struct lnklst_struct* lnklst_create(void)
{
	struct lnklst_struct *retval;

	retval = lnklst_platform_alloc(sizeof(struct lnklst_struct));

	retval->head.before = NULL;
	retval->head.after  = NULL;
    retval->count        = 0;
	#ifdef lnklst_mutex_t
	lnklst_mutex_init(&retval->mutex);
	#endif
	return retval;
}

void* lnklst_allocate(struct lnklst_struct *lst, size_t size)
{
	struct header_struct *new_entry;
	void* retval = NULL;

	if(lst)
	{
		lnklst_mutex_lock(&lst->mutex);
		//allocate new entry
		new_entry = lnklst_platform_alloc(sizeof(struct header_struct) + size);

		//add new entry to list
		new_entry->before = lst->head.before;
		lst->head.before = new_entry;
		new_entry->after = &lst->head;
		if(new_entry->before)
    	    new_entry->before->after = new_entry;

    	lst->count++;
		retval = &new_entry->allocation;
		lnklst_mutex_unlock(&lst->mutex);
	};

	return retval;
}

void lnklst_free(struct lnklst_struct *lst, void* allocation)
{
	struct header_struct  *target;

	if(lst)
	{
		lnklst_mutex_lock(&lst->mutex);
		target = container_of(allocation, struct header_struct, allocation);

		//change the *before link in the header after this one, from this header to this headers *before link
		target->after->before = target->before;
		//if there was a header before this one
		if(target->before)
    	    //change the *after link in the header before this one, from this header to this headers *after link
    	    target->before->after = target->after;

    	lst->count--;
		lnklst_platform_free(target);
		lnklst_mutex_unlock(&lst->mutex);
	};
}

void* lnklst_last(struct lnklst_struct *lst)
{
	void *retval=NULL;

	if(lst)
	{
		lnklst_mutex_lock(&lst->mutex);
		if(lst->count)
			retval = &lst->head.before->allocation;
		lnklst_mutex_unlock(&lst->mutex);
	};

	return retval;
}

void* lnklst_first(struct lnklst_struct *lst)
{
 	struct header_struct *x;
    void* retval=NULL;
	uint32_t index;

	if(lst)
	{
		lnklst_mutex_lock(&lst->mutex);
	    if(lst->count)
	    {
			index = lst->count - 1;
	        x = lst->head.before;
	        while(index--)
	            x = x->before;
	        retval = &x->allocation;
	    };
		lnklst_mutex_unlock(&lst->mutex);
	};

    return retval;
}

void* lnklst_before(struct lnklst_struct *lst, void *allocation)
{
	void				  *retval=NULL;
	struct header_struct  *header;

	if(lst && allocation)
	{
		lnklst_mutex_lock(&lst->mutex);
		header = container_of(allocation, struct header_struct, allocation);

		if(header->before)		//if there was an allocation before this one
		{
			header = header->before;		//point to it
			retval = &header->allocation;	//and return it's allocation
		};
		lnklst_mutex_unlock(&lst->mutex);
	};

	return retval;
}

void* lnklst_after(struct lnklst_struct *lst, void *allocation)
{
	void				  *retval=NULL;
	struct header_struct  *header;

	if(lst && allocation)
	{
		lnklst_mutex_lock(&lst->mutex);
		header = container_of(allocation, struct header_struct, allocation);

		if(header->after && (header->after != &lst->head))	    //if there was an allocation after this one
		{
			header = header->after;		    //point to it
			retval = &header->allocation;	//and return it's allocation
		};
		lnklst_mutex_unlock(&lst->mutex);
	};

	return retval;
}

void lnklst_destroy(struct lnklst_struct **lst)
{
	struct header_struct  *hop;

	if(lst && *lst)
	{
		lnklst_mutex_lock(&(*lst)->mutex);
		// while the head still points at something
		while((*lst)->head.before)
		{
			hop = (*lst)->head.before->before;
			lnklst_platform_free((*lst)->head.before);
			(*lst)->head.before = hop;
		};
		lnklst_mutex_unlock(&(*lst)->mutex);
		lnklst_mutex_destroy(&(*lst)->mutex);
        // free the list itself
        lnklst_platform_free((*lst));
		*lst = NULL;
	};
}

void lnklst_sort(struct lnklst_struct *lst, int(*swapfunc)(void*, void*))
{
    if(lst && swapfunc)
	{
		lnklst_mutex_lock(&lst->mutex);
		if(lst->count >1)
			sort_list(lst, swapfunc);
		lnklst_mutex_unlock(&lst->mutex);
	};
}

int lnklst_count(struct lnklst_struct *lst)
{
	int retval = 0;
	if(lst)
	{
		lnklst_mutex_lock(&lst->mutex);
		retval = lst->count;
		lnklst_mutex_unlock(&lst->mutex);
	};
    return retval;
}

void* lnklst_index(struct lnklst_struct *lst, int index)
{
 	struct header_struct *x;
    void* retval=NULL;

	if(lst)
	{
		lnklst_mutex_lock(&lst->mutex);
    	if(0 <= index && index < lst->count)
    	{
			index = lst->count - 1 - index;
	        x = lst->head.before;
        	while(index--)
	            x = x->before;
        	retval = &x->allocation;
    	};
		lnklst_mutex_unlock(&lst->mutex);
	};

    return retval;
}

static void sort_list(struct lnklst_struct *lst, int(*swapfunc)(void*, void*))
{
	struct header_struct *x;
	struct header_struct *y;
    bool swapped = false;

	do
	{
		//start of list
		x = lst->head.before;
		y = x->before;
		swapped = false;
		//walk the list
		while(y)
		{
			//swap nodes?
			if(swapfunc(&x->allocation, &y->allocation) < 0)
			{
				swap_nodes(&x, &y);
				swapped = true;   //sort may not be finished
			};
			//step
			x = y;
			y = y->before;
		};
	}while(swapped);
}

static void swap_nodes(struct header_struct **x_ptr, struct header_struct **y_ptr)
{
	struct header_struct *x = *x_ptr;	//de-reference
	struct header_struct *y = *y_ptr;

	//swap the outer links
	if(y->before)
	y->before->after = x;	//(was y)
	x->after->before = y;	//(was x)

	//swap the inner links
	y->after  = x->after;
	x->before = y->before;
	y->before = x;
	x->after  = y;

	//swap the x-y to restore head -> x -> y ->end order
	x = x->after;
	y = y->before;

	*x_ptr = x;	//re-reference
	*y_ptr = y;
}

#endif
