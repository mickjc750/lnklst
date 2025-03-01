
	#include <stdio.h>
    #include <stdlib.h>

    #include "greatest.h"
    #include "../lnklst.h"

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	GREATEST_MAIN_DEFS();

	extern int test_lock_count;
	extern int test_unlock_count;
	extern int test_init_count;
	extern int test_destroy_count;
    extern int test_allocation_count;

//********************************************************************************************************
// Public variables 
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

    static void reset_stats(void);


	SUITE(suite_all_tests);

    TEST test_lnklst_create(void);
    TEST test_lnklst_destroy(void);
    TEST test_lnklst_allocate(void);
    TEST test_lnklst_reallocate(void);
    TEST test_lnklst_free(void);
    TEST test_lnklst_before(void);
    TEST test_lnklst_after(void);
    TEST test_lnklst_last(void);
    TEST test_lnklst_first(void);
    TEST test_lnklst_index(void);
    TEST test_lnklst_sort(void);
    TEST test_lnklst_count(void);
    TEST test_gen_linked_list(void);

    static int swapfunc(void *a, void *b);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{
	GREATEST_MAIN_BEGIN();
	RUN_SUITE(suite_all_tests);
	GREATEST_MAIN_END();

	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

SUITE(suite_all_tests)
{
    RUN_TEST(test_lnklst_create);
    RUN_TEST(test_lnklst_destroy);
    RUN_TEST(test_lnklst_allocate);
    RUN_TEST(test_lnklst_reallocate);
    RUN_TEST(test_lnklst_free);
    RUN_TEST(test_lnklst_before);
    RUN_TEST(test_lnklst_after);
    RUN_TEST(test_lnklst_last);
    RUN_TEST(test_lnklst_first);
    RUN_TEST(test_lnklst_index);
    RUN_TEST(test_lnklst_sort);
    RUN_TEST(test_lnklst_count);
}

TEST test_lnklst_create(void)
{
    struct lnklst_struct *list;
    test_init_count = 0;
    test_allocation_count = 0;
    list = lnklst_create();
    ASSERT(list);
    ASSERT(test_init_count == 1);
    ASSERT(test_allocation_count == 1);
    ASSERT(lnklst_count(list) == 0);
    ASSERT(lnklst_first(list) == NULL);
    ASSERT(lnklst_last(list) == NULL);
    ASSERT(lnklst_index(list,0) == NULL);
    lnklst_destroy(&list);
    PASS();
}

TEST test_lnklst_destroy(void)
{
    struct lnklst_struct *list;

    list = lnklst_create();
    reset_stats();
    lnklst_destroy(&list);
    ASSERT(test_allocation_count == -1);
    ASSERT(test_lock_count == 1);
    ASSERT(test_unlock_count == 1);
    ASSERT(test_destroy_count == 1);

    list = lnklst_create();
    lnklst_allocate(list, rand()%1024);

    reset_stats();
    lnklst_destroy(&list);
    ASSERT(test_allocation_count == -2);
    ASSERT(test_lock_count == 1);
    ASSERT(test_unlock_count == 1);
    ASSERT(test_destroy_count == 1);

    list = lnklst_create();
    lnklst_allocate(list, rand()%1024);
    lnklst_allocate(list, rand()%1024);

    reset_stats();
    lnklst_destroy(&list);
    ASSERT(test_allocation_count == -3);
    ASSERT(test_lock_count == 1);
    ASSERT(test_unlock_count == 1);
    ASSERT(test_destroy_count == 1);

    PASS();
}

TEST test_lnklst_allocate(void)
{
    struct lnklst_struct *list;
    void *ptr;
    list = lnklst_create();
    size_t sz = rand()%1024;

    reset_stats();

    ptr = lnklst_allocate(list, sz);
    memset(ptr, 0xFF, sz);
    ASSERT(lnklst_count(list) == 1);        //(also locks)
    ASSERT(test_allocation_count == 1);
    ASSERT(test_lock_count == 2);
    ASSERT(test_unlock_count == 2);

    ptr = lnklst_allocate(list, sz);
    memset(ptr, 0xFF, sz);
    ASSERT(lnklst_count(list) == 2);
    ASSERT(test_allocation_count == 2);
    ASSERT(test_lock_count == 4);
    ASSERT(test_unlock_count == 4);
    lnklst_destroy(&list);

    PASS();
}

TEST test_lnklst_reallocate(void)
{
    struct lnklst_struct *list;
    void *a, *b, *c;
 
    // test reallocating the middle of 3 allocations
    list = lnklst_create();
 
    a = lnklst_allocate(list, 8);
    b = lnklst_allocate(list, 8);
    c = lnklst_allocate(list, 8);

    reset_stats();
    strcpy(b, "1234567");
    b = lnklst_reallocate(list, b, 524288);
    ASSERT(!strcmp(b, "1234567"));

    ASSERT(lnklst_count(list) == 3);        //(also locks)
    ASSERT(test_lock_count == 2);
    ASSERT(test_unlock_count == 2);

    ASSERT(lnklst_after(list, a) == b);
    ASSERT(lnklst_before(list, c) == b);

    lnklst_destroy(&list);
 

    // test reallocating a single allocations (so there is no before link)
    list = lnklst_create();
    a = lnklst_allocate(list, 8);

    reset_stats();
    strcpy(a, "1234567");
    a = lnklst_reallocate(list, a, 524288);

    ASSERT(!strcmp(a, "1234567"));
    ASSERT(lnklst_count(list) == 1);        //(also locks)
    ASSERT(test_lock_count == 2);
    ASSERT(test_unlock_count == 2);

    ASSERT(lnklst_index(list, 0) == a);

    lnklst_destroy(&list);

    PASS();
}

TEST test_lnklst_free(void)
{
    struct lnklst_struct *list = lnklst_create();

    *(int*)lnklst_allocate(list, sizeof(int)) = 1;
    *(int*)lnklst_allocate(list, sizeof(int)) = 2;
    *(int*)lnklst_allocate(list, sizeof(int)) = 3;
    *(int*)lnklst_allocate(list, sizeof(int)) = 4;
    *(int*)lnklst_allocate(list, sizeof(int)) = 5;
    ASSERT(lnklst_count(list) == 5);

    reset_stats();
    lnklst_free(list, lnklst_last(list));       //free last
    ASSERT(test_allocation_count == -1);
    ASSERT(test_lock_count == 2);
    ASSERT(test_unlock_count == 2);
    ASSERT(lnklst_count(list) == 4);
    ASSERT(*(int*)lnklst_last(list) == 4);

    reset_stats();
    lnklst_free(list, lnklst_first(list));      //free first
    ASSERT(test_allocation_count == -1);
    ASSERT(test_lock_count == 2);
    ASSERT(test_unlock_count == 2);
    ASSERT(lnklst_count(list) == 3);
    ASSERT(*(int*)lnklst_first(list) == 2);
    

    reset_stats();
    lnklst_free(list, lnklst_index(list, 1));   //free middle
    ASSERT(test_allocation_count == -1);
    ASSERT(test_lock_count == 2);
    ASSERT(test_unlock_count == 2);
    ASSERT(lnklst_count(list) == 2);
    ASSERT(*(int*)lnklst_index(list, 0) == 2);
    ASSERT(*(int*)lnklst_index(list, 1) == 4);

    lnklst_destroy(&list);
    PASS();
}

TEST test_lnklst_before(void)
{
    struct lnklst_struct *list = lnklst_create();
    int *ptr;

    *(int*)lnklst_allocate(list, sizeof(int)) = 1;
    ptr = lnklst_allocate(list, sizeof(int));
    *ptr = 2;

    reset_stats();
    ASSERT(*(int*)lnklst_before(list, ptr) == 1);
    ASSERT(test_lock_count == 1);
    ASSERT(test_unlock_count == 1);

    lnklst_destroy(&list);
    PASS();
}

TEST test_lnklst_after(void)
{
    struct lnklst_struct *list = lnklst_create();
    int *ptr;

    ptr = lnklst_allocate(list, sizeof(int));
    *(int*)lnklst_allocate(list, sizeof(int)) = 2;
    *ptr = 1;

    reset_stats();
    ASSERT(*(int*)lnklst_after(list, ptr) == 2);
    ASSERT(test_lock_count == 1);
    ASSERT(test_unlock_count == 1);

    lnklst_destroy(&list);
    PASS();
}

TEST test_lnklst_last(void)
{
    struct lnklst_struct *list = lnklst_create();
    *(int*)lnklst_allocate(list, sizeof(int)) = 1;
    *(int*)lnklst_allocate(list, sizeof(int)) = 2;
    *(int*)lnklst_allocate(list, sizeof(int)) = 3;

    reset_stats();
    ASSERT(*(int*)lnklst_last(list) == 3);

    lnklst_destroy(&list);
    PASS();
}

TEST test_lnklst_first(void)
{
    struct lnklst_struct *list = lnklst_create();
    *(int*)lnklst_allocate(list, sizeof(int)) = 1;
    *(int*)lnklst_allocate(list, sizeof(int)) = 2;
    *(int*)lnklst_allocate(list, sizeof(int)) = 3;

    reset_stats();
    ASSERT(*(int*)lnklst_first(list) == 1);

    lnklst_destroy(&list);
    PASS();
}

TEST test_lnklst_index(void)
{
    struct lnklst_struct *list = lnklst_create();
    *(int*)lnklst_allocate(list, sizeof(int)) = 1;
    *(int*)lnklst_allocate(list, sizeof(int)) = 2;
    *(int*)lnklst_allocate(list, sizeof(int)) = 3;
    *(int*)lnklst_allocate(list, sizeof(int)) = 4;
    *(int*)lnklst_allocate(list, sizeof(int)) = 5;

    reset_stats();
    ASSERT(*(int*)lnklst_index(list,0) == 1);
    ASSERT(*(int*)lnklst_index(list,1) == 2);
    ASSERT(*(int*)lnklst_index(list,2) == 3);
    ASSERT(*(int*)lnklst_index(list,3) == 4);
    ASSERT(*(int*)lnklst_index(list,4) == 5);

    ASSERT(test_lock_count == 5);
    ASSERT(test_unlock_count == 5);

    ASSERT(lnklst_index(list,5) == NULL);
    ASSERT(lnklst_index(list,-1) == NULL);

    lnklst_destroy(&list);
    PASS();
}

TEST test_lnklst_sort(void)
{
    struct lnklst_struct *list = lnklst_create();

    *(int*)lnklst_allocate(list, sizeof(int)) = 5;
    *(int*)lnklst_allocate(list, sizeof(int)) = 3;
    *(int*)lnklst_allocate(list, sizeof(int)) = 1;
    *(int*)lnklst_allocate(list, sizeof(int)) = 2;
    *(int*)lnklst_allocate(list, sizeof(int)) = 4;

    reset_stats();
    lnklst_sort(list, swapfunc);

    ASSERT(test_lock_count == 1);
    ASSERT(test_unlock_count == 1);

    ASSERT(*(int*)lnklst_index(list,0) == 1);
    ASSERT(*(int*)lnklst_index(list,1) == 2);
    ASSERT(*(int*)lnklst_index(list,2) == 3);
    ASSERT(*(int*)lnklst_index(list,3) == 4);
    ASSERT(*(int*)lnklst_index(list,4) == 5);

    lnklst_destroy(&list);
    PASS();
}

TEST test_lnklst_count(void)
{
    struct lnklst_struct *list = lnklst_create();

    ASSERT(lnklst_count(list) == 0);
    lnklst_allocate(list, sizeof(int));
    reset_stats();
    ASSERT(lnklst_count(list) == 1);
    ASSERT(test_lock_count == 1);
    ASSERT(test_unlock_count == 1);

    lnklst_destroy(&list);
    PASS();
}

static void reset_stats(void)
{
    test_lock_count = 0;
    test_unlock_count = 0;
    test_init_count = 0;
    test_destroy_count = 0;
    test_allocation_count = 0;
}

static int swapfunc(void *a, void *b)
{
    return *(int*)a - *(int*)b;
}

