/**
 * @author Devon Andrade
 * @created 12/6/2018
 *
 * Include the correct memory map file based on the platform specified in the
 * Makefile.
 *
 * config.h also includes this file. Including it separately is only useful
 * to avoid header file circular dependencies (like in a few of the "register"
 * files).
 */
#pragma once

#if defined(platform_stm32f7)
	#include "stm32f7/stm32f7_mem_map.h"
#else
	#error "The platform doesn't exist"
#endif
