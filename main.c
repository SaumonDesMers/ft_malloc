#include "ft_malloc.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define TEST_COUNT 30000

void test_perf(
	void * (*malloc_func)(size_t size),
	void (*free_func)(void * ptr),
	void * (*realloc_func)(void * ptr, size_t size),
	size_t * sizes
)
{
	void ** ptrs = malloc_func(TEST_COUNT * sizeof(void *));
	for (int i = 0; i < TEST_COUNT; i++)
	{
		ptrs[i] = malloc_func(sizes[i]);
		// memset(ptrs[i], 0, sizes[i]);
	}
	for (int i = 0; i < TEST_COUNT; i++)
	{
		ptrs[i] = realloc_func(ptrs[i], sizes[i] * 2);
		// memset(ptrs[i], 0, sizes[i] * 2);
	}
	for (int i = 0; i < TEST_COUNT; i++)
	{
		free_func(ptrs[i]);
	}
	free_func(ptrs);
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

	ptr = ft_realloc(ptr, 40);
	if (ptr == NULL)
	{
		printf("Memory reallocation failed\n");
		return;
	}
	strcat(ptr, " Welcome to ft_malloc!");
	printf("Reallocated memory at %p with content: %s\n", ptr, (char *)ptr);

	ft_free(ptr);
	printf("Freed memory at %p\n", ptr);

	void * ptrs[20];
	for (int i = 0; i < 20; i++)
	{
		ptrs[i] = ft_malloc(rand() % 10000 + 1);
	}
	show_alloc_mem();
	for (int i = 0; i < 20; i++)
	{
		ft_free(ptrs[i]);
	}
}

int main()
{
	srand((unsigned int)time(NULL));

	// test_classic();


	size_t * sizes = malloc(TEST_COUNT * sizeof(size_t));
	for (int i = 0; i < TEST_COUNT; i++)
	{
		sizes[i] = (rand() % 10000) + 1;
	}

	double start_time = (clock() / (double)CLOCKS_PER_SEC);
	test_perf(ft_malloc, ft_free, ft_realloc, sizes);
	double end_time = (clock() / (double)CLOCKS_PER_SEC);
	printf("ft_malloc ft_free: %.2f seconds\n", end_time - start_time);

	start_time = (clock() / (double)CLOCKS_PER_SEC);
	test_perf(malloc, free, realloc, sizes);
	end_time = (clock() / (double)CLOCKS_PER_SEC);
	printf("malloc free: %.2f seconds\n", end_time - start_time);

	return 0;
}