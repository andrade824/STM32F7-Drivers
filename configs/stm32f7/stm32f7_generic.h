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

/* STM32F7 uses 4-bits for the interrupt priority level. */
#define INTR_PRIORITY_BITS 4U

#include "stm32f7/stm32f7_mem_map.h"

/* Utilize the default OS configuration. */
#include "default_os_config.h"
