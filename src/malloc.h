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

typedef struct s_AllocHeader
{
	size_t size; // Size of the allocated block
} AllocHeader;

// typedef struct s_AllocatedBlock
// {
// 	void * address;
// 	size_t size;
// 	struct s_AllocatedBlock * next;
// } AllocatedBlock;

// typedef struct s_AllocatedZone
// {
// 	int is_occupied[ZONE_SIZE];
// 	int alloc_count;
// 	void * address;
// 	size_t size;
// 	struct s_AllocatedZone * next;
// } AllocatedZone;


typedef struct s_FtMallocGlobal
{
	// AllocatedZone * tiny_zones;
	// AllocatedZone * small_zones;
	// AllocatedBlock * allocated_blocks;
} FtMallocGlobal;

extern FtMallocGlobal g_ft_malloc;

void * allocate_memory(void * address, size_t size);

void * ft_malloc(size_t size);
void ft_free(void * ptr);

#endif // MALLOC_H