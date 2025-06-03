#include "malloc.h"

void * ft_realloc(void * ptr, size_t size)
{
	// printf("ft_realloc called with ptr: %p, size: %zu\n", ptr, size);
	if (ptr == NULL)
	{
		return ft_malloc(size);
	}
	if (size == 0)
	{
		ft_free(ptr);
		return NULL;
	}
	AllocatedBlock * current = g_ft_malloc.allocated_blocks;
	while (current != NULL)
	{
		if (current->address == ptr)
		{
			if (current->size >= size)
			{
				// printf("Not reallocating, current block size (%zu) is sufficient for requested size (%zu)\n", current->size, size);
				return ptr;
			}
			void * new_address = allocate_memory(NULL, size);
			if (new_address == NULL)
			{
				perror("Memory allocation failed during realloc");
				return NULL;
			}
			memcpy(new_address, ptr, current->size);
			munmap(current->address, current->size);
			current->address = new_address;
			current->size = size;
			// printf("Memory reallocated from %p to %p, size: %zu bytes\n", ptr, new_address, size);
			return new_address;
		}
		current = current->next;
	}
	return NULL;
}