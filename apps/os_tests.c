#include "config.h"
#include "debug.h"
#include "os/mem_alloc.h"

#include <stdint.h>


/**
 * Perform some very simple tests on the "init only, no free" dynamic memory
 * allocator.
 */
void mem_alloc_test(void)
{
#ifdef DEBUG_ON
	extern void *_sdata;
	extern void *_edata;
	extern void *_sbss;
	extern void *_ebss;
#endif /* DEBUG_ON */

	dbprintf("DATA: %p through %p\n", &_sdata, &_edata);
	dbprintf("BSS: %p through %p\n", &_sbss, &_ebss);

	const size_t buffer1_size = 1000;
	uint8_t *buffer1 = (uint8_t*)mem_alloc(buffer1_size);
	dbprintf("Obtained buffer: %p\n", buffer1);
	dbprintf("sbrk() value: %p\n", _sbrk(0));

	/**
	 * Actually write into the buffer. If the allocator didn't work and handed
	 * out memory that is being used by newlib/malloc(), then writing over the
	 * memory will most likely cause the next printf() call to fail.
	 */
	for(size_t i = 0; i < buffer1_size; i++) {
		buffer1[i] = i;
	}

	dbprintf("Test printf() to make sure this still works.\n");

	/* Perform a second allocation for good measure. */
	const size_t buffer2_size = 4000;
	uint8_t *buffer2 = (uint8_t*)mem_alloc(buffer2_size);

	for(size_t i = 0; i < buffer2_size; i++) {
		buffer1[i] = i;
	}

	mem_free(buffer2);

	dbprintf("Test 2 printf() to make sure this still works.\n");

	/* Asking for more RAM than exists should fail. */
	buffer2 = mem_alloc(100000000);
	if(buffer2 != ALLOC_FAILURE) {
		dbprintf("Trying to allocate huge buffer DID NOT fail. Weird %p\n", buffer2);
	} else {
		dbprintf("Allocating huge buffer predictably failed. Good.\n");
	}

	mem_free(buffer1);

	dbprintf("Memory Allocation Test complete\n");
}
