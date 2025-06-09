#include "malloc.h"

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
	// Check if new size and old size are both in the tiny allocation range
	if (header->size <= TINY_ALLOC_SIZE && size <= TINY_ALLOC_SIZE)
	{
		header->size = size;
		g_ft_malloc.tiny_allocated_used -= (header->size - size);
		return ptr;
	}
	// Check if new size and old size are both in the small allocation range
	if (header->size <= SMALL_ALLOC_SIZE && header->size > TINY_ALLOC_SIZE && size <= SMALL_ALLOC_SIZE)
	{
		header->size = size;
		g_ft_malloc.small_allocated_used -= (header->size - size);
		return ptr;
	}
	// Check if the old size is large and the new size is also large, but fits within the same page
	if (header->size > SMALL_ALLOC_SIZE
		&& size <= ALIGN(header->size + ALIGNED_HEADER_SIZE, sysconf(_SC_PAGESIZE)) - ALIGNED_HEADER_SIZE)
	{
		header->size = size;
		g_ft_malloc.large_allocated_used -= (header->size - size);
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