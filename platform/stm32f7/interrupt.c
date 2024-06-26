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
#include "system.h"

#include "registers/nvic_reg.h"
#include "registers/scb_reg.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/**
 * When an exception occurs, the current state of the CPU will be dumped onto
 * the stack in the exact order represented in this structure.
 *
 * @note The layout of this structure needs to match exactly with the way that
 *       the registers are pushed onto the stack in the PUSH_REGISTERS macro.
 */
typedef struct {
	/* Link Register (return address) of the function that was interrupted. */
	uint32_t lr;

	/* The instruction that caused the exception. */
	uint32_t pc;

	/* Processor state of the function that got interrupted. */
	uint32_t psr;

	/* Registers r0-r12. */
	uint32_t r[13];
} arm_exception_context_t;

/**
 * Dump an ARM exception context to the log.
 *
 * @param context Register state pushed in the first level exception handler.
 */
static void dump_exc_context(__unused arm_exception_context_t *context)
{
	dbprintf("\n r0: 0x%08lx, r1: 0x%08lx,  r2: 0x%08lx,   r3: 0x%08lx,\n"
	         " r4: 0x%08lx, r5: 0x%08lx,  r6: 0x%08lx,   r7: 0x%08lx,\n"
	         " r8: 0x%08lx, r9: 0x%08lx, r10: 0x%08lx,  r11: 0x%08lx,\n"
	         "r12: 0x%08lx, LR: 0x%08lx,  PC: 0x%08lx, xPSR: 0x%lx\n\n",
	         context->r[0], context->r[1], context->r[2], context->r[3],
	         context->r[4], context->r[5], context->r[6], context->r[7],
	         context->r[8], context->r[9], context->r[10], context->r[11],
	         context->r[12], context->lr, context->pc, context->psr);
}

/**
 * Exception handler used for exceptions that we don't expect to get triggered.
 */
void no_exc_handler(arm_exception_context_t *context)
{
	ABORT("\n!!!!! Unexpected exception got triggered... !!!!!\n");

	dump_exc_context(context);
}

/**
 * Default Hard Fault exception handler.
 */
void hard_fault_exc(arm_exception_context_t *context)
{
	dbprintf("\n!!!!! Hard Fault Exception triggered !!!!!\n");

	dump_exc_context(context);

	dbprintf("CFSR Dump: 0x%lx\n", SCB->CFSR);
	dbprintf("HFSR Dump: 0x%lx\n", SCB->HFSR);

	if(GET_SCB_HFSR_VECTTBL(SCB->HFSR)) {
		dbprintf("A BusFault occurred on a vector table read.\n");
	}

	if(GET_SCB_HFSR_FORCED(SCB->HFSR)) {
		dbprintf("Hard Fault generated by escalation of a fault with "
		         "configurable priority that cannot be handled, either because "
		         "of priority or because it is disabled.\n");
		dbprintf("Check CFSR dump for more information as to the real reason.\n");
	}

	die();
}

/**
 * Default Memory Management exception handler.
 */
