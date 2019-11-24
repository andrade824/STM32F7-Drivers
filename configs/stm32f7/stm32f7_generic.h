/**
 * @author Devon Andrade
 * @created 12/6/2018
 *
 * Configuration settings common to all STM32F7 based devices.
 */
#pragma once

/***** CLOCK SETTINGS *****/

/**
 * Clock settings. The user needs to ensure that clocks_init() in "system.c"
 * generates clocks with these frequencies.
 */
#define CPU_HZ            216000000UL
#define APB2_HZ           108000000UL
#define APB1_HZ           54000000UL
#define AHB_HZ            CPU_HZ
#define SDMMC_HZ          48000000UL
#define FLASH_WAIT_STATES 7U

/**
 * PLL and bus divider settings used to get the clock values above.
 *
 * CPU_HZ = (fInput * (PLLN / PLLM)) / PLLP
 * AHB_HZ = CPU_HZ
 * APB2_HZ = AHB_HZ / APB2_DIV
 * APB1_HZ = AHB_HZ / APB1_DIV
 *
 * The 48MHz clock is used by the SDMMC and USB modules.
 * CLK_48MHz = (fInput * (PLLN / PLLM)) / PLLQ
 */
#define CLK_PLLQ 9U
#define CLK_PLLM 25U
#define CLK_PLLN 432U
#define CLK_PLLP 0U     /* Divide by 2 */

#define CLK_APB1_DIV 5U /* Divide by 4 */
#define CLK_APB2_DIV 4U /* Divide by 2 */

/**
 * Clock settings used for the LCD pixel clock.
 *
 * PLLSAI = fInput * (PLLSAIN / PLLM)
 * PLLSAIR = PLLSAI / PLLSAIR
 * Pixel Clock = PLLSAIR / PLLSAIDIVR
 *
 * Pixel Clock = ((25MHz * (192 / 25)) / 5) / 4 = 9.6MHz
 */
#define CLK_PLLSAI_PLLN 192
#define CLK_PLLSAIR     5
#define CLK_PLLSAIDIVR  1 /* Divide by 4 */

/* 48MHz (SDMMC/USB) Clock Settings */
#define ENABLE_48MHZ_CLOCK 1
#define CLK_CK48MSEL 0U /* Use the PLLQ output as the 48MHz */
#define CLK_SDMMCSEL 0U /* Use the 48MHz clock as the SDMMC clock source */

/***** RTOS SETTINGS ******/

/**
 * System timer tick granularity (in CPU_HZ cycles).
 *
 * This has to be less than 2^24 cycles (the width of the systick timer). With
 * a 216MHz CPU clock, this gives a maximum tick of 77.6ms.
 */
#define SYSTIMER_TICK (CPU_HZ / 1000U) /* 1ms tick */

/**
 * Configuration of the Cortex-M7 Processor and Core Peripherals for CMSIS.
 *
 * "core_cm7.h" requires the IRQn_Type enum located in the device-specific
 * config file, so make sure to include this file after that is defined.
 */
#define __CM7_REV              0x0001U /* Cortex-M7 revision r0p1                      */
#define __MPU_PRESENT          1       /* CM7 provides an MPU                          */
#define __NVIC_PRIO_BITS       4       /* CM7 uses 4 Bits for the Priority Levels      */
#define __Vendor_SysTickConfig 0       /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT          1       /* FPU present                                  */
#define __ICACHE_PRESENT       1       /* CM7 instruction cache present                */
#define __DCACHE_PRESENT       1       /* CM7 data cache present                       */
#include "core_cm7.h"                  /* Cortex-M7 processor and core peripherals     */

#include "stm32f7/stm32f7_mem_map.h"
