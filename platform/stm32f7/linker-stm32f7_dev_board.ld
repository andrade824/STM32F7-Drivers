/* Entry Point */
ENTRY(Reset_Handler)

/**
 * Highest address of the Main stack (the stack grows downwards). This will be
 * used by interrupts. User stacks will be allocated separately.
 */
_interrupt_stack_top = 0x20040000; /* end of RAM */

/* Specify the memory areas */
MEMORY
{
/**
 * FLASH via AXI bus. If instead address 0x00200000 was used for the FLASH then
 * the FLASH would be accessed over the ITCM (tightly coupled memory) interface.
 *
 * On STM32F72xxx and STM32F73xxx devices the ITCM operates on a small 128-bit
 * bus which restricts performance so it's generally recommended to access FLASH
 * over AXI on these devices (with the I-Cache enabled).
 */
FLASH (rx): ORIGIN = 0x08000000, LENGTH = 64K

/**
 * The three RAM regions in this chip are contiguous starting at 0x20000000:
 * 0x20000000: DTCM (64KB)
 * 0x20010000: SRAM1 (176KB)
 * 0x2003C000: SRAM2 (16KB)
 *
 * Since the FLASH is accessed over AXI, it's beneficial that most of the DATA
 * will be placed into DTCM for the first 64KB (that means code and data accesses
 * won't both be going out over the AXI bus, potentially clogging it). The kernel
 * stack will be placed into SRAM2 (it grows downward) by default though.
 */
RAM (xrw): ORIGIN = 0x20000000, LENGTH = 256k
}

INCLUDE platform/stm32f7/linker-generic.ld
