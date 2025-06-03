#include "malloc.h"

int add_allocated_block(void *address, size_t size)
{
	AllocatedBlock * new_block = allocate_memory(NULL, sizeof(AllocatedBlock));
	if (!new_block)
	{
		perror("Failed to allocate memory for new block");
		return -1;
	}
	new_block->address = address;
	new_block->size = size;
	new_block->next = g_ft_malloc.allocated_blocks;
	g_ft_malloc.allocated_blocks = new_block;
	return 0;
}

void * ft_malloc(size_t size)
{
	// printf("ft_malloc called with size: %zu\n", size);
	void * address = allocate_memory(NULL, size);
	if (address == MAP_FAILED)
	{
		perror("Memory allocation failed");
		return NULL;
	}
	if (add_allocated_block(address, size) != 0)
	{
		munmap(address, size);
		return NULL;
	}
	// printf("Memory allocated at %p, size: %zu bytes\n", address, size);
	return address;
}
