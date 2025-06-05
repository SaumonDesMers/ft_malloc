#include "malloc.h"

static void print_block_linked_list(AllocBlockHeader * head)
{
	AllocBlockHeader * current = head;
	while (current != NULL)
	{
		printf("0x%lX - 0x%lX : %zu bytes\n",
			(__uint64_t)FROM_HEADER_TO_BUFFER_ADDR(current),
			(__uint64_t)FROM_HEADER_TO_BUFFER_ADDR(current) + current->size,
			current->size);
		current = current->next;
	}
}

static void print_zone_linked_list(AllocZoneHeader * head, char * name)
{
	AllocZoneHeader * current = head;
	while (current != NULL)
	{
		printf("%s : 0x%lX\n", name, (__uint64_t)current);
		print_block_linked_list(current->used_blocks);
		current = current->next;
	}
}

void show_alloc_mem()
{
	if (g_ft_malloc.tiny_zones)
	{
		print_zone_linked_list(g_ft_malloc.tiny_zones, "TINY");
	}

	if (g_ft_malloc.small_zones)
	{
		print_zone_linked_list(g_ft_malloc.small_zones, "SMALL");
	}

	if (g_ft_malloc.large_blocks)
	{
		printf("LARGE :\n");
		print_block_linked_list(g_ft_malloc.large_blocks);
	}

	printf("Total allocated: %zu bytes\n", g_ft_malloc.total_allocated_used);
}




void show_alloc_mem_stat()
{
	printf("Allocated memory statistics:\n");
	const size_t total = g_ft_malloc.total_allocated;
	if (total == 0)
	{
		printf("No memory allocated.\n");
		return;
	}

	const size_t internal = g_ft_malloc.tiny_zone_count * MEM_USED_BY_ZONE_HEADERS(TINY_BLOCK_SIZE)
		+ g_ft_malloc.small_zone_count * MEM_USED_BY_ZONE_HEADERS(SMALL_BLOCK_SIZE)
		+ g_ft_malloc.large_block_count * ALIGNED_HEADER_SIZE;
	const float internal_percentage = (float)internal * 100.0 / (float)total;

	const size_t used = g_ft_malloc.total_allocated_used;
	const float used_percentage = (float)used * 100.0 / (float)total;

	const size_t unused = total - used - internal;
	const float unused_percentage = (float)unused * 100.0 / (float)total;


	printf("Total: %zu Kb\n", total / 1024);
	printf(" - used:     %zu Kb (%.2f%%)\n", used / 1024, used_percentage);
	printf(" - unused:   %zu Kb (%.2f%%)\n", unused / 1024, unused_percentage);
	printf(" - internal: %zu Kb (%.2f%%)\n\n", internal / 1024, internal_percentage);
	printf("Tiny:  %zu blocks in %zu zones\n", g_ft_malloc.tiny_block_count, g_ft_malloc.tiny_zone_count);
	printf("Small: %zu blocks in %zu zones\n", g_ft_malloc.small_block_count, g_ft_malloc.small_zone_count);
	printf("Large: %zu blocks\n", g_ft_malloc.large_block_count);
	
}
