#include "malloc.h"

void ft_free(void * ptr)
{
	if (ptr == NULL)
	{
		return;
	}

	AllocBlockHeader * header = FROM_BUFFER_TO_HEADER_ADDR(ptr);

	if (header->size <= TINY_ALLOC_SIZE)
	{
		free_block(&g_ft_malloc.tiny_zones, header);
		return;
	}
	else if (header->size <= SMALL_ALLOC_SIZE)
	{
		free_block(&g_ft_malloc.small_zones, header);
		return;
	}
	
	if (g_ft_malloc.large_blocks == header)
	{
		g_ft_malloc.large_blocks = header->next;
	}
	if (header->prev)
	{
		header->prev->next = header->next;
	}
	if (header->next)
	{
		header->next->prev = header->prev;
	}

	g_ft_malloc.total_allocated_by_user -= header->size;
	if (munmap(header, header->size + ALIGNED_HEADER_SIZE) == -1)
	{
		return;
	}
}