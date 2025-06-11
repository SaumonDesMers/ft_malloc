#ifndef MALLOC_H
#define MALLOC_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <assert.h>
#include <pthread.h>

#include <stdio.h>


#define ADD_TO_LINKED_LIST(list, header) \
	(header)->prev = NULL; \
	(header)->next = (list); \
	if ((list) != NULL) \
		(list)->prev = (header); \
	(list) = (header);

#define REMOVE_FROM_LINKED_LIST(list, header) \
	if ((list) == (header)) \
		(list) = (header)->next; \
	if ((header)->prev) \
		(header)->prev->next = (header)->next; \
	if ((header)->next) \
		(header)->next->prev = (header)->prev;


#define MIN_ALIGNMENT 16
#define MIN_FRAGMENTATION_SIZE 64

#define ALIGN(value, align) \
	((__uint64_t)((value) + (align - 1)) & ~(align - 1))

#define ALIGNED_HEADER_SIZE ALIGN(sizeof(AllocBlockHeader), MIN_ALIGNMENT)

#define ALIGNED_LARGE_BLOCK_SIZE(size) \
	ALIGN((size) + ALIGNED_HEADER_SIZE, sysconf(_SC_PAGESIZE))

#define TINY_BLOCK_SIZE 256
#define TINY_ALLOC_SIZE (TINY_BLOCK_SIZE - ALIGNED_HEADER_SIZE)
#define SMALL_BLOCK_SIZE 2048
#define SMALL_ALLOC_SIZE (SMALL_BLOCK_SIZE - ALIGNED_HEADER_SIZE)
#define MIN_BLOCK_COUNT_IN_ZONE 128

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

	size_t total_allocated;

} AllocZoneHeader;

typedef struct s_FtMallocGlobal
{
	AllocZoneHeader * tiny_zones;
	AllocZoneHeader * small_zones;
	AllocBlockHeader * used_large_blocks;
	AllocBlockHeader * free_large_blocks;

	pthread_mutex_t mutex;

} FtMallocGlobal;

extern FtMallocGlobal g_ft_malloc;


void * allocate_memory(void * address, size_t size);

AllocZoneHeader * add_alloc_zone(AllocZoneHeader ** first_zone, size_t block_size);
void remove_alloc_zone(AllocZoneHeader ** first_zone, AllocZoneHeader * zone);

void * alloc_block(AllocZoneHeader * first_zone, size_t alloc_size);
void free_block(AllocBlockHeader * block_address);

void * intern_malloc(size_t size);
void * intern_realloc(void * ptr, size_t size);
void intern_free(void * ptr);

#endif // MALLOC_H