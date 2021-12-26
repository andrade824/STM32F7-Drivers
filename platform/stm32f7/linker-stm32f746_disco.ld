/* Entry Point */
ENTRY(Reset_Handler)

/**
 * Highest address of the Main stack (the stack grows downwards). This will be
 * used by interrupts. User stacks will be allocated separately.
 */
_interrupt_stack_top = 0x20050000; /* end of RAM */

/* Specify the memory areas */
MEMORY
{
FLASH (rx)     : ORIGIN = 0x08000000, LENGTH = 1024K
RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 320K
}

INCLUDE platform/stm32f7/linker-generic.ld
