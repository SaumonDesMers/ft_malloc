#include <stdlib.h>
#include <stdio.h>

void ft_free(void * ptr)
{
	printf("ft_free called with ptr: %p\n", ptr);
}