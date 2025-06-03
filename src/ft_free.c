#include "malloc.h"

void ft_free(void * ptr)
{
	if (ptr == NULL)
	{
		return;
	}
	AllocHeader * header = FROM_BUFFER_TO_HEADER_ADDR(ptr);
	
	if (header->prev)
	{
		header->prev->next = header->next;
	}
	if (header->next)
	{
		header->next->prev = header->prev;
	}
	if (g_ft_malloc.allocated_blocks == header)
	{
		g_ft_malloc.allocated_blocks = header->next;
	}

	if (munmap(header, header->size + sizeof(AllocHeader)) == -1)
	{
		perror("Memory deallocation failed");
		return;
	}
}