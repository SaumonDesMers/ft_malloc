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
	header->size = size; // Store the size in the header
	address = (char *)address + sizeof(AllocHeader); // Adjust address to point to the usable memory

	return address;
}
