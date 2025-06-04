#include "ft_malloc.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

void test_perf_malloc_free(
	void * (*malloc_func)(size_t size),
	void (*free_func)(void * ptr),
	void * (*realloc_func)(void * ptr, size_t size)
)
{
	#define NUM_ALLOCATIONS 10000
	void * ptrs[NUM_ALLOCATIONS];
	memset(ptrs, 0, sizeof(ptrs));

	for (int i = 0; i < NUM_ALLOCATIONS; i++)
	{
		free_func(ptrs[i]);
		ptrs[i] = malloc_func(10);
	}
	for (int i = 0; i < NUM_ALLOCATIONS; i++)
	{
		free_func(ptrs[i]);
		ptrs[i] = malloc_func(100);
	}
	for (int i = 0; i < NUM_ALLOCATIONS; i++)
	{
		free_func(ptrs[i]);
		ptrs[i] = malloc_func(1000);
	}
	for (int i = 0; i < NUM_ALLOCATIONS; i++)
	{
		free_func(ptrs[i]);
		ptrs[i] = malloc_func(10000);
	}
	for (int i = 0; i < NUM_ALLOCATIONS; i++)
	{
		free_func(ptrs[i]);
	}
}

void test_perf_realloc(
	void * (*malloc_func)(size_t size),
	void (*free_func)(void * ptr),
	void * (*realloc_func)(void * ptr, size_t size)
)
{
	#define NUM_ALLOCATIONS 10000
	void * ptrs[NUM_ALLOCATIONS];
	memset(ptrs, 0, sizeof(ptrs));

	for (int i = 0; i < NUM_ALLOCATIONS; i++)
	{
		ptrs[i] = realloc_func(ptrs[i], 10);
	}
	for (int i = 0; i < NUM_ALLOCATIONS; i++)
	{
		ptrs[i] = realloc_func(ptrs[i], 100);
	}
	for (int i = 0; i < NUM_ALLOCATIONS; i++)
	{
		ptrs[i] = realloc_func(ptrs[i], 1000);
	}
	for (int i = 0; i < NUM_ALLOCATIONS; i++)
	{
		ptrs[i] = realloc_func(ptrs[i], 10000);
	}
	for (int i = 0; i < NUM_ALLOCATIONS; i++)
	{
		free_func(ptrs[i]);
	}
}

void test_classic()
{
	void * ptr = ft_malloc(15);
	if (ptr == NULL)
	{
		printf("Memory allocation failed\n");
		return;
	}
	strcpy(ptr, "Hello, World!");
	printf("Allocated memory at %p with content: %s\n", ptr, (char *)ptr);

	// ptr = ft_realloc(ptr, 40);
	// if (ptr == NULL)
	// {
	// 	printf("Memory reallocation failed\n");
	// 	return;
	// }
	// strcat(ptr, " Welcome to ft_malloc!");
	// printf("Reallocated memory at %p with content: %s\n", ptr, (char *)ptr);

	ft_free(ptr);
	printf("Freed memory at %p\n", ptr);

	// void * ptrs[10];
	// size_t sizes[] = {38, 586, 128, 2504, 393, 3, 743, 5678, 67, 666};
	// for (int i = 0; i < 10; i++)
	// {
	// 	ptrs[i] = ft_malloc(sizes[i]);
	// }
	// show_alloc_mem();
	// for (int i = 0; i < 10; i++)
	// {
	// 	ft_free(ptrs[i]);
	// }
}

int main()
{
	// test_classic();

	// double start_time = (clock() / (double)CLOCKS_PER_SEC);
	// test_perf_malloc_free(ft_malloc, ft_free, ft_realloc);
	// double end_time = (clock() / (double)CLOCKS_PER_SEC);
	// printf("ft_malloc ft_free: %.2f seconds\n", end_time - start_time);

	// start_time = (clock() / (double)CLOCKS_PER_SEC);
	// test_perf_malloc_free(malloc, free, realloc);
	// end_time = (clock() / (double)CLOCKS_PER_SEC);
	// printf("malloc free: %.2f seconds\n\n", end_time - start_time);

	// start_time = (clock() / (double)CLOCKS_PER_SEC);
	// test_perf_realloc(ft_malloc, ft_free, ft_realloc);
	// end_time = (clock() / (double)CLOCKS_PER_SEC);
	// printf("ft_realloc: %.2f seconds\n", end_time - start_time);

	// start_time = (clock() / (double)CLOCKS_PER_SEC);
	// test_perf_realloc(malloc, free, realloc);
	// end_time = (clock() / (double)CLOCKS_PER_SEC);
	// printf("realloc: %.2f seconds\n", end_time - start_time);

	return 0;
}