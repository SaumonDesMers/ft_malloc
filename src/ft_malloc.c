#include "malloc.h"

void * ft_malloc(size_t size)
{
	void * address = allocate_memory(NULL, size + sizeof(AllocHeader));
	if (address == MAP_FAILED)
	{
		perror("Memory allocation failed");
		return NULL;
	}

	AllocHeader * header = (AllocHeader *)address;
	header->size = size;
	header->prev = NULL;
	header->next = g_ft_malloc.allocated_blocks;
	g_ft_malloc.allocated_blocks = header;
	if (header->next != NULL)
	{
		header->next->prev = header;
	}

	return FROM_HEADER_TO_BUFFER_ADDR(address);
}
