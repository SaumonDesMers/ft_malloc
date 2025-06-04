#include "malloc.h"

void ft_free(void * ptr)
{
	if (ptr == NULL)
	{
		return;
	}

	if (header->size <= TINY_MAX_SIZE)
	{
		free_block(&g_ft_malloc.tiny_zones, FROM_HEADER_TO_BUFFER_ADDR(header));
	}
	else if (header->size <= SMALL_MAX_SIZE)
	{
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
	if (g_ft_malloc.large_blocks == header)
	{
		g_ft_malloc.large_blocks = header->next;
	}

	if (munmap(header, header->size + ALIGNED_HEADER_SIZE) == -1)
	{
		perror("Memory deallocation failed");
		return;
	}
}