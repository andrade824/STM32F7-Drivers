/**
 * @author Devon Andrade
 * @created 6/14/2018
 *
 * Definitions and functions used to manipulate the SDRAM Controller (located
 * within the Flexible Memory Controller peripheral) [13.7.5].
 */
#ifdef INCLUDE_SDRAM_DRIVER
#ifndef FMC_SDRAM_H
#define FMC_SDRAM_H

#include "gpio.h"
#include "status.h"

/**
 * The internal base address where the SDRAM controller starts generating
 * addresses on the external bus.
 */
#define SDRAM_BASE ((uint32_t)0xC0000000)

/**
 * The GPIO alternate function for the FMC SDRAM pins. Check section 3 of the
 * datasheet for more details.
 */
#define SDRAM_ALT_FUNC AF12

status_t init_fmc_sdram(void);

#endif
#endif