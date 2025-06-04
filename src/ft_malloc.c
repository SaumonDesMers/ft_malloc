#include "malloc.h"

void * ft_malloc(size_t size)
{
	if (size <= TINY_MAX_SIZE)
	{
		void * address = alloc_block(&g_ft_malloc.tiny_zones);
		if (address == NULL)
		{
			add_alloc_zone(&g_ft_malloc.tiny_zones, TINY_MAX_SIZE);
			address = alloc_block(&g_ft_malloc.tiny_zones);
		}
		return address;
	}
	else if (size <= SMALL_MAX_SIZE)
	{
	}
	
	// For large allocations, we allocate a new memory block directly.
	void * address = allocate_memory(NULL, size + ALIGNED_HEADER_SIZE);
	if (address == MAP_FAILED)
	{
		return NULL;
	}

	AllocHeader * header = (AllocHeader *)address;
	header->size = size;
	header->prev = NULL;
	header->next = g_ft_malloc.large_blocks;
	g_ft_malloc.large_blocks = header;
	if (header->next != NULL)
	{
		header->next->prev = header;
	}
	return FROM_HEADER_TO_BUFFER_ADDR(address);
}
