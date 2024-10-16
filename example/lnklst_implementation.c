	#define LNKLST_IMPLEMENTATION

//	Mandatory allocator
	#include <stdlib.h>
	#define lnklst_platform_alloc(sz)	malloc(sz)
	#define lnklst_platform_free(ptr)	free(ptr)

//	Optional thread safety
	#include "pthread.h"
	#define lnklst_mutex_lock(arg)		pthread_mutex_lock(arg)		
	#define lnklst_mutex_unlock(arg)	pthread_mutex_unlock(arg)	
	#define lnklst_mutex_init(arg)		pthread_mutex_init(arg, NULL)		
	#define lnklst_mutex_destroy(arg)	pthread_mutex_destroy(arg)	
	#define lnklst_mutex_t				pthread_mutex_t

	#include "lnklst.h"
