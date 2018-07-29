/*
	A simple linked list

	Use in a similar fashion to malloc/free
	All allocations are added to, or freed from the linked list specified.

*/

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
	void lnklst_destroy(struct lnklst_struct *lst);

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

//  return allocation referenced by index, 0-N where 0=the last (most recent) allocation in the list
	void* lnklst_index(struct lnklst_struct *lst, uint32_t index);

//  re-link the list to sort the allocations in an order determined by swapfunc()
//  swapfunc(arg1, arg2) should return true if allocations need swapping for arg1 to be closest to the head
    void lnklst_sort(struct lnklst_struct *lst, bool(*swapfunc)(void*, void*));

//  return a count of the number of allocations in the list
    uint32_t lnklst_count(struct lnklst_struct *lst);

