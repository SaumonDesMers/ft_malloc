#include "malloc.h"

static size_t ft_strlen(const char * str)
{
	size_t len = 0;
	while (str[len] != '\0')
	{
		len++;
	}
	return len;
}

static void put_char(char c)
{
	write(1, &c, 1);
}

static void put_nb(size_t nb)
{
	if (nb >= 10)
	{
		put_nb(nb / 10);
	}
	put_char((nb % 10) + '0');
}

static size_t put_hex(size_t nb)
{
	size_t count = 0;
	if (nb >= 16)
	{
		count += put_hex(nb / 16);
	}
	char c = (nb % 16);
	if (c < 10)
	{
		put_char(c + '0');
	}
	else
	{
		put_char(c - 10 + 'A');
	}
	return count + 1;
}


static void sort(void ** ptr_vector, size_t count)
{
	if (ptr_vector == NULL || count < 2)
		return;
	
	for (size_t i = 0; i < count - 1; i++)
	{
		for (size_t j = i + 1; j < count; j++)
		{
			if (ptr_vector[i] > ptr_vector[j])
			{
				void * temp = ptr_vector[i];
				ptr_vector[i] = ptr_vector[j];
				ptr_vector[j] = temp;
			}
		}
	}
}

static void print_block_linked_list(AllocBlockHeader * head)
{
	AllocBlockHeader * current = head;

	size_t used_block_count = 0;
	while (current != NULL)
	{
		used_block_count++;
		current = current->next;
	}

	AllocBlockHeader ** ptr_vector = allocate_memory(NULL, used_block_count * sizeof(AllocBlockHeader *));
	if (ptr_vector == NULL)
		return;

	current = head;
	size_t index = 0;
	while (current != NULL)
	{
		ptr_vector[index++] = current;
		current = current->next;
	}

	sort((void **)ptr_vector, used_block_count);

	for (size_t i = 0; i < used_block_count; i++)
	{
		current = ptr_vector[i];
		
		write(1, "0x", 2);
		put_hex((__uint64_t)FROM_HEADER_TO_BUFFER_ADDR(current));
		write(1, " - 0x", 5);
		put_hex((__uint64_t)FROM_HEADER_TO_BUFFER_ADDR(current) + current->size);
		write(1, " : ", 3);
		put_nb(current->size);
		write(1, " bytes\n", 7);
	}
}

static void print_zone_linked_list(AllocZoneHeader * head, char * name)
{
	AllocZoneHeader * current = head;

	size_t used_block_count = 0;
	while (current != NULL)
	{
		used_block_count++;
		current = current->next;
	}

	AllocZoneHeader ** ptr_vector = allocate_memory(NULL, used_block_count * sizeof(AllocZoneHeader *));
	if (ptr_vector == NULL)
		return;

	current = head;
	size_t index = 0;
	while (current != NULL)
	{
		ptr_vector[index++] = current;
		current = current->next;
	}

	sort((void **)ptr_vector, used_block_count);

	for (size_t i = 0; i < used_block_count; i++)
	{
		current = ptr_vector[i];
		
		write(1, name, ft_strlen(name));
		write(1, " : 0x", 5);
		put_hex((__uint64_t)current);
		write(1, "\n", 1);

		print_block_linked_list(current->used_blocks);
	}
}

void show_alloc_mem()
{
	pthread_mutex_lock(&g_ft_malloc.mutex);

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
		write(1, "LARGE :\n", 8);
		print_block_linked_list(g_ft_malloc.large_blocks);
	}

	write(1, "Total allocated: ", 17);
	put_nb(g_ft_malloc.tiny_allocated + g_ft_malloc.small_allocated + g_ft_malloc.large_allocated);
	write(1, " bytes\n", 7);
	
	pthread_mutex_unlock(&g_ft_malloc.mutex);
}


void print_used_unused_internal(
	size_t used, size_t used_percentage,
	size_t unused, size_t unused_percentage,
	size_t internal, size_t internal_percentage)
{
	write(1, " - used:     ", 13);
	put_nb(used / 1024);
	write(1, " Kb (", 5);
	put_nb(used_percentage);
	write(1, "%)\n", 3);

	write(1, " - unused:   ", 13);
	put_nb(unused / 1024);
	write(1, " Kb (", 5);
	put_nb(unused_percentage);
	write(1, "%)\n", 3);

	write(1, " - internal: ", 13);
	put_nb(internal / 1024);
	write(1, " Kb (", 5);
	put_nb(internal_percentage);
	write(1, "%)\n\n", 4);
}

void show_alloc_mem_stat()
{
	write(1, "Allocated memory statistics:\n", 30);

	const size_t tiny_total = g_ft_malloc.tiny_allocated;
	const size_t small_total = g_ft_malloc.small_allocated;
	const size_t large_total = g_ft_malloc.large_allocated;
	const size_t total = tiny_total + small_total + large_total;
	
	if (total == 0)
	{
		write(1, "No memory allocated.\n", 21);
		return;
	}
	
	const size_t tiny_total_percentage = (tiny_total * 100) / total;
	const size_t small_total_percentage = (small_total * 100) / total;
	const size_t large_total_percentage = (large_total * 100) / total;

	const size_t tiny_internal = g_ft_malloc.tiny_zone_count * MEM_USED_BY_ZONE_HEADERS(TINY_BLOCK_SIZE);
	const size_t tiny_internal_percentage = (tiny_internal * 100) / total;
	const size_t small_internal = g_ft_malloc.small_zone_count * MEM_USED_BY_ZONE_HEADERS(SMALL_BLOCK_SIZE);
	const size_t small_internal_percentage = (small_internal * 100) / total;
	const size_t large_internal = g_ft_malloc.large_block_count * ALIGNED_HEADER_SIZE;
	const size_t large_internal_percentage = (large_internal * 100) / total;
	const size_t internal = tiny_internal + small_internal + large_internal;
	const size_t internal_percentage = (internal * 100) / total;

	const size_t tiny_used = g_ft_malloc.tiny_allocated_used;
	const size_t tiny_used_percentage = (tiny_used * 100) / total;
	const size_t small_used = g_ft_malloc.small_allocated_used;
	const size_t small_used_percentage = (small_used * 100) / total;
	const size_t large_used = g_ft_malloc.large_allocated_used;
	const size_t large_used_percentage = (large_used * 100) / total;
	const size_t used = g_ft_malloc.tiny_allocated_used + g_ft_malloc.small_allocated_used + g_ft_malloc.large_allocated_used;
	const size_t used_percentage = (used * 100) / total;

	const size_t tiny_unused = g_ft_malloc.tiny_allocated - g_ft_malloc.tiny_allocated_used - tiny_internal;
	const size_t tiny_unused_percentage = (tiny_unused * 100) / total;
	const size_t small_unused = g_ft_malloc.small_allocated - g_ft_malloc.small_allocated_used - small_internal;
	const size_t small_unused_percentage = (small_unused * 100) / total;
	const size_t large_unused = g_ft_malloc.large_allocated - g_ft_malloc.large_allocated_used - large_internal;
	const size_t large_unused_percentage = (large_unused * 100) / total;
	const size_t unused = total - used - internal;
	const size_t unused_percentage = (unused * 100) / total;

	write(1, "Total: ", 7);
	put_nb(total / 1024);
	write(1, " Kb\n", 4);

	print_used_unused_internal(
		used, used_percentage,
		unused, unused_percentage,
		internal, internal_percentage
	);

	write(1, "Tiny: ", 7);
	put_nb(tiny_total / 1024);
	write(1, " Kb (", 5);
	put_nb(tiny_total_percentage);
	write(1, "%) - ", 6);
	put_nb(g_ft_malloc.tiny_block_count);
	write(1, " blocks in ", 11);
	put_nb(g_ft_malloc.tiny_zone_count);
	write(1, " zones\n", 7);

	print_used_unused_internal(
		tiny_used, tiny_used_percentage,
		tiny_unused, tiny_unused_percentage,
		tiny_internal, tiny_internal_percentage
	);

	write(1, "Small: ", 7);
	put_nb(small_total / 1024);
	write(1, " Kb (", 5);
	put_nb(small_total_percentage);
	write(1, "%) - ", 6);
	put_nb(g_ft_malloc.small_block_count);
	write(1, " blocks in ", 11);
	put_nb(g_ft_malloc.small_zone_count);
	write(1, " zones\n", 7);

	print_used_unused_internal(
		small_used, small_used_percentage,
		small_unused, small_unused_percentage,
		small_internal, small_internal_percentage
	);

	write(1, "Large: ", 7);
	put_nb(large_total / 1024);
	write(1, " Kb (", 5);
	put_nb(large_total_percentage);
	write(1, "%) - ", 6);
	put_nb(g_ft_malloc.large_block_count);
	write(1, " blocks\n", 8);

	print_used_unused_internal(
		large_used, large_used_percentage,
		large_unused, large_unused_percentage,
		large_internal, large_internal_percentage
	);
}
