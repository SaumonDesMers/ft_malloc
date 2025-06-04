#include "malloc.h"

FtMallocGlobal g_ft_malloc = {
	.large_blocks = NULL
};

void show_alloc_mem()
{
	// AllocHeader * blocks[3] = {
	// 	g_ft_malloc.tiny_blocks,
	// 	g_ft_malloc.small_blocks,
	// 	g_ft_malloc.large_blocks
	// };
	// char * block_names[3] = {
	// 	"TINY",
	// 	"SMALL",
	// 	"LARGE"
	// };
	// for (size_t i = 0; i < 3; i++)
	// {
	// 	printf("%s :\n", block_names[i]);
	// 	AllocHeader * current = blocks[i];
	// 	while (current != NULL)
	// 	{
	// 		printf("0x%lX - 0x%lX : %zu bytes\n",
	// 			(__uint64_t)FROM_HEADER_TO_BUFFER_ADDR(current),
	// 			(__uint64_t)FROM_HEADER_TO_BUFFER_ADDR(current) + current->size,
	// 			current->size);
	// 		current = current->next;
	// 	}
	// }
}

void * allocate_memory(void * address, size_t size)
{
	return mmap(address, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}


AllocZone * add_alloc_zone(AllocZone ** first_zone, size_t block_size)
{
	if (first_zone == NULL)
	{
		return NULL;
	}

	// Ensure that the block size is large enough to hold the AllocZone header.
	assert(sizeof(AllocZone) < block_size * 2 && 
		"AllocZone size must be less than twice the block size");

	// Allocate a new zone of memory.
	AllocZone * new_zone = (AllocZone *)allocate_memory(NULL, block_size * BLOCK_COUNT_IN_ZONE);
	if (new_zone == MAP_FAILED)
	{
		return NULL;
	}
	new_zone->block_size = block_size;
	new_zone->free_block_count = BLOCK_COUNT_IN_ZONE - 2; // The first four blocks are reserved for the header.

	// Insert the new zone at the beginning of the linked list.
	new_zone->prev = NULL;
	new_zone->next = *first_zone;
	*first_zone = new_zone;
	if (new_zone->next != NULL)
	{
		new_zone->next->prev = new_zone;
	}

	// Initialize the free blocks linked list.
	for (size_t i = 0; i < BLOCK_COUNT_IN_ZONE; i++)
	{
		new_zone->free_blocks[i] = i + 1;
	}
	new_zone->free_blocks[0] = 2;
	new_zone->free_blocks[1] = 0;
	new_zone->free_blocks[BLOCK_COUNT_IN_ZONE - 1] = 0;

	return new_zone;
}

void remove_alloc_zone(AllocZone ** first_zone, AllocZone * zone)
{
	if (first_zone == NULL || zone == NULL)
	{
		return;
	}

	if (*first_zone == zone)
	{
		*first_zone = zone->next;
	}
	if (zone->prev != NULL)
	{
		zone->prev->next = zone->next;
	}
	if (zone->next != NULL)
	{
		zone->next->prev = zone->prev;
	}

	munmap(zone, BLOCK_COUNT_IN_ZONE * zone->block_size);
}

void * alloc_block(AllocZone * zone, size_t alloc_size)
{
	assert(alloc_size > ~(__uint16_t)0 &&
		"Allocation size must be less than or equal to short max (65535 bytes)");

	while (zone != NULL)
	{
		// Check if there are any free blocks in the zone.
		if (zone->free_block_count > 0)
		{
			// Get the index of the first free block.
			const size_t free_index = zone->free_blocks[0];
			const void * block_address = (char *)zone + free_index * zone->block_size;

			*(__uint16_t *)block_address = alloc_size; // Store the allocation size in the block header.

			zone->free_blocks[0] = zone->free_blocks[free_index]; // Update the head of the free blocks list.
			zone->free_blocks[free_index] = 0; // Mark the block as allocated.

			zone->free_block_count--;

			return (char *)block_address + 2; // Return the address of the allocated block, skipping the header.
		}
		zone = zone->next;
	}
	return NULL;
}

void free_block(AllocZone ** first_zone, void * block_address)
{
	if (block_address == NULL)
	{
		return;
	}

	block_address = (char *)block_address - 2; // Adjust to get the address of the block header.

	AllocZone * zone = *first_zone;
	while (zone != NULL)
	{
		// Check if the block address is within the zone.
		if ((char *)block_address >= (char *)zone &&
			(char *)block_address < (char *)zone + BLOCK_COUNT_IN_ZONE * zone->block_size)
		{
			// Calculate the index of the block in the zone.
			size_t block_index = ((char *)block_address - (char *)zone) / zone->block_size;

			// Add the block back to the linked list of free blocks.
			zone->free_blocks[block_index] = zone->free_blocks[0];
			zone->free_blocks[0] = block_index;

			zone->free_block_count++;

			if (zone->free_block_count == BLOCK_COUNT_IN_ZONE - 2)
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
