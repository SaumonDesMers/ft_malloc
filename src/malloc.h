#ifndef MALLOC_H
#define MALLOC_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <assert.h>

#define MIN_ALIGNMENT 16

#define ALIGN(value, align) \
	(((value) + (align - 1)) & ~(align - 1))

#define ALIGNED_HEADER_SIZE 32

#define TINY_BLOCK_SIZE 256
#define TINY_ALLOC_SIZE (TINY_BLOCK_SIZE - ALIGNED_HEADER_SIZE)
#define SMALL_BLOCK_SIZE 2048
#define SMALL_ALLOC_SIZE (SMALL_BLOCK_SIZE - ALIGNED_HEADER_SIZE)
#define BLOCK_COUNT_IN_ZONE 256

// Header placed at the beginning of each allocated block.
typedef struct s_AllocBlockHeader
{
	size_t size;
	struct s_AllocBlockHeader * prev;
	struct s_AllocBlockHeader * next;
} AllocBlockHeader;

// Get the address of the memory block following the header
#define FROM_HEADER_TO_BUFFER_ADDR(header) ((void *)((char *)(header) + ALIGNED_HEADER_SIZE))

// Get the address of the header from the memory block address
#define FROM_BUFFER_TO_HEADER_ADDR(buffer) ((AllocBlockHeader *)((char *)(buffer) - ALIGNED_HEADER_SIZE))

// Header for alloc zones, which are a pool of memory blocks of the same size.
// Each zone contains a linked list of free blocks and a linked list of used blocks.
// The header is placed in the first block of each zone.
typedef struct s_AllocZoneHeader
{
	struct s_AllocZoneHeader * prev;
	struct s_AllocZoneHeader * next;
	size_t block_size;
	AllocBlockHeader * free_blocks;
	AllocBlockHeader * used_blocks;

} AllocZoneHeader;

typedef struct s_FtMallocGlobal
{
	AllocZoneHeader * tiny_zones;
	AllocZoneHeader * small_zones;
	AllocBlockHeader * large_blocks;
	size_t total_allocated_by_user;
} FtMallocGlobal;

extern FtMallocGlobal g_ft_malloc;


void * allocate_memory(void * address, size_t size);

AllocZoneHeader * add_alloc_zone(AllocZoneHeader ** first_zone, size_t block_size);
void remove_alloc_zone(AllocZoneHeader ** first_zone, AllocZoneHeader * zone);

void * alloc_block(AllocZoneHeader * first_zone, size_t alloc_size);
void free_block(AllocZoneHeader ** first_zone, AllocBlockHeader * block_address);

void * ft_malloc(size_t size);
void ft_free(void * ptr);

#endif // MALLOC_H