#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void * ft_malloc(size_t size)
{
	printf("ft_malloc called with size: %zu\n", size);
	return NULL;
}