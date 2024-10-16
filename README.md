# lnklst
A doubly linked list, in the form of an allocator wrapper.

* STB style single header
* Full test suite using greatest (https://github.com/silentbicycle/greatest)
* Thread safe
* Sort function compatible with qsort() comparators int()(void*,void*)

## Why?
The classical approach to a linked list involves including a node within the object to be linked, traversing these nodes, and recovering their containing objects with the container_of() macro.
A linked list, should be able to contain a list of *anything*. The object in the list should not need to contain the node, as it is not necessarily the objects business if it is in a linked list or not.
As the elements of a linked list are typically memory allocations, it makes more sense to link these allocations themselves, rather than jamming nodes into the objects being linked.
lnklst achieves this by wrapping an allocator, and extending the allocations made with the metadata required to link them together.

lnklst is a single header, as popularized by Sean Barratt https://github.com/nothings

## Usage:
A linked list is held by the application as a pointer to a struct. This pointer should be initialized with the value returned by lnklst_create();

    struct lnklst_struct *list = lnklst_create();

Elements are added to the list using lnklst_allocate();

    int *ptr = lnklst_allocate(list, sizeof(int))
    *ptr = 4;

Elements are removed from the list using lnklst_free();

    int *ptr = lnklst_last(list);
    lnklst_free(list, ptr);

The first (oldest) and last (newest) allocations are available using lnklst_first() or lnklst_last(), or an index may be used (0 being the oldest) with lnklst_index().
The allocations made before/after an existing allocation may be accessed with lnklst_before() and lnklst_after().
The number of elements in a list is available using lnklst_count().
The list may be sorted using lnklst_sort(), this must be passed a comparator function, which has the same signature as used by qsort().

The list must be destroyed by calling lnklst_destroy(). Note that this takes the address of a pointer, and will NULL the pointer.

    lnklst_destroy(&list);

For more info see the example/ given, and read the explanation within lnklst.h

