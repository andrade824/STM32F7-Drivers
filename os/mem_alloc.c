/**
 * @author Devon Andrade
 * @created 3/28/21
 *
 * Simple dynamic memory allocation library. The algorithm used is an
 * "init-only, no free" allocator. The expectation is that all dynamic memory
 * allocations would be performed during bootstrap() (potentially influenced by
 * runtime configuration data) and won't be changed after that.
 */
#include "config.h"
#include "debug.h"
#include "os/mem_alloc.h"

#include <stdint.h>

/**
 * Overload the weak linkage of the newlib-provided _sbrk() system call. This
 * call is used by newlib's malloc() to allocate and free memory. This call is
 * also used by the mem_alloc() functions in this file to provide a very simple
 * init-only, no-free allocator. By using the same _sbrk() system call as the
 * newlib malloc(), it ensures that allocating heap memory outside of malloc()
 * does not clobber malloc() memory.
 *
 * The malloc() call is used by various newlib functions (like printf).
 *
 * @param incr The number of bytes to increment the heap by. Can be negative to
 *             return memory to the heap, or zero just to return back the
 *             current address of the heap.
 *
 * @return Previous address of the heap before incrementing on successful
 *         allocation, or ALLOC_FAILURE if the allocation failed. Typically an
 *         allocation fails when the heap grows into the stack.
 */
void * _sbrk(int incr) {
	/**
	 * The heap begins after the BSS section in RAM. This symbol is created by
	 * the linker script. It's easiest to think of this as a variable that
	 * points to the first word on the heap. Get the address of this variable to
	 * determine where the heap should begin.
	 */
	extern uint8_t _end;

	/**
	 * Symbol created by the linker script to represent the top of the kernel
	 * stack (it grows downward). Generally this represents the top of RAM.
	 */
	extern const void *_kern_stack_top;

	/* Static variable to keep track of the current heap pointer. */
	static uint8_t *heap_current = 0;

	if (heap_current == 0) {
		heap_current = &_end;
	}

	uint8_t *prev_heap = heap_current;

	/**
	 * Ensure the heap isn't growing into the kernel stack. This isn't entirely
	 * accurate since the kernel stack could be larger than this value, but it
	 * does provide at least a minimum size the kernel stack can be without
	 * needing to worry that the heap will grow into it.
	 */
	if(((uintptr_t)heap_current + incr) >= ((uintptr_t)&_kern_stack_top - MIN_KERN_STACK_SIZE)) {
		return ALLOC_FAILURE;
	}

	heap_current += incr;

	return (void*)prev_heap;
}

/**
 * Allocate a chunk of memory from the heap.
 *
 * @param size The number of bytes to allocate from the heap. Can't be zero.
 *
 * @return A pointer to the beginning of the dynamically allocated memory on
 *         success, or ALLOC_FAILURE if the memory could not be allocated.
 */
void * mem_alloc(size_t size)
{
	ASSERT(size > 0);

	return _sbrk(size);
}

/**
 * This allocator doesn't support the freeing of memory which means there can't
 * be any memory fragmentation. This function is left in here as a dummy
 * function so that if the algorithm changes, none of the client code will need
 * to be updated.
 *
 * It's expected that consumers of this API will use this function to "free"
 * their buffers if needed even though nothing will actually happen.
 *
 * @param data The previously allocated heap memory to free. Cannot be NULL.
 */
void mem_free(__unused void *data)
{
	/* Intentionally left blank. */
	ASSERT(data != NULL);
}
