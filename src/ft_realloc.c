#include <stdlib.h>
#include <stdio.h>

void * ft_realloc(void * ptr, size_t size)
{
	printf("ft_realloc called with ptr: %p, size: %zu\n", ptr, size);
	return NULL;
}