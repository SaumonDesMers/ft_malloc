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

static void print_block_linked_list(AllocBlockHeader * head, size_t * total)
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

		*total += current->size;
	}
}

static void print_zone_linked_list(AllocZoneHeader * head, char * name, size_t * total)
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
		if (current->used_blocks == NULL)
			continue;
		
		write(1, name, ft_strlen(name));
		write(1, " : 0x", 5);
		put_hex((__uint64_t)current);
		write(1, "\n", 1);

		print_block_linked_list(current->used_blocks, total);
	}
}

void show_alloc_mem()
{
	pthread_mutex_lock(&g_ft_malloc.mutex);

	size_t total = 0;

	if (g_ft_malloc.tiny_zones)
	{
		print_zone_linked_list(g_ft_malloc.tiny_zones, "TINY", &total);
	}

	if (g_ft_malloc.small_zones)
	{
		print_zone_linked_list(g_ft_malloc.small_zones, "SMALL", &total);
	}

	if (g_ft_malloc.used_large_blocks)
	{
		write(1, "LARGE :\n", 8);
		print_block_linked_list(g_ft_malloc.used_large_blocks, &total);
	}

	write(1, "Total allocated: ", 17);
	put_nb(total);
	write(1, " bytes\n", 7);
	
	pthread_mutex_unlock(&g_ft_malloc.mutex);
}
