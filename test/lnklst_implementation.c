	#include <stddef.h>
	#include <stdlib.h>

	int test_lock_count = 0;
	int test_unlock_count = 0;
	int test_init_count = 0;
	int test_destroy_count = 0;
	int test_allocation_count = 0;

	static void* platform_allocate(size_t sz)
	{
		test_allocation_count++;
		return malloc(sz);
	};

	static void platform_free(void *ptr)
	{
		test_allocation_count--;
		free(ptr);
	};

//	Mandatory allocator
	#include <stdlib.h>
	#define lnklst_platform_alloc(sz)	platform_allocate(sz)
	#define lnklst_platform_free(ptr)	platform_free(ptr)

//	Optional thread safety
	#define lnklst_mutex_lock(arg)		do{test_lock_count++;}while(0)
	#define lnklst_mutex_unlock(arg)	do{test_unlock_count++;}while(0)
	#define lnklst_mutex_init(arg)		do{test_init_count++;}while(0)
	#define lnklst_mutex_destroy(arg)	do{test_destroy_count++;}while(0)
	#define lnklst_mutex_t				int

	#define LNKLST_IMPLEMENTATION
	#include "lnklst.h"
