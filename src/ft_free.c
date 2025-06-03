#include "malloc.h"

void ft_free(void * ptr)
{
	if (ptr == NULL)
	{
		return;
	}
	AllocHeader * header = (AllocHeader *)((char *)ptr - sizeof(AllocHeader));
	if (munmap(header, header->size) == -1)
	{
		perror("Memory deallocation failed");
		return;
	}
}