/*
    Simple singularly linked list by Michael Clift.

    Credit to Charles Lehner who authored a similar implementation called ll.c here:
    https://github.com/clehner/ll.c

*/
	#include    <stddef.h>
	#include    <stdlib.h>
	#include    <stdbool.h>
	#include    <assert.h>

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	//serves as a header for allocations, hiding the link in memory before them
	struct lnklst_struct
	{
		struct lnklst_struct 	*prior;			//NULL or the address of the header of the previous allocation
		void					*allocation[];	//does not add to the size of this structure, only addresses memory after the *prior member
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

	retval->prior = NULL;

	return retval;
}


//	allocate memory on the heap, and add it to the list
void* lnklst_allocate(struct lnklst_struct *lst, size_t size)
{
	struct lnklst_struct 	*new_entry;

	//error if there is no list
	assert(lst);

	//allocate new entry
	new_entry = malloc(sizeof(struct lnklst_struct) + size);
	assert(new_entry);

	//add new entry to list
	new_entry->prior = lst->prior;
	lst->prior = new_entry;

	//return address of allocation
	return &new_entry->allocation;
}

//	remove an allocation from the list and free it from the heap
void lnklst_free(struct lnklst_struct *lst, void* allocation)
{
	struct lnklst_struct  *target;
	struct lnklst_struct  *next;

    //error if there is no list
	assert(lst);

	//start at the head
	next   = lst;
	target = lst->prior;

	//while target still points at a header
	while(target)
	{
		//if that header is for the allocation we want to remove
		if(&target->allocation == allocation)
		{
			//modify the header which points to the target, to point to the targets prior entry, to remove the target from the list
			next->prior = target->prior;
			target = NULL;		// stop search
			next = NULL;		// indicate that target was removed
		}
		else
		{
			//else try the previous header
			next 	= target;
			target 	= target->prior;
		};
	};

	//if the allocation was removed from the list (maybe we didn't find it)
	if(!next)
	{
		//access the allocations header, by decrementing a header ptr
		next = allocation;
		next--;
		//free allocation and it's header from the heap
		free(next);
	};
}

//	return the last (most recent) allocation in the list
void* lnklst_last(struct lnklst_struct *lst)
{
	void *retval=NULL;

	assert(lst);
	if(lst->prior)
		retval = &lst->prior->allocation;

	return retval;
}

//	return the allocation made prior to *allocation if there was one (else return NULL)
void* lnklst_prior(struct lnklst_struct *lst, void *allocation)
{
	void				  *retval=NULL;
	struct lnklst_struct  *header;

    assert(lst);

	if(allocation)
	{
		header = allocation;	//access the allocations header
		header--;
		if(header->prior)		//if there was a prior allocation
		{
			header = header->prior;			//point to it
			retval = &header->allocation;	//and return it's allocation
		};
	};

	return retval;
}

//	free all entries in the list, and free the list itself
void lnklst_destroy(struct lnklst_struct *lst)
{
	struct lnklst_struct  *hop;

	if(lst)
	{
		//while the head still points at something
		while(lst->prior)
		{
			hop = lst->prior->prior;
			free(lst->prior);
			lst->prior = hop;
		};
        //free the list itself
        free(lst);
	};
}

//re-link the list to sort the allocations in an order determined by swapfunc()
void lnklst_sort(struct lnklst_struct *lst, bool(*swapfunc)(void*, void*))
{
	struct lnklst_struct    *x;         //points to the header that points at y
	struct lnklst_struct    *y;         //points to the 1st comparison header that also points at z
	struct lnklst_struct    *z;         //points to the 2nd comparison header
    bool                    finished=false;
    bool                    swapped=false;

    assert(lst);
    assert(swapfunc);

    x = lst;
    if(x->prior)
    {
        while(!finished)
        {
            //start of list
            x = lst;
            y = x->prior;
            z = y->prior;
            swapped=false;

            //walk the list
            while(z)
            {
                //swap nodes?
                if(swapfunc(&y->allocation, &z->allocation))
                {
                    //currently:
                    //x->prior points at y
                    //y->prior points at z
                    //z->prior points at whatever
                    // to swap y and z:
                    x->prior = z;           //x->prior now points to z (instead of y)
                    y->prior = z->prior;    //y->prior now points at whatever (instead of z->prior pointing at whatever)
                    z->prior = y;           //z->prior now points at y (instead of y->prior pointing at z)

                    //because y and z were swapped in the list, restore x->y->z order using x
                    y = x->prior;
                    z = y->prior;
                    swapped=true;   //sort may not be finished
                };
                //step
                x = y;
                y = z;
                z = z->prior;
            };
            //if no swapping occured on this pass, bubble sort is finished
            if(!swapped)
                finished=true;
        };
    };
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************
