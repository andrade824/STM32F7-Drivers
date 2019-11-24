/**
 * @author Devon Andrade
 * @created 6/14/2018
 *
 * Definitions and functions used to manipulate the SDRAM Controller (located
 * within the Flexible Memory Controller peripheral).
 */
#pragma once

#ifdef INCLUDE_SDRAM_DRIVER

#include "gpio.h"

/**
 * The internal base address where the SDRAM controller starts generating
 * addresses on the external bus.
 */
#define SDRAM_BASE 0xC0000000U

void fmc_sdram_init(void);

#endif /* INCLUDE_SDRAM_DRIVER */