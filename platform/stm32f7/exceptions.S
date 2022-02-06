/**
 * @author Devon Andrade
 * @created 12/25/21 (Merry Christmas! Definitely not coding to avoid my family ._.)
 *
 * First level exception handling logic for the internal exceptions (HardFault,
 * UsageFault, BusFault, etc). Setting the exception handlers to assembly code
 * allows us to store the register state before the compiler can clobber
 * anything. The rest of the exception handling logic will occur in C.
 */

.syntax unified
.cpu cortex-m7
.fpu fpv5-sp-d16
.thumb

.text

/**
 * Push all registers onto the stack (should be the Main stack when operating
 * inside of an exception handler). This register context will then be passed
 * into the second level exception handler.
 *
 * @param exc_handler Address of the second level exception handler to jump to
 *                    after saving registers onto the stack.
 */
.macro PUSH_REGISTERS exc_handler
	/**
	 * These dwarf directives are needed for the debugger to properly see this
	 * code as a signal handler.
	 */
	.cfi_sections .debug_frame
	.cfi_startproc
	.cfi_signal_frame

	/* Push every register besides LR/PC/xPSR onto the stack. */
	push.w	{r0-r12}

	/**
	 * LR/PC/xPSR are stored on whatever stack was being used when the exception
	 * occurred. Bit 2 of the current value of LR tells us which stack was being
	 * used:
	 *
	 * 0 == Main Stack
	 * 1 == Process Stack
	 *
	 * If the main stack was previously selected, then the offset from the Main
	 * SP to get LR/PC/xPSR will have the typical offset of 0x14 (this is the
	 * offset from the values that the exception entry automatically push) plus
	 * an offset of 0x34 to account for the 13 registers that were just pushed
	 * onto the stack.
	 */

	/* Compute the offset from MSP/PSP to get LR/PC/xPSR and store it in r0. */
	#define LR_PC_XPSR_OFFSET 0x14U
	mov		r0, LR_PC_XPSR_OFFSET
	mrs		r1, PSP
	tst		lr, 0x4
	bne		1f

	/* LR/PC/xPSR were pushed onto the main stack, add offset for r0-r12. */
	#define R0_THROUGH_R12_SIZE 0x34U
	add		r0, r0, R0_THROUGH_R12_SIZE
	mrs		r1, MSP

1:
	/* r1 == stack that contains LR/PC/xPSR, r0 == offset into that stack */
	add		r0, r0, r1
	ldm		r0, {r2-r4}

	/* At this point: r2 == LR, r3 == PC (Return Address), r4 == xPSR */
	push	{r2-r4}

	/**
	 * Tell the debugger to treat the PC value that was pushed onto the stack
	 * during exception entry as the current LR value. This is needed for the
	 * debugger to walk the call stack from the exception handler back to the
	 * code that caused the exception.
	 *
	 * At this point, 16 registers have been pushed onto the stack so the stack
	 * pointer has decremented by 64 bytes.
	 *
	 * There doesn't seem to be a way to embed in the DWARF info that it's
	 * possible for the stack to have changed (from PSP to MSP), which means if
	 * the user wants GDB to backtrace from an exception all the way back to the
	 * task that caused the exception, then -fno-omit-frame-pointer is required
	 * to ensure that GDB can use the frame pointer to link the stack frames
	 * across the two stacks.
	 */
	.cfi_def_cfa_offset 64
	.cfi_offset 14, -60

	/**
	 * Move the beginning of the register context into r0 as the parameter to
	 * the exception handler.
	 */
	mov		r0, sp

	/**
	 * Call the wanted second level exception handler. Overwriting the LR by
	 * using "bl" instead of "b" means we can never properly return from this
	 * exception handler. The system is dying so this is fine.
	 */
	bl		\exc_handler

	/* Ensure that if the handler does return for some reason, we just hang. */
	b		.

	.cfi_endproc

.endm

.global hard_fault_asm
.type hard_fault_asm, %function
hard_fault_asm:
	PUSH_REGISTERS hard_fault_exc

.global mem_manage_asm
.type mem_manage_asm, %function
mem_manage_asm:
	PUSH_REGISTERS mem_manage_exc

.global bus_fault_asm
.type bus_fault_asm, %function
bus_fault_asm:
	PUSH_REGISTERS bus_fault_exc

.global usage_fault_asm
.type usage_fault_asm, %function
usage_fault_asm:
	PUSH_REGISTERS usage_fault_exc

.global no_exc_handler_asm
.type no_exc_handler_asm, %function
no_exc_handler_asm:
	PUSH_REGISTERS no_exc_handler
