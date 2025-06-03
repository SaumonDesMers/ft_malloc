#ifndef MALLOC_H
#define MALLOC_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>


typedef struct s_AllocatedBlock
{
	void * address;
	size_t size;
	struct s_AllocatedBlock * next;
} AllocatedBlock;


typedef struct s_FtMallocGlobal
{
	AllocatedBlock * allocated_blocks;
} FtMallocGlobal;

extern FtMallocGlobal g_ft_malloc;

void * allocate_memory(void * address, size_t size);

void * ft_malloc(size_t size);
void ft_free(void * ptr);

#endif // MALLOC_H