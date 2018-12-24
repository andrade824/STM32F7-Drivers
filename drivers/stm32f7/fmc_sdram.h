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
#define SDRAM_BASE 0xC0000000U

status_t init_fmc_sdram(void);

#endif
#endif