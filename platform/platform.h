/**
 * @author Devon Andrade
 * @created 12/22/2016
 *
 * Platform-specific definitions relating to the STM32F746 microcontroller
 * and the STM32F746 Discovery board.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

/**
 * Configuration of the Cortex-M7 Processor and Core Peripherals for CMSIS.
 *
 * "core_cm7.h" requires the IRQ enum located in "interrupt.h", so make sure
 * to include that beforehand.
 */
#define __CM7_REV                 0x0001U  /*!< Cortex-M7 revision r0p1                      */
#define __MPU_PRESENT             1       /*!< CM7 provides an MPU                           */
#define __NVIC_PRIO_BITS          4       /*!< CM7 uses 4 Bits for the Priority Levels       */
#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used  */
#define __FPU_PRESENT             1       /*!< FPU present                                   */
#define __ICACHE_PRESENT          1       /*!< CM7 instruction cache present                 */
#define __DCACHE_PRESENT          1       /*!< CM7 data cache present                        */
#include "interrupt.h"
#include "core_cm7.h"                     /*!< Cortex-M7 processor and core peripherals      */

/**
 * Clock settings. The user needs to ensure that init_clocks() in "system.c"
 * generates clocks with these frequencies.
 */
#define CPU_HZ 216000000
#define APB2_HZ 108000000
#define APB1_HZ 54000000
#define AHB_HZ CPU_HZ
#define FLASH_WAIT_STATES 7

/**
 * Memory Map definitions.
 */
#define RAMITCM_BASE    0x00000000U /*!< Base address of : 16KB RAM reserved for CPU execution/instruction accessible over ITCM  */
#define FLASHITCM_BASE  0x00200000U /*!< Base address of : (up to 1 MB) embedded FLASH memory  accessible over ITCM              */
#define FLASHAXI_BASE   0x08000000U /*!< Base address of : (up to 1 MB) embedded FLASH memory accessible over AXI                */
#define RAMDTCM_BASE    0x20000000U /*!< Base address of : 64KB system data RAM accessible over DTCM                             */
#define PERIPH_BASE     0x40000000U /*!< Base address of : AHB/ABP Peripherals                                                   */
#define BKPSRAM_BASE    0x40024000U /*!< Base address of : Backup SRAM(4 KB)                                                     */
#define QSPI_BASE       0x90000000U /*!< Base address of : QSPI memories  accessible over AXI                                    */
#define FMC_R_BASE      0xA0000000U /*!< Base address of : FMC Control registers                                                 */
#define QSPI_R_BASE     0xA0001000U /*!< Base address of : QSPI Control  registers                                               */
#define SRAM1_BASE      0x20010000U /*!< Base address of : 240KB RAM1 accessible over AXI/AHB                                    */
#define SRAM2_BASE      0x2004C000U /*!< Base address of : 16KB RAM2 accessible over AXI/AHB                                     */
#define FLASH_END       0x080FFFFFU /*!< FLASH end address */

#define FLASH_BASE      FLASHAXI_BASE

#define APB1PERIPH_BASE PERIPH_BASE
#define APB2PERIPH_BASE (PERIPH_BASE + 0x00010000U)
#define AHB1PERIPH_BASE (PERIPH_BASE + 0x00020000U)
#define AHB2PERIPH_BASE (PERIPH_BASE + 0x10000000U)

#endif
