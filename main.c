#include "ft_malloc.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void test_perf(
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

	show_alloc_mem();

	ptr = ft_realloc(ptr, 30);
	if (ptr == NULL)
	{
		printf("Memory reallocation failed\n");
		return;
	}
	strcpy(ptr, "Hello, World! Reallocated!");
	printf("Reallocated memory at %p with content: %s\n", ptr, (char *)ptr);

	show_alloc_mem();

	ft_free(ptr);
	printf("Freed memory at %p\n", ptr);
}

int main()
{
	test_classic();

	// double start_time = (clock() / (double)CLOCKS_PER_SEC);
	// test_perf(ft_malloc, ft_free, ft_realloc);
	// double end_time = (clock() / (double)CLOCKS_PER_SEC);
	// printf("Time taken for my function: %.2f seconds\n", end_time - start_time);

	// start_time = (clock() / (double)CLOCKS_PER_SEC);
	// test_perf(malloc, free, realloc);
	// end_time = (clock() / (double)CLOCKS_PER_SEC);
	// printf("Time taken for standard function: %.2f seconds\n", end_time - start_time);

	return 0;
}