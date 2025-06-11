#include "malloc.h"

FtMallocGlobal g_ft_malloc = {
	.tiny_zones = NULL,
	.small_zones = NULL,
	.used_large_blocks = NULL,
	.free_large_blocks = NULL,

	.mutex = PTHREAD_MUTEX_INITIALIZER,
};


void * allocate_memory(void * address, size_t size)
{
	return mmap(address, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}


AllocZoneHeader * add_alloc_zone(AllocZoneHeader ** first_zone, size_t block_size)
{
	// Ensure that the block size is large enough to hold the AllocZoneHeader header.
	assert(sizeof(AllocZoneHeader) < block_size &&
		"AllocZoneHeader size must be less than the block size");

	// Allocate a new zone of memory.
	size_t zone_size = ALIGN(block_size * MIN_BLOCK_COUNT_IN_ZONE, sysconf(_SC_PAGESIZE));
	AllocZoneHeader * new_zone = (AllocZoneHeader *)allocate_memory(NULL, zone_size);
	if (new_zone == MAP_FAILED)
	{
		return NULL;
	}
	new_zone->total_allocated = zone_size;

	ADD_TO_LINKED_LIST(*first_zone, new_zone);

	// Initialize the blocks linked list.
	new_zone->used_blocks = NULL;
	new_zone->free_blocks = (AllocBlockHeader *)ALIGN(new_zone + 1, 16);
	new_zone->free_blocks->size = (char *)new_zone + zone_size - (char *)FROM_HEADER_TO_BUFFER_ADDR(new_zone->free_blocks);
	new_zone->free_blocks->prev = NULL;
	new_zone->free_blocks->next = NULL;
	new_zone->free_blocks->zone = new_zone;

	return new_zone;
}

void remove_alloc_zone(AllocZoneHeader ** first_zone, AllocZoneHeader * zone)
{
	REMOVE_FROM_LINKED_LIST(*first_zone, zone);
	munmap(zone, zone->total_allocated);
}

void * alloc_block(AllocZoneHeader * zone, size_t alloc_size)
{
	while (zone)
	{
		detect_linked_list_cycle(zone->free_blocks);
		AllocBlockHeader * free_block = zone->free_blocks;
		while (free_block)
		{
			if (free_block->size >= alloc_size)
			{
				AllocBlockHeader * alloc_block = free_block;
				if (free_block->size > alloc_size + MIN_FRAGMENTATION_SIZE)
				{
					// Find new free block location
					free_block = (AllocBlockHeader *)ALIGN((char *)free_block + ALIGNED_HEADER_SIZE + alloc_size, 16);
					// Copy header from old block
					*free_block = *alloc_block;
					// Get the real allocation size caused by memory alignement
					alloc_block->size = (char *)free_block - (char *)alloc_block - ALIGNED_HEADER_SIZE;
					// Get the real remaining size
					free_block->size = (char *)alloc_block + free_block->size - (char *)free_block;
					
					// Update previous node
					if (free_block->prev)
						free_block->prev->next = free_block;
					else
						zone->free_blocks = free_block;
					// Update next node
					if (free_block->next)
						free_block->next->prev = free_block;
				}
				else
				{
					REMOVE_FROM_LINKED_LIST(zone->free_blocks, free_block);
				}
				ADD_TO_LINKED_LIST(zone->used_blocks, alloc_block);
				return FROM_HEADER_TO_BUFFER_ADDR(alloc_block);
			}
			free_block = free_block->next;
		}
		zone = zone->next;
	}
	return NULL;
}

void free_block(AllocBlockHeader * block)
{
	REMOVE_FROM_LINKED_LIST(block->zone->used_blocks, block);

	AllocBlockHeader * free_block = block->zone->free_blocks;
	if (free_block == NULL)
	{
		block->prev = NULL;
		block->next = NULL;
		block->zone->free_blocks = block;
	}
	else if (free_block > block)
	{
		AllocBlockHeader * next = free_block;
		
		// Check if next block is adjacent for defragmentation
		if (next && (char *)block + ALIGNED_HEADER_SIZE + block->size == (char *)next)
		{
			block->size += ALIGNED_HEADER_SIZE + next->size;
			next = next->next;
		}

		block->prev = NULL;
		block->zone->free_blocks = block;

		block->next = next;
		if (block->next)
			block->next->prev = block;
	}
	else
	{
		while (free_block->next && free_block->next < block)
			free_block = free_block->next;

		AllocBlockHeader * prev = free_block;
		AllocBlockHeader * next = free_block->next;
		
		// Check if prev block is adjacent for defragmentation
		if (prev && (char *)prev + ALIGNED_HEADER_SIZE + prev->size == (char *)block)
		{
			prev->size += ALIGNED_HEADER_SIZE + block->size;
			// Make the freed block point to the previous one
			block = prev;
			prev = prev->prev;
		}

		// Check if next block is adjacent for defragmentation
		if (next && (char *)block + ALIGNED_HEADER_SIZE + block->size == (char *)next)
		{
			block->size += ALIGNED_HEADER_SIZE + next->size;
			next = next->next;
		}

		block->prev = prev;
		if (block->prev)
			block->prev->next = block;

		block->next = next;
		if (block->next)
			block->next->prev = block;
	}
}
