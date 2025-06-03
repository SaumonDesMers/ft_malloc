#include "malloc.h"

void ft_free(void * ptr)
{
	// printf("ft_free called with ptr: %p\n", ptr);
	if (ptr == NULL)
	{
		return;
	}
	AllocatedBlock * prev = NULL;
	AllocatedBlock * current = g_ft_malloc.allocated_blocks;
	while (current != NULL)
	{
		if (current->address == ptr)
		{
			if (prev == NULL)
			{
				g_ft_malloc.allocated_blocks = current->next;
			}
			else
			{
				prev->next = current->next;
			}
			munmap(current->address, current->size);
			munmap(current, sizeof(AllocatedBlock));
			// printf("Memory at %p freed successfully.\n", ptr);
			return;
		}
		prev = current;
		current = current->next;
	}
}