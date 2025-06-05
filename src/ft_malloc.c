#include "malloc.h"

void * ft_malloc(size_t size)
{
	if (size <= TINY_ALLOC_SIZE)
	{
		void * address = alloc_block(g_ft_malloc.tiny_zones, size);
		if (address == NULL)
		{
			AllocZoneHeader * new_zone = add_alloc_zone(&g_ft_malloc.tiny_zones, TINY_BLOCK_SIZE);
			if (new_zone == NULL)
			{
				return NULL;
			}
			address = alloc_block(new_zone, size);
		}
		return address;
	}
	else if (size <= SMALL_ALLOC_SIZE)
	{
		void * address = alloc_block(g_ft_malloc.small_zones, size);
		if (address == NULL)
		{
			AllocZoneHeader * new_zone = add_alloc_zone(&g_ft_malloc.small_zones, SMALL_BLOCK_SIZE);
			if (new_zone == NULL)
			{
				return NULL;
			}
			address = alloc_block(new_zone, size);
		}
		return address;
	}
	
	// For large allocations, we allocate a new memory block directly.
	void * address = allocate_memory(NULL, size + ALIGNED_HEADER_SIZE);
	if (address == MAP_FAILED)
	{
		return NULL;
	}
	g_ft_malloc.total_allocated_by_user += size;

	AllocBlockHeader * header = (AllocBlockHeader *)address;
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
