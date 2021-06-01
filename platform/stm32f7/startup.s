/**
 * This startup script handles the following:
 *   - Setting up the initial stack pointer.
 *   - Setting up the reset vector and default exception handlers.
 *   - Copying DATA into RAM and clearing out BSS.
 *   - Calling C static constructors.
 *   - Branch to main().
 *
 * After reset the Cortex-M7 processor is in Thread mode, priority is privileged,
 * and the stack is set to Main.
 *
 * This code was modified from a startup script supplied by STMicroelectronics.
 * Here's the original copyright license:
 *
 * COPYRIGHT 2016 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

.syntax unified
.cpu cortex-m7
.fpu softvfp
.thumb

/**
 * Start address for the initialization values of the .data section.
 * defined in linker script.
 */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss
/* Top of the main/kernel stack defined in the linker script. */
.word _kern_stack_top

/**
 * Reset vector. This will perform all setup necessary to get the system into a
 * state where C code can operate (setting up the DATA/BSS sections, setting up
 * the Main/kernel stack, etc).
 */
.section .text.Reset_Handler
	.weak Reset_Handler
	.type Reset_Handler, %function
Reset_Handler:
	/* Set stack pointer */
	ldr		sp, =_kern_stack_top

	/* Copy the data segment initializers from flash to SRAM */
	movs	r1, #0
	b		LoopCopyDataInit

CopyDataInit:
	ldr		r3, =_sidata
	ldr		r3, [r3, r1]
	str		r3, [r0, r1]
	adds	r1, r1, #4

LoopCopyDataInit:
	ldr		r0, =_sdata
	ldr		r3, =_edata
	adds	r2, r0, r1
	cmp		r2, r3
	bcc		CopyDataInit
	ldr		r2, =_sbss
	b		LoopFillZerobss

/* Zero fill the bss segment. */
FillZerobss:
	movs	r3, #0
	str		r3, [r2], #4

LoopFillZerobss:
	ldr		r3, = _ebss
	cmp		r2, r3
	bcc		FillZerobss

	/* Call static constructors (mostly used by the standard library). */
	bl		__libc_init_array

	/* Call the application's entry point. */
	bl		main

	/* main() should never exit, if it does just infinite loop. */
	b		.
.size Reset_Handler, .-Reset_Handler

/**
 * The default exception handler during initialization is just an infinite loop.
 * This is done under the expectation that interrupt_init() will get called
 * shortly after boot and before any code causes exceptions to setup the real
 * default exception handlers.
*/
.section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
	b		Infinite_Loop
.size Default_Handler, .-Default_Handler

/**
 * Initial vector table (infinite loops). The proper linker constructs must be
 * in place to put this vector table at address 0x00000000. After bootstrap, it
 * is expected that a new vector table will be created in RAM and the Vector
 * Table Offset Register (VTOR) properly updated to point to the new vector table.
 */
.section .isr_vector,"a",%progbits
	.type init_vector_table, %object

init_vector_table:
	.word _kern_stack_top
	.word Reset_Handler

	.word Default_Handler
	.word Default_Handler
	.word Default_Handler
	.word Default_Handler
	.word Default_Handler
	.word 0
	.word 0
	.word 0
	.word 0
	.word Default_Handler
	.word Default_Handler
	.word 0
	.word Default_Handler
	.word Default_Handler
.size init_vector_table, .-init_vector_table