void mem_manage_exc(arm_exception_context_t *context)
{
	dbprintf("\n!!!!! Memory Management Exception triggered !!!!!\n");

	dump_exc_context(context);

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
void bus_fault_exc(arm_exception_context_t *context)
{
	dbprintf("\n!!!!! Bus Fault Exception triggered !!!!!\n");

	dump_exc_context(context);

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
void usage_fault_exc(arm_exception_context_t *context)
{
	dbprintf("\n!!!!! Usage Fault Exception triggered !!!!!\n");

	dump_exc_context(context);

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
 * Set a pointer to where the vector table will be relocated after intr_init()
 * is run.
 *
 * The reason we skip 16 words from the start of where the vector table is
 * copied is because the first word is the start value for the stack pointer
 * and the next 15 words are the exception vectors.
 *
 * The IRQ numbers for the internal exceptions are negative, so to use the IRQ
 * numbers as indices into the vector table, the vector table needs to point to
 * whatever interrupt has IRQ "0" (WWDG_IRQn in this case).
 */
#define EXCEPTION_VECTORS_OFFSET (16 * 4)
static isr_func_t *vector_table =
    (isr_func_t *)(VECTOR_TABLE_ADDR + EXCEPTION_VECTORS_OFFSET);

/**
 * IRQ definitions for all of the internal exceptions. The vector table is setup
 * to point to the first external interrupt handler, so negative IRQ numbers are
 * needed to access the internal exception handlers.
 */
enum {
	Reset_IRQn            = -15, /* 1 Reset Handler                         */
	NonMaskableInt_IRQn   = -14, /* 2 Non Maskable Interrupt                */
	HardFault_IRQn        = -13, /* 3 Cortex-M7 Hard Fault Interrupt        */
	MemoryManagement_IRQn = -12, /* 4 Cortex-M7 Memory Management Interrupt */
	BusFault_IRQn         = -11, /* 5 Cortex-M7 Bus Fault Interrupt         */
	UsageFault_IRQn       = -10, /* 6 Cortex-M7 Usage Fault Interrupt       */
	SVCall_IRQn           = -5,  /* 11 Cortex-M7 SV Call Interrupt          */
	DebugMonitor_IRQn     = -4,  /* 12 Cortex-M7 Debug Monitor Interrupt    */
	PendSV_IRQn           = -2,  /* 14 Cortex-M7 Pend SV Interrupt          */
	SysTick_IRQn          = -1,  /* 15 Cortex-M7 System Tick Interrupt      */
};

/**
 * Copy the vector table from flash into RAM and change the Vector Table Offset
 * Register to reflect this new location.
 *
 * @note This method must be called before any of the interrupt methods,
 *       otherwise undefined behavior may occur.
 */
void intr_init(void)
{
	/* The reset handler is located in the startup script. */
	extern void Reset_Handler(void);

	/**
	 * Setup default internal exception handlers. The first level of the
	 * exception handlers is handled in assembly so as to properly save off all
	 * of the registers. The second level handlers located in this file will be
	 * called after the registers have been saved.
	 */
	extern void no_exc_handler_asm(void);
	extern void hard_fault_asm(void);
	extern void mem_manage_asm(void);
	extern void bus_fault_asm(void);
	extern void usage_fault_asm(void);

	vector_table[Reset_IRQn]            = &Reset_Handler;
	vector_table[NonMaskableInt_IRQn]   = &no_exc_handler_asm;
	vector_table[HardFault_IRQn]        = &hard_fault_asm;
	vector_table[MemoryManagement_IRQn] = &mem_manage_asm;
	vector_table[BusFault_IRQn]         = &bus_fault_asm;
	vector_table[UsageFault_IRQn]       = &usage_fault_asm;
	vector_table[DebugMonitor_IRQn]     = &no_exc_handler_asm;

	/* Zero out the internal exception vectors which can be registered separately. */
	vector_table[SysTick_IRQn] = NULL;
	vector_table[PendSV_IRQn] = NULL;
	vector_table[SVCall_IRQn] = NULL;

	/* Zero out the external interrupt vectors. */
	memset((void*)vector_table, 0, IRQ_END * 4);

	/* Enable the default exception handlers. */
	SET_FIELD(SCB->SHCSR, SCB_SHCSR_MEMFAULTENA() |
	                      SCB_SHCSR_BUSFAULTENA() |
	                      SCB_SHCSR_USGFAULTENA());

	/* Enable Divide by zero and unaligned access exceptions. */
	SET_FIELD(SCB->CCR, SCB_CCR_UNALIGN_TRP() | SCB_CCR_DIV_0_TRP());

	/* Configure the Vector Table location. */
	SCB->VTOR = VECTOR_TABLE_ADDR;
}

/**
 * Enable all exceptions with configurable priority (including all external
 * interrupts).
 */
void intr_enable_interrupts(void)
{
	asm volatile("cpsie i" ::: "memory");
}

/**
 * Disable all exceptions with configurable priority (including all external
 * interrupts).
 */
void intr_disable_interrupts(void)
{
	asm volatile("cpsid i" ::: "memory");
}

/**
 * Set the vector table entry for [irq] to point to an interrupt service
 * routine, configure that interrupt's priority, and enable that interrupt.
 *
 * @note The ISR for interrupt [irq] must not have already been registered.
 *
 * @param irq The interrupt request number for the ISR to set.
 * @param isr A pointer to a function to act as the interrupt service routine.
 * @param priority The priority level of the interrupt where zero is the highest
 *                 priority level and LOWEST_INTR_PRIORITY is the lowest.
 *                 Priority zero is reserved for static internal exception
 *                 handlers. If priority zero was allowed to be chosen, an
 *                 exception caused by the registered handler wouldn't be able
 *                 to execute because it would be at the same priority as the
 *                 code that caused it.
 */
void intr_register(irq_num_t irq, isr_func_t isr, uint8_t priority)
{
	ASSERT(irq < IRQ_END);
	ASSERT(isr != NULL);
	ASSERT((priority > 0) && priority <= LOWEST_INTR_PRIORITY);
	ASSERT(vector_table[irq] == NULL);

	/* Set the priority level for this interupt. */
	NVIC->IP[irq] |= SET_NVIC_IP_PRIORITY(priority << (8U - INTR_PRIORITY_BITS));

	vector_table[irq] = isr;

	/**
	 * Enable the interrupt in the NVIC. The peripheral will still need to be
	 * configured to generate the interrupt properly.
	 */
	intr_enable_irq(irq);
}

/**
 * Set the SVCall exception handler and exception priority level.
 *
 * @note The SVCall exception must have not already been registered.
 *
 * @note Priority zero is reserved for the static internal exception handlers.
 *       If priority zero was chosen, an exception caused in the SVCall handler
 *       wouldn't be able to execute because it would be at the same priority.
 *
 * @param isr A pointer to the function to act as the interrupt service routine.
 * @param priority The priority level of the interrupt where zero is the highest
 *                 priority level and LOWEST_INTR_PRIORITY is the lowest.
 *                 Priority zero is reserved for static internal exception
 *                 handlers. If priority zero was allowed to be chosen, an
 *                 exception caused by the registered handler wouldn't be able
 *                 to execute because it would be at the same priority as the
 *                 code that caused it.
 */
void intr_register_svcall(isr_func_t isr, uint8_t priority)
{
	ASSERT(isr != NULL);
	ASSERT((priority > 0) && (priority <= LOWEST_INTR_PRIORITY));
	ASSERT(vector_table[SVCall_IRQn] == NULL);

	/* Set the priority level. */
	SCB->SHPR[7] = SET_SCB_SHPR_PRI(priority << (8U - INTR_PRIORITY_BITS));

	vector_table[SVCall_IRQn] = isr;
}

/**
 * Set the PendSV exception handler and exception priority level.
 *
 * @note The PendSV exception must have not already been registered.
 *
 * @note Priority zero is reserved for the static internal exception handlers.
 *       If priority zero was chosen, an exception caused in the PendSV handler
 *       wouldn't be able to execute because it would be at the same priority.
 *
 * @param isr A pointer to the function to act as the interrupt service routine.
 * @param priority The priority level of the interrupt where zero is the highest
 *                 priority level and LOWEST_INTR_PRIORITY is the lowest.
 *                 Priority zero is reserved for static internal exception
 *                 handlers. If priority zero was allowed to be chosen, an
 *                 exception caused by the registered handler wouldn't be able
 *                 to execute because it would be at the same priority as the
 *                 code that caused it.
 */
void intr_register_pendsv(isr_func_t isr, uint8_t priority)
{
	ASSERT(isr != NULL);
	ASSERT((priority > 0) && (priority <= LOWEST_INTR_PRIORITY));
	ASSERT(vector_table[PendSV_IRQn] == NULL);

	/* Set the priority level. */
	SCB->SHPR[10] = SET_SCB_SHPR_PRI(priority << (8U - INTR_PRIORITY_BITS));

	vector_table[PendSV_IRQn] = isr;
}

/**
 * Set the SysTick timer's interrupt handler and interrupt priority level.
 *
 * @note The SysTick interrupt must have not already been registered.
 *
 * @note Priority zero is reserved for the static internal exception handlers.
 *       If priority zero was chosen, an exception caused in the SysTick handler
 *       wouldn't be able to execute because it would be at the same priority.
 *
 * @param isr A pointer to the function to act as the interrupt service routine.
 * @param priority The priority level of the interrupt where zero is the highest
 *                 priority level and LOWEST_INTR_PRIORITY is the lowest.
 *                 Priority zero is reserved for static internal exception
 *                 handlers. If priority zero was allowed to be chosen, an
 *                 exception caused by the registered handler wouldn't be able
 *                 to execute because it would be at the same priority as the
 *                 code that caused it.
 */
void intr_register_systick(isr_func_t isr, uint8_t priority)
{
	ASSERT(isr != NULL);
	ASSERT((priority > 0) && (priority <= LOWEST_INTR_PRIORITY));
	ASSERT(vector_table[SysTick_IRQn] == NULL);

	/* Set the priority level. */
	SCB->SHPR[11] = SET_SCB_SHPR_PRI(priority << (8U - INTR_PRIORITY_BITS));

	vector_table[SysTick_IRQn] = isr;

	/* Enabling the SysTick interupt will be done by the SysTick driver. */
}

/**
 * Enable a specific IRQ's interrupt.
 *
 * @note The ISR for this IRQ must have already been registered.
 *
 * @param irq The IRQ to enable.
 */
void intr_enable_irq(irq_num_t irq)
{
	ASSERT(irq < IRQ_END);
	ASSERT(vector_table[irq] != NULL);

	NVIC->ISER[NVIC_REG_SELECT(irq)] |= (1 << NVIC_BIT_SELECT(irq));
}

/**
 * Disable a specific IRQ's interrupt.
 *
 * @note The ISR for this IRQ must have already been registered.
 *
 * @param irq The IRQ to disable.
 */
void intr_disable_irq(irq_num_t irq)
{
	ASSERT(irq < IRQ_END);
	ASSERT(vector_table[irq] != NULL);

	NVIC->ICER[NVIC_REG_SELECT(irq)] |= (1 << NVIC_BIT_SELECT(irq));
}

/**
 * Set the BASEPRI register which determines the base priority level required
 * for an exception to occur. All exceptions with a priority lower than the
 * programmed priority will not get triggered.
 *
 * @param priority The new base priority level. Zero will disable this masking.
 */
void intr_set_base_priority(uint8_t priority)
{
	ASSERT(priority <= LOWEST_INTR_PRIORITY);
	priority = priority << (8U - INTR_PRIORITY_BITS);

	asm volatile("msr BASEPRI, %0" : : "r" (priority) : "memory");
}

/**
 * Trigger a PendSV exception to occur.
 */
void intr_trigger_pendsv(void)
{
	SCB->ICSR |= SCB_ICSR_PENDSVSET();

	/* Ensure the PendSV actually got triggered before returning. */
	DSB();
}
