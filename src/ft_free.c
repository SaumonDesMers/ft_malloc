#include "malloc.h"

void free(void * ptr)
{
	if (ptr == NULL)
	{
		return;
	}

	pthread_mutex_lock(&g_ft_malloc.mutex);
	intern_free(ptr);
	pthread_mutex_unlock(&g_ft_malloc.mutex);
}

void intern_free(void * ptr)
{
	AllocBlockHeader * header = FROM_BUFFER_TO_HEADER_ADDR(ptr);

	if (header->size <= TINY_ALLOC_SIZE || header->size <= SMALL_ALLOC_SIZE)
	{
		free_block(header);
	}
	else
	{
		REMOVE_FROM_LINKED_LIST(g_ft_malloc.used_large_blocks, header);
		ADD_TO_LINKED_LIST(g_ft_malloc.free_large_blocks, header);
	}
}