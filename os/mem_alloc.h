/**
 * @author Devon Andrade
 * @created 3/28/21
 *
 * Contains functions for dynamically allocating memory.
 */
#pragma once

/* Returned from mem_alloc to denote an allocation failure. */
#define ALLOC_FAILURE ((void*)-1)

void * _sbrk(int incr);
void * mem_alloc(size_t size);
void mem_free(void *data);
