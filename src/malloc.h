#ifndef MALLOC_H
#define MALLOC_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

// #define TINY_MAX_SIZE 128
// #define SMALL_MAX_SIZE 1024
// #define ZONE_SIZE 128

// Header placed at the beginning of each allocated block.
typedef struct s_AllocHeader
{
	size_t size;
	struct s_AllocHeader * prev;
	struct s_AllocHeader * next;
} AllocHeader;

// Get the address of the memory block following the header
#define FROM_HEADER_TO_BUFFER_ADDR(header) ((void *)((char *)(header) + sizeof(AllocHeader)))

// Get the address of the header from the memory block address
#define FROM_BUFFER_TO_HEADER_ADDR(buffer) ((AllocHeader *)((char *)(buffer) - sizeof(AllocHeader)))


typedef struct s_FtMallocGlobal
{
	AllocHeader * allocated_blocks;
} FtMallocGlobal;

extern FtMallocGlobal g_ft_malloc;


void * allocate_memory(void * address, size_t size);

void * ft_malloc(size_t size);
void ft_free(void * ptr);

#endif // MALLOC_H