/**
 * @author Devon Andrade
 * @created 12/6/2018
 *
 * Configuration settings common to all STM32F7 based devices.
 */
#ifndef STM32F7_GENERIC_H
#define STM32F7_GENERIC_H

/**
 * Configuration of the Cortex-M7 Processor and Core Peripherals for CMSIS.
 *
 * "core_cm7.h" requires the IRQn_Type enum located in the device-specific
 * config file, so make sure to include this file after that is defined.
 */
#define __CM7_REV                 0x0001U  /*!< Cortex-M7 revision r0p1                      */
#define __MPU_PRESENT             1        /*!< CM7 provides an MPU                          */
#define __NVIC_PRIO_BITS          4        /*!< CM7 uses 4 Bits for the Priority Levels      */
#define __Vendor_SysTickConfig    0        /*!< Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             1        /*!< FPU present                                  */
#define __ICACHE_PRESENT          1        /*!< CM7 instruction cache present                */
#define __DCACHE_PRESENT          1        /*!< CM7 data cache present                       */
#include "core_cm7.h"                      /*!< Cortex-M7 processor and core peripherals     */

#include "stm32f7/stm32f7_mem_map.h"

#endif /* STM32F7_GENERIC_H */
