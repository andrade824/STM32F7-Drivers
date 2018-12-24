/**
 * @author Devon Andrade
 * @created 6/8/2017
 *
 * Contains methods for initializaing and controlling interrupts and the
 * vector table.
 */
#include "config.h"
#include "debug.h"
#include "interrupt.h"
#include "status.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/**
 * Set a pointer to where the vector table will be relocated after
 * init_interrupts() is run.
 *
 * The reason we skip 16 words from the start of where the vector table is
 * copied is because the first word is the start value for the stack pointer
 * and the next 15 words are the exception vectors.
 *
 * CMSIS requires that the IRQ numbers for the exceptions be negative, so to use
 * the IRQ numbers as indices into the vector table, the vector table needs to
 * point to whatever interrupt has IRQ "0" (WWDG_IRQn in this case).
 */
#define EXCEPTION_VECTORS_OFFSET (16 * 4)
static ISR_Type *vector_table = (ISR_Type *)(VECTOR_TABLE_ADDR +
											 EXCEPTION_VECTORS_OFFSET);

/**
 * Copy the vector table from flash into RAM and change the Vector Table Offset
 * Register to reflect this new location.
 *
 * @note This method must be called before any of the interrupt methods,
 *       otherwise undefined behavior may occur.
 */
status_t init_interrupts(void)
{
	/**
	 * Linker script symbols defining where the default vector table located in
	 * flash starts and ends.
	 */
	extern uint32_t _start_vector;
	extern uint32_t _end_vector;

	/**
	 * The base address of the tightly coupled RAM (where the vector table will
	 * be relocated) is at address zero. GCC sees this and thinks we passed in
	 * a null pointer by accident. This suppresses that warning only while
	 * executing the memcpy.
	 */
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wnonnull"

	/**
	 * Copy the vector table from FLASH to tightly coupled RAM.
	 */
	memcpy((void *)VECTOR_TABLE_ADDR,
		   (void *)&_start_vector,
		   &_end_vector - &_start_vector);

	#pragma GCC diagnostic pop

	/**
	 * Configure the Vector Table location.
	 */
	SCB->VTOR = VECTOR_TABLE_ADDR;

	return Success;
}

/**
 * Set the vector table entry for "irq" to point to an interrupt service
 * routine.
 *
 * @param irq The interrupt request number for the ISR to set.
 * @param isr A pointer to a function to act as the interrupt service routine.
 */
status_t request_interrupt(IRQn_Type irq, ISR_Type isr)
{
#ifdef DEBUG_ON
	ABORT_IF(irq <= IRQ_START || irq >= IRQ_END);

	/**
	 * Keep track of which interrupts have already been requested. This method
	 * will Fail if the same interrupt is requested twice.
	 */
	static bool requested_interrupts[IRQ_END - IRQ_START];
	ABORT_IF(requested_interrupts[irq + 15] == true);
	requested_interrupts[irq + 15] = true;
#endif

	vector_table[irq] = isr;

	/**
	 * Exceptions are enabled through a System Control Block register.
	 */
	if(irq >= 0) {
		NVIC_EnableIRQ(irq);
	}

	return Success;
}