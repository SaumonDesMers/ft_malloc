#ifndef MALLOC_H
#define MALLOC_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#define MIN_ALIGNMENT 16

#define ALIGN(value, align) \
	(((value) + (align - 1)) & ~(align - 1))

#define ALIGNED_HEADER_SIZE 32

#define TINY_MAX_SIZE 128
#define SMALL_MAX_SIZE 1024
#define BLOCK_COUNT_IN_ZONE 128

// Header placed at the beginning of each allocated block.
typedef struct s_AllocHeader
{
	size_t size;
	struct s_AllocHeader * prev;
	struct s_AllocHeader * next;
} AllocHeader;

// Get the address of the memory block following the header
#define FROM_HEADER_TO_BUFFER_ADDR(header) ((void *)((char *)(header) + ALIGNED_HEADER_SIZE))

// Get the address of the header from the memory block address
#define FROM_BUFFER_TO_HEADER_ADDR(buffer) ((AllocHeader *)((char *)(buffer) - ALIGNED_HEADER_SIZE))

// Header for alloc zones, which are used to manage memory blocks of different sizes.
// The header is placed in the first two block of each zone.
// It contains a pointer to the next zone and a linked list of free blocks.
typedef struct s_AllocZone
{
	struct s_AllocZone * prev;
	struct s_AllocZone * next;
	size_t block_size;
	size_t free_block_count;

	// Linked list of free blocks in this zone.
	// Each byte points to the index of the next free block.
	// The first two blocks are reserved for the header.
	// Index 0 is the start of the list.
	// Index 1 is reserved for the header.
	__uint8_t free_blocks[BLOCK_COUNT_IN_ZONE];

} AllocZone;

typedef struct s_FtMallocGlobal
{
	AllocZone * tiny_zones;
	AllocHeader * small_blocks;
	AllocHeader * large_blocks;
} FtMallocGlobal;

extern FtMallocGlobal g_ft_malloc;


void * allocate_memory(void * address, size_t size);

AllocZone * add_alloc_zone(AllocZone ** first_zone, size_t block_size);
void remove_alloc_zone(AllocZone ** first_zone, AllocZone * zone);

void * alloc_block(AllocZone ** first_zone);
void free_block(AllocZone ** first_zone, void * block_address);

void * ft_malloc(size_t size);
void ft_free(void * ptr);

#endif // MALLOC_H