#include "malloc.h"

FtMallocGlobal g_ft_malloc = {
	.allocated_blocks = NULL
};

void show_alloc_mem()
{
	printf("Memory allocation status:\n");
	AllocatedBlock *current = g_ft_malloc.allocated_blocks;
	while (current != NULL)
	{
		printf("Block address: %p, size: %zu bytes\n", current->address, current->size);
		current = current->next;
	}
}

void * allocate_memory(void * address, size_t size)
{
	return mmap(address, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}