/*
    Simple doubly linked list by Michael Clift.

    Credit to Charles Lehner who authored a similar implementation called ll.c here:
    https://github.com/clehner/ll.c

*/
	#include    <stddef.h>
	#include    <stdlib.h>
	#include    <stdbool.h>
	#include    <stdint.h>
	#include    <assert.h>

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	//serves as a header for allocations, hiding the link in memory before them
	struct header_struct
	{
		struct header_struct 	*before;		//NULL or the address of the header of the allocation made before this one
		struct header_struct    *after;         //NULL or the address of the header of the allocation made after this one
		void					*allocation[];	//does not add to the size of this structure, only addresses memory after the *before member
	};

    //holds the head, and a counter
	struct lnklst_struct
	{
	    struct header_struct    head;
	    uint32_t                count;
	};

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

//********************************************************************************************************
// Public functions
//********************************************************************************************************

//	return a pointer to a new linked list, with head=NULL
struct lnklst_struct* lnklst_create(void)
{
	struct lnklst_struct *retval;

	retval = malloc(sizeof(struct lnklst_struct));
	assert(retval);

	retval->head.before = NULL;
	retval->head.after  = NULL;
    retval->count        = 0;

	return retval;
}

//	allocate memory on the heap, and add it to the list
void* lnklst_allocate(struct lnklst_struct *lst, size_t size)
{
	struct header_struct 	*new_entry;

	//error if there is no list
	assert(lst);

	//allocate new entry
	new_entry = malloc(sizeof(struct header_struct) + size);
	assert(new_entry);

	//add new entry to list
	new_entry->before = lst->head.before;
	lst->head.before = new_entry;
	new_entry->after = &lst->head;
	if(new_entry->before)
        new_entry->before->after = new_entry;

    lst->count++;

	//return address of allocation
	return &new_entry->allocation;
}

//	remove an allocation from the list and free it from the heap
void lnklst_free(struct lnklst_struct *lst, void* allocation)
{
	struct header_struct  *target;

    //error if there is no list
	assert(lst);

    //access allocations header by decrementing a header ptr
	target = allocation;
	target--;

	//change the *before link in the header after this one, from this header to this headers *before link
	target->after->before = target->before;
	//if there was a header before this one
	if(target->before)
        //change the *after link in the header before this one, from this header to this headers *after link
        target->before->after = target->after;

    lst->count--;
	free(target);
}

//	return the last (most recent) allocation in the list
void* lnklst_last(struct lnklst_struct *lst)
{
	void *retval=NULL;

	assert(lst);
	if(lst->count)
		retval = &lst->head.before->allocation;

	return retval;
}

//	return the allocation made before *allocation if there was one (else return NULL)
void* lnklst_before(struct lnklst_struct *lst, void *allocation)
{
	void				  *retval=NULL;
	struct header_struct  *header;

    assert(lst);

	if(allocation)
	{
		header = allocation;	//access the allocations header
		header--;
		if(header->before)		//if there was an allocation before this one
		{
			header = header->before;		//point to it
			retval = &header->allocation;	//and return it's allocation
		};
	};

	return retval;
}

//	return the allocation made after *allocation if there was one (else return NULL)
void* lnklst_after(struct lnklst_struct *lst, void *allocation)
{
	void				  *retval=NULL;
	struct header_struct  *header;

    assert(lst);

	if(allocation)
	{
		header = allocation;	//access the allocations header
		header--;
		if(header->after && (header->after != &lst->head))	    //if there was an allocation after this one
		{
			header = header->after;		    //point to it
			retval = &header->allocation;	//and return it's allocation
		};
	};

	return retval;
}

// free all entries in the list, and free the list itself
void lnklst_destroy(struct lnklst_struct *lst)
{
	struct header_struct  *hop;

	if(lst)
	{
		// while the head still points at something
		while(lst->head.before)
		{
			hop = lst->head.before->before;
			free(lst->head.before);
			lst->head.before = hop;
		};
        // free the list itself
        free(lst);
	};
}

//re-link the list to sort the allocations in an order determined by swapfunc()
void lnklst_sort(struct lnklst_struct *lst, bool(*swapfunc)(void*, void*))
{
	struct header_struct    *x;
	struct header_struct    *y;
    bool                    finished=false;
    bool                    swapped=false;

    assert(lst);
    assert(swapfunc);

    if(lst->count >1)
    {
        while(!finished)
        {
            //start of list
            x = lst->head.before;
            y = x->before;
            swapped = false;

            //walk the list
            while(y)
            {
                //swap nodes?
                if(swapfunc(&x->allocation, &y->allocation))
                {
                    //swap the outter links
                    if(y->before)
                        y->before->after = x;   //(was y)
                    x->after->before = y;       //(was x)

                    //swap the inner links
                    y->after  = x->after;
                    x->before = y->before;
                    y->before = x;
                    x->after  = y;

                    //swap the x-y to restore head -> x -> y ->end order
                    x = x->after;
                    y = y->before;
                    swapped = true;   //sort may not be finished
                };
                //step
                x = y;
                y = y->before;
            };
            //if no swapping occured on this pass, bubble sort is finished
            if(!swapped)
                finished=true;
        };
    };
}

uint32_t lnklst_count(struct lnklst_struct *lst)
{
    assert(lst);
    return lst->count;
}

//  return allocation referenced by index 0-N where 0=the last (most recent) allocation in the list
void* lnklst_index(struct lnklst_struct *lst, uint32_t index)
{
 	struct header_struct    *x;
    void* retval=NULL;

    assert(lst);
    if(index < lst->count)
    {
        x = lst->head.before;
        while(index--)
            x = x->before;
        retval = &x->allocation;
    };

    return retval;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************
