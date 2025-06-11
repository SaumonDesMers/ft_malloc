#include "malloc.h"


void * malloc(size_t size)
{
	pthread_mutex_lock(&g_ft_malloc.mutex);
	void * address = intern_malloc(size);
	pthread_mutex_unlock(&g_ft_malloc.mutex);
	return address;
}

void * intern_malloc(size_t size)
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

	AllocBlockHeader * free_large_block = g_ft_malloc.free_large_blocks;

	const size_t aligned_size = ALIGNED_LARGE_BLOCK_SIZE(size);
	while (free_large_block && ALIGNED_LARGE_BLOCK_SIZE(free_large_block->size) == aligned_size)
		free_large_block = free_large_block->next;
	
	if (free_large_block)
	{
		free_large_block->size = size;
		REMOVE_FROM_LINKED_LIST(g_ft_malloc.free_large_blocks, free_large_block);
		ADD_TO_LINKED_LIST(g_ft_malloc.used_large_blocks, free_large_block);
		return FROM_HEADER_TO_BUFFER_ADDR(free_large_block);
	}
	
	void * address = allocate_memory(NULL, aligned_size);
	if (address == MAP_FAILED)
	{
		return NULL;
	}
	
	AllocBlockHeader * header = (AllocBlockHeader *)address;
	header->size = size;

	ADD_TO_LINKED_LIST(g_ft_malloc.used_large_blocks, header);
	return FROM_HEADER_TO_BUFFER_ADDR(address);
}
