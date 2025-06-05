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

	printf("Total allocated: %zu bytes\n", g_ft_malloc.total_allocated_by_user);
}