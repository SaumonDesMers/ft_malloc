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
		free_block(header);
		g_ft_malloc.tiny_block_count--;
		g_ft_malloc.tiny_allocated_used -= header->size;

		if (header->zone->used_blocks == NULL)
		{
			g_ft_malloc.tiny_zone_count--;
			g_ft_malloc.tiny_allocated -= header->zone->total_allocated;
			remove_alloc_zone(&g_ft_malloc.tiny_zones, header->zone);
		}
		return;
	}
	else if (header->size <= SMALL_ALLOC_SIZE)
	{
		free_block(header);
		g_ft_malloc.small_block_count--;
		g_ft_malloc.small_allocated_used -= header->size;

		if (header->zone->used_blocks == NULL)
		{
			g_ft_malloc.small_zone_count--;
			g_ft_malloc.small_allocated -= header->zone->total_allocated;
			remove_alloc_zone(&g_ft_malloc.small_zones, header->zone);
		}
		return;
	}
	
	if (g_ft_malloc.large_blocks == header)
		g_ft_malloc.large_blocks = header->next;
	if (header->prev)
		header->prev->next = header->next;
	if (header->next)
		header->next->prev = header->prev;

	g_ft_malloc.large_block_count--;
	g_ft_malloc.large_allocated -= ALIGN(header->size + ALIGNED_HEADER_SIZE, sysconf(_SC_PAGESIZE));
	g_ft_malloc.large_allocated_used -= header->size;
	munmap(header, header->size + ALIGNED_HEADER_SIZE);
}