#include "ft_malloc.h"
#include <stdio.h>

int main()
{
	void * malloc_ptr = ft_malloc(100);
	(void *)malloc_ptr;

	void * realloc_ptr = ft_realloc(NULL, 50);
	(void *)realloc_ptr;

	ft_free(NULL);

	show_alloc_mem();

	return 0;
}