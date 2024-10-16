
    #include <stdlib.h>
	#include <stdio.h>
    #include "../lnklst.h"


static int swapfunc(void *a, void *b)
{
    return *(int*)a - *(int*)b;
}

int main(int argc, const char* argv[])
{
    (void)argc;(void)argv;
    int *int_ptr;
    int count;

    struct lnklst_struct *list;
    
    printf("Creating a list\n");
    list = lnklst_create();
    
    printf("Adding 10 random ints\n");
    count = 10;
    while(count--)
    {
        int_ptr = lnklst_allocate(list, sizeof(int));
        *int_ptr = rand();
    };

    count = 0;
    while(count != lnklst_count(list))
    {
        int_ptr = lnklst_index(list, count);
        printf("index %i = %i\n", count, *int_ptr);
        count++;
    };

    printf("Sorting the list\n");
    lnklst_sort(list, swapfunc);

    count = 0;
    while(count != lnklst_count(list))
    {
        int_ptr = lnklst_index(list, count);
        printf("index %i = %i\n", count, *int_ptr);
        count++;
    };

    printf("Destroying the list\n");
    lnklst_destroy(&list);
	return 0;
}

