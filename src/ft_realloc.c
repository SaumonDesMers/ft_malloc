#include "malloc.h"

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

	if ((header->size <= TINY_ALLOC_SIZE && size <= TINY_ALLOC_SIZE) ||
	    (header->size <= SMALL_ALLOC_SIZE && size <= SMALL_ALLOC_SIZE))
	{
		// If the size is still within the tiny or small allocation size, we can reuse the block.
		return ptr;
	}


	void * new_address = ft_malloc(size);
	if (new_address == NULL)
	{
		return NULL;
	}

	size_t min_size = header->size < size ? header->size : size;
	memcpy(new_address, ptr, min_size);
	ft_free(ptr);
	
	return new_address;
}