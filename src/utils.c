#include "malloc.h"

FtMallocGlobal g_ft_malloc = {
	.large_blocks = NULL
};

void * allocate_memory(void * address, size_t size)
{
	return mmap(address, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}


AllocZoneHeader * add_alloc_zone(AllocZoneHeader ** first_zone, size_t block_size)
{
	if (first_zone == NULL)
	{
		return NULL;
	}

	// Ensure that the block size is large enough to hold the AllocZoneHeader header.
	assert(sizeof(AllocZoneHeader) < block_size && 
		"AllocZoneHeader size must be less than the block size");

	// Allocate a new zone of memory.
	AllocZoneHeader * new_zone = (AllocZoneHeader *)allocate_memory(NULL, block_size * BLOCK_COUNT_IN_ZONE);
	if (new_zone == MAP_FAILED)
	{
		return NULL;
	}
	new_zone->block_size = block_size;

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
	for (size_t i = 1; i < BLOCK_COUNT_IN_ZONE - 1; i++)
	{
		AllocBlockHeader * block = (AllocBlockHeader *)((char *)new_zone + i * block_size);
		block->next = (AllocBlockHeader *)((char *)new_zone + (i + 1) * block_size);
	}

	return new_zone;
}

void remove_alloc_zone(AllocZoneHeader ** first_zone, AllocZoneHeader * zone)
{
	if (first_zone == NULL || zone == NULL)
	{
		return;
	}

	if (*first_zone == zone)
	{
		*first_zone = zone->next;
	}
	if (zone->prev)
	{
		zone->prev->next = zone->next;
	}
	if (zone->next)
	{
		zone->next->prev = zone->prev;
	}

	munmap(zone, BLOCK_COUNT_IN_ZONE * zone->block_size);
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

			g_ft_malloc.total_allocated_by_user += alloc_size;

			return FROM_HEADER_TO_BUFFER_ADDR(block);
		}
		zone = zone->next;
	}
	return NULL;
}

void free_block(AllocZoneHeader ** first_zone, AllocBlockHeader * block_address)
{
	AllocZoneHeader * zone = *first_zone;
	while (zone)
	{
		// Check if the block address is within the zone.
		if ((char *)block_address >= (char *)zone &&
			(char *)block_address < (char *)zone + BLOCK_COUNT_IN_ZONE * zone->block_size)
		{
			// Remove the block from the used blocks list.
			if (block_address == zone->used_blocks)
			{
				zone->used_blocks = block_address->next;
			}
			if (block_address->prev)
			{
				block_address->prev->next = block_address->next;
			}
			if (block_address->next)
			{
				block_address->next->prev = block_address->prev;
			}

			// Add the block to the free blocks list.
			block_address->next = zone->free_blocks;
			block_address->prev = NULL;
			zone->free_blocks = block_address;

			g_ft_malloc.total_allocated_by_user -= block_address->size;

			if (zone->used_blocks == NULL)
			{
				// If all blocks are free, remove the zone.
				remove_alloc_zone(first_zone, zone);
			}

			return;
		}
		zone = zone->next;
	}
	fprintf(stderr, "Error: Attempted to free a block not managed by the allocator.\n");
}
