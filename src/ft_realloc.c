#include "malloc.h"

static void ft_memcpy(void * dest, const void * src, size_t n)
{
	char * d = (char *)dest;
	const char * s = (const char *)src;
	for (size_t i = 0; i < n; i++)
	{
		d[i] = s[i];
	}
}

void * ft_realloc(void * ptr, size_t size)
{
	if (ptr == NULL)
	{
		return ft_malloc(size);
	}
	if (size == 0)
	{
		ft_free(ptr);
		return NULL;
	}

	AllocBlockHeader * header = FROM_BUFFER_TO_HEADER_ADDR(ptr);
	if (header->size >= size)
	{
		return ptr;
	}
	
	if (header->size <= TINY_ALLOC_SIZE && size <= TINY_ALLOC_SIZE)
	{
		header->size = size;
		g_ft_malloc.tiny_allocated_used -= (header->size - size);
		return ptr;
	}
	if (header->size <= SMALL_ALLOC_SIZE && header->size > TINY_ALLOC_SIZE && size <= SMALL_ALLOC_SIZE)
	{
		header->size = size;
		g_ft_malloc.small_allocated_used -= (header->size - size);
		return ptr;
	}


	void * new_address = ft_malloc(size);
	if (new_address == NULL)
	{
		return NULL;
	}

	size_t min_size = header->size < size ? header->size : size;
	ft_memcpy(new_address, ptr, min_size);
	ft_free(ptr);
	
	return new_address;
}