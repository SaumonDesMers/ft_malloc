#include "ft_malloc.h"
#include <stdio.h>
#include <string.h>

int main()
{
	char * ptr = ft_malloc(15);

	strcpy(ptr, "Hello, World!");
	printf("Allocated memory: %s\n", ptr);

	show_alloc_mem();

	ptr = ft_realloc(ptr, 30);

	strcpy(ptr, "Hello, World! Reallocated!");
	printf("Reallocated memory: %s\n", ptr);

	show_alloc_mem();
	
	ft_free(ptr);

	return 0;
}