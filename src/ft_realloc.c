#include "malloc.h"

int check_range_in_list(void * ptr, size_t size, AllocBlockHeader * header)
{
	AllocBlockHeader * current = header;
	while (current != NULL)
	{
		char * ptr_start = (char *)ptr - ALIGNED_HEADER_SIZE;
		char * ptr_end = (char *)ptr + size;
		char * block_start = (char *)current;
		char * block_end = (char *)current + current->size + ALIGNED_HEADER_SIZE;
		if (
			ptr_start != block_start && // Ignore the current block itself
			((ptr_start >= block_start && ptr_start < block_end) ||
			(ptr_end > block_start && ptr_end <= block_end) ||
			(block_start >= ptr_start && block_start < ptr_end) ||
			(block_end > ptr_start && block_end <= ptr_end))
		)
		{
			printf("\e[31mError\e[0m: Memory range %p - %p overlaps with allocated block %p - %p\n",
				ptr_start, ptr_end, block_start, block_end);
			return 1;
		}
		current = current->next;
	}
	return 0;
}

int check_range_in_zone(void * ptr, size_t size, AllocZoneHeader * header)
{
	AllocZoneHeader * current_zone = header;
	while (current_zone != NULL)
	{
		if (check_range_in_list(ptr, size, current_zone->used_blocks))
		{
			return 1;
		}
		current_zone = current_zone->next;
	}
	return 0;
}

int check_range(void * ptr, size_t size)
{
	return check_range_in_zone(ptr, size, g_ft_malloc.tiny_zones) ||
		check_range_in_zone(ptr, size, g_ft_malloc.small_zones) ||
		check_range_in_list(ptr, size, g_ft_malloc.used_large_blocks) ||
		check_range_in_list(ptr, size, g_ft_malloc.free_large_blocks);
}


void * realloc(void * ptr, size_t size)
{
	pthread_mutex_lock(&g_ft_malloc.mutex);
	void * new_address = intern_realloc(ptr, size);
	pthread_mutex_unlock(&g_ft_malloc.mutex);
	return new_address;
}

static void ft_memcpy(void * dest, const void * src, size_t n)
{
	char * d = (char *)dest;
	const char * s = (const char *)src;
	for (size_t i = 0; i < n; i++)
	{
		d[i] = s[i];
	}
}

void * intern_realloc(void * ptr, size_t size)
{
	if (ptr == NULL)
	{
		return intern_malloc(size);
	}
	if (size == 0)
	{
		intern_free(ptr);
		return NULL;
	}
	
	AllocBlockHeader * header = FROM_BUFFER_TO_HEADER_ADDR(ptr);
	// Check if old size is greater than new size
	if (header->size >= size)
	{
		return ptr;
	}

	void * new_address = intern_malloc(size);
	if (new_address == NULL)
	{
		return NULL;
	}

	size_t min_size = header->size < size ? header->size : size;
	ft_memcpy(new_address, ptr, min_size);
	intern_free(ptr);

	return new_address;
}