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
	AllocHeader * old_header = (AllocHeader *)((char *)ptr - sizeof(AllocHeader));
	void * new_address = ft_malloc(size);
	if (new_address == NULL)
	{
		return NULL;
	}
	memcpy(new_address, ptr, old_header->size < size ? old_header->size : size);
	ft_free(ptr);
	return new_address;
}