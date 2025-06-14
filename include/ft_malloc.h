#ifndef FT_MALLOC_H
#define FT_MALLOC_H

#include <stdlib.h>

void free(void * ptr);
void * malloc(size_t size);
void * realloc(void * ptr, size_t size);

void show_alloc_mem();

#endif // FT_MALLOC_H