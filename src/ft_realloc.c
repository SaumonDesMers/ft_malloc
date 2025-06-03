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

	AllocHeader * old_header = FROM_BUFFER_TO_HEADER_ADDR(ptr);
	void * new_address = ft_malloc(size);
	if (new_address == NULL)
	{
		return NULL;
	}

	size_t min_size = old_header->size < size ? old_header->size : size;
	memcpy(new_address, ptr, min_size);
	ft_free(ptr);
	
	return new_address;
}