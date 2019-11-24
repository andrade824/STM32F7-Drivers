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

#include "registers/scb_reg.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static void nmi_exc(void)
{
	ABORT("Non-Maskable Interrupt triggered... that's weird\n");
}

/**
 * Default Hard Fault exception handler.
 */
static void hard_fault_exc(void)
{
	dbprintf("Hard Fault Exception triggered\n");

	dbprintf("CFSR Dump: 0x%lx\n", SCB->CFSR);
	dbprintf("HFSR Dump: 0x%lx\n", SCB->HFSR);

	if(GET_SCB_HFSR_VECTTBL(SCB->HFSR)) {
		dbprintf("A BusFault occurred on a vector table read.\n");
	}

	if(GET_SCB_HFSR_FORCED(SCB->HFSR)) {
		dbprintf("Hard Fault generated by escalation of a fault with "
		         "configurable priority that cannot be handled, either because "
		         "of priority or becuase it is disabled.\n");
		dbprintf("Check CFSR dump for more information as to the real reason.\n");
	}

	die();
}

/**
 * Default Memory Management exception handler.
 */
static void mem_manage_exc(void)
{
	dbprintf("Memory Management Exception triggered\n");

	dbprintf("CFSR Dump: 0x%lx\n", SCB->CFSR);

	if(GET_SCB_CFSR_IACCVIOL(SCB->CFSR)) {
		dbprintf("The processor attempted an instruction fetch from a location "
		         "that does not permit execution.\n");
	}

	if(GET_SCB_CFSR_DACCVIOL(SCB->CFSR)) {
		dbprintf("The processor attempted a load or store at a location that "
		         "does not permit the operation.\n");
	}

	if(GET_SCB_CFSR_MUNSTKERR(SCB->CFSR)) {
		dbprintf("Unstack for an exception return has caused one or more access "
		         "violations.\n");
	}

	if(GET_SCB_CFSR_MSTKERR(SCB->CFSR)) {
		dbprintf("Stacking for an exception entry has caused one or more access "
		         "violations.\n");
	}

	if(GET_SCB_CFSR_MLSPERR(SCB->CFSR)) {
		dbprintf("A MemManage fault occurred during floating-point lazy state "
		         "preservation.\n");
	}

	if(GET_SCB_CFSR_MMARVALID(SCB->CFSR)) {
		dbprintf("The MemManage fault address: 0x%lx\n", SCB->MMFAR);
	}

	die();
}

/**
 * Default Bus Fault exception handler.
 */
static void bus_fault_exc(void)
{
	dbprintf("Bus Fault Exception triggered\n");

	dbprintf("CFSR Dump: 0x%lx\n", SCB->CFSR);

	if(GET_SCB_CFSR_IBUSERR(SCB->CFSR)) {
		dbprintf("Instruction bus error.\n");
	}

	if(GET_SCB_CFSR_PRECISERR(SCB->CFSR)) {
		dbprintf("Precise data bus error at address 0x%lx.\n", SCB->BFAR);
	}

	if(GET_SCB_CFSR_IMPRECISERR(SCB->CFSR)) {
		dbprintf("Imprecise data bus error\n");
	}

	if(GET_SCB_CFSR_UNSTKERR(SCB->CFSR)) {
		dbprintf("Unstack for an exception return has caused one or more "
		         "BusFaults.\n");
	}

	if(GET_SCB_CFSR_STKERR(SCB->CFSR)) {
		dbprintf("Stacking for an exception entry has caused one or more "
		         "BusFaults.\n");
	}

	if(GET_SCB_CFSR_LSPERR(SCB->CFSR)) {
		dbprintf("A bus fault occurred during floating-point lazy state "
		         "preservation.\n");
	}

	if(GET_SCB_CFSR_BFARVALID(SCB->CFSR)) {
		dbprintf("The Bus fault address: 0x%lx\n", SCB->BFAR);
	}

	die();
}

/**
 * Default Usage Fault exception handler.
 */
static void usage_fault_exc(void)
{
	dbprintf("Usage Fault Exception triggered\n");

	dbprintf("CFSR Dump: 0x%lx\n", SCB->CFSR);

	if(GET_SCB_CFSR_UNDEFINSTR(SCB->CFSR)) {
		dbprintf("The processor has attempted to execute an undefined "
		         "instruction.\n");
	}

	if(GET_SCB_CFSR_INVSTATE(SCB->CFSR)) {
		dbprintf("The processor has attempted to execute an instruction that "
		         "makes illegal use of the EPSR.\n");
	}

	if(GET_SCB_CFSR_INVPC(SCB->CFSR)) {
		dbprintf("The processor has attempted an illegal load of EXC_RETURN to "
		         "the PC, as a result of an invalid context, or an invalid "
		         "EXC_RETURN value.\n");
	}

	if(GET_SCB_CFSR_NOCP(SCB->CFSR)) {
		dbprintf("The processor has attempted to access a coprocessor and "
		         "coprocessor instructions are not supported.\n");
	}

	if(GET_SCB_CFSR_UNALIGNED(SCB->CFSR)) {
		dbprintf("The processor has made an unaligned memory access.\n");
	}

	if(GET_SCB_CFSR_DIVBYZERO(SCB->CFSR)) {
		dbprintf("The processor has executed an SDIV or UDIV instruction with a "
		         "divisor of 0.\n");
	}

	die();
}

/**
 * Set a pointer to where the vector table will be relocated after
 * interrupt_init() is run.
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
static ISR_Type *vector_table =
    (ISR_Type *)(VECTOR_TABLE_ADDR + EXCEPTION_VECTORS_OFFSET);

/**
 * Copy the vector table from flash into RAM and change the Vector Table Offset
 * Register to reflect this new location.
 *
 * @note This method must be called before any of the interrupt methods,
 *       otherwise undefined behavior may occur.
 */
void interrupt_init(void)
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

	/* Copy the vector table from FLASH to tightly coupled RAM. */
	memcpy((void *)VECTOR_TABLE_ADDR,
	       (void *)&_start_vector,
	       &_end_vector - &_start_vector);

	#pragma GCC diagnostic pop

	/* Setup default exception handlers. */
	vector_table[NonMaskableInt_IRQn]   = &nmi_exc;
	vector_table[HardFault_IRQn]        = &hard_fault_exc;
	vector_table[MemoryManagement_IRQn] = &mem_manage_exc;
	vector_table[BusFault_IRQn]         = &bus_fault_exc;
	vector_table[UsageFault_IRQn]       = &usage_fault_exc;

	/* Enable the exception handlers. */
	SET_FIELD(SCB->SHCSR, SCB_SHCSR_MEMFAULTENA() |
	                      SCB_SHCSR_BUSFAULTENA() |
	                      SCB_SHCSR_USGFAULTENA());

	/* Enable Divide by zero and unaligned access exceptions. */
	SET_FIELD(SCB->CCR, SCB_CCR_UNALIGN_TRP() | SCB_CCR_DIV_0_TRP());

	/* Configure the Vector Table location. */
	SCB->VTOR = VECTOR_TABLE_ADDR;
}

/**
 * Set the vector table entry for "irq" to point to an interrupt service
 * routine.
 *
 * @param irq The interrupt request number for the ISR to set.
 * @param isr A pointer to a function to act as the interrupt service routine.
 */
void interrupt_request(IRQn_Type irq, ISR_Type isr)
{
#ifdef DEBUG_ON
	ABORT_IF(irq <= IRQ_START || irq >= IRQ_END);

	/**
	 * Keep track of which interrupts have already been requested. This method
	 * will ABORT if the same interrupt is requested twice.
	 */
	static bool requested_interrupts[IRQ_END - IRQ_START];
	ABORT_IF(requested_interrupts[irq + 15] == true);
	requested_interrupts[irq + 15] = true;
#endif

	vector_table[irq] = isr;

	/**
	 * Exceptions are enabled through a System Control Block register.
	 *
	 * Default exception handlers are enabled in the interrupt_init() method.
	 */
	if(irq >= 0) {
		NVIC_EnableIRQ(irq);
	}
}