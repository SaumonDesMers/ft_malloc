#include "malloc.h"

FtMallocGlobal g_ft_malloc = {
	.tiny_zones = NULL,
	.small_zones = NULL,
	.large_blocks = NULL,

	.mutex = PTHREAD_MUTEX_INITIALIZER,

	.tiny_zone_count = 0,
	.tiny_block_count = 0,
	.tiny_allocated = 0,
	.tiny_allocated_used = 0,

	.small_zone_count = 0,
	.small_block_count = 0,
	.small_allocated = 0,
	.small_allocated_used = 0,

	.large_block_count = 0,
	.large_allocated = 0,
	.large_allocated_used = 0
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
	new_zone->block_size = block_size;
	new_zone->block_count = zone_size / block_size;
	new_zone->total_allocated = zone_size;

	// Insert the new zone at the beginning of the linked list.
	new_zone->prev = NULL;
	new_zone->next = *first_zone;
	*first_zone = new_zone;
	if (new_zone->next != NULL)
	{
		new_zone->next->prev = new_zone;
	}

	// Initialize the blocks linked list.
	new_zone->used_blocks = NULL;
	new_zone->free_blocks = (AllocBlockHeader *)((char *)new_zone + block_size);
	for (size_t i = 1; i < new_zone->block_count - 1; i++)
	{
		AllocBlockHeader * block = (AllocBlockHeader *)((char *)new_zone + i * block_size);
		block->next = (AllocBlockHeader *)((char *)new_zone + (i + 1) * block_size);
		block->zone = new_zone;
	}
	// Special case for the last block.
	AllocBlockHeader * last_block = (AllocBlockHeader *)((char *)new_zone + (new_zone->block_count - 1) * block_size);
	last_block->next = NULL;
	last_block->zone = new_zone;

	return new_zone;
}

void remove_alloc_zone(AllocZoneHeader ** first_zone, AllocZoneHeader * zone)
{
	if (*first_zone == zone)
		*first_zone = zone->next;
	if (zone->prev)
		zone->prev->next = zone->next;
	if (zone->next)
		zone->next->prev = zone->prev;

	munmap(zone, zone->total_allocated);
}

void * alloc_block(AllocZoneHeader * zone, size_t alloc_size)
{
	while (zone)
	{
		// Check if there are any free blocks in the zone.
		if (zone->free_blocks)
		{
			// Allocate a block from the free blocks linked list.
			AllocBlockHeader * block = zone->free_blocks;
			zone->free_blocks = block->next;

			// Add the block to the used blocks linked list.
			block->next = zone->used_blocks;
			block->prev = NULL;
			zone->used_blocks = block;
			if (zone->used_blocks->next)
			{
				zone->used_blocks->next->prev = zone->used_blocks;
			}

			block->size = alloc_size;

			return FROM_HEADER_TO_BUFFER_ADDR(block);
		}
		zone = zone->next;
	}
	return NULL;
}

void free_block(AllocBlockHeader * block)
{
	// Remove the block from the used blocks list.
	if (block == block->zone->used_blocks)
		block->zone->used_blocks = block->next;
	if (block->prev)
		block->prev->next = block->next;
	if (block->next)
		block->next->prev = block->prev;

	// Add the block to the free blocks list.
	block->next = block->zone->free_blocks;
	block->prev = NULL;
	block->zone->free_blocks = block;
}
