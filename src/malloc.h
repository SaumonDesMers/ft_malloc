#ifndef MALLOC_H
#define MALLOC_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <assert.h>

#define MIN_ALIGNMENT 16

#define ALIGN(value, align) \
	(((value) + (align - 1)) & ~(align - 1))

#define ALIGNED_HEADER_SIZE 32

#define TINY_BLOCK_SIZE 512
#define TINY_ALLOC_SIZE (TINY_BLOCK_SIZE - ALIGNED_HEADER_SIZE)
#define SMALL_BLOCK_SIZE 4096
#define SMALL_ALLOC_SIZE (SMALL_BLOCK_SIZE - ALIGNED_HEADER_SIZE)
#define MIN_BLOCK_COUNT_IN_ZONE 4

#define MEM_USED_BY_ZONE_HEADERS(block_size) \
	((block_size) + ALIGNED_HEADER_SIZE * (MIN_BLOCK_COUNT_IN_ZONE - 1))

struct s_AllocZoneHeader;

// Header placed at the beginning of each allocated block.
typedef struct s_AllocBlockHeader
{
	size_t size;
	struct s_AllocBlockHeader * prev;
	struct s_AllocBlockHeader * next;
	struct s_AllocZoneHeader * zone; // Address of the zone this block belongs to, if applicable.
} AllocBlockHeader;

// Get the address of the memory block following the header
#define FROM_HEADER_TO_BUFFER_ADDR(header) ((void *)((char *)(header) + ALIGNED_HEADER_SIZE))

// Get the address of the header from the memory block address
#define FROM_BUFFER_TO_HEADER_ADDR(buffer) ((AllocBlockHeader *)((char *)(buffer) - ALIGNED_HEADER_SIZE))

// Header for alloc zones, which are pools of memory blocks of the same size.
// The header is placed in the first block of each zone.
// Each zone contains a linked list of free blocks and a linked list of used blocks.
typedef struct s_AllocZoneHeader
{
	struct s_AllocZoneHeader * prev;
	struct s_AllocZoneHeader * next;
	AllocBlockHeader * free_blocks;
	AllocBlockHeader * used_blocks;

	size_t block_size;
	size_t block_count;
	size_t total_allocated;

} AllocZoneHeader;

typedef struct s_FtMallocGlobal
{
	AllocZoneHeader * tiny_zones;
	AllocZoneHeader * small_zones;
	AllocBlockHeader * large_blocks;

	size_t tiny_zone_count;
	size_t tiny_block_count;
	size_t tiny_allocated;
	size_t tiny_allocated_used;

	size_t small_zone_count;
	size_t small_block_count;
	size_t small_allocated;
	size_t small_allocated_used;

	size_t large_block_count;
	size_t large_allocated;
	size_t large_allocated_used;

} FtMallocGlobal;

extern FtMallocGlobal g_ft_malloc;


void * allocate_memory(void * address, size_t size);

AllocZoneHeader * add_alloc_zone(AllocZoneHeader ** first_zone, size_t block_size);
void remove_alloc_zone(AllocZoneHeader ** first_zone, AllocZoneHeader * zone);

void * alloc_block(AllocZoneHeader * first_zone, size_t alloc_size);
void free_block(AllocBlockHeader * block_address);

void * ft_malloc(size_t size);
void ft_free(void * ptr);

#endif // MALLOC_H