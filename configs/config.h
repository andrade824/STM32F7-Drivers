/**
 * @author Devon Andrade
 * @created 12/6/2018
 *
 * Include the correct configuration file based on the platform and config
 * specified in the Makefile.
 */

#ifndef CONFIG_H
#define CONFIG_H

#if defined(platform_stm32f7) && defined(config_stm32f746_disco)
	#include "stm32f7/stm32f746_disco_config.h"
#elif defined(platform_stm32f7) && defined(config_stm32f7_dev_board)
	#include "stm32f7/stm32f7_dev_board_config.h"
#else
	#error "The platform/config combination doesn't exist"
#endif

#endif /* CONFIG_H */
