/**
 * @author Devon Andrade
 * @created 12/23/2016
 *
 * Definitions and functions used to manipulate the Embedded Flash memory [3].
 */
#ifndef FLASH_REG_H
#define FLASH_REG_H

#include "bitfield.h"
#include "platform.h"

#include <stdint.h>

/**
 * Type defining the Flash module register map.
 */
typedef struct
{
  volatile uint32_t ACR;      /*!< FLASH access control register,     Address offset: 0x00 */
  volatile uint32_t KEYR;     /*!< FLASH key register,                Address offset: 0x04 */
  volatile uint32_t OPTKEYR;  /*!< FLASH option key register,         Address offset: 0x08 */
  volatile uint32_t SR;       /*!< FLASH status register,             Address offset: 0x0C */
  volatile uint32_t CR;       /*!< FLASH control register,            Address offset: 0x10 */
  volatile uint32_t OPTCR;    /*!< FLASH option control register ,    Address offset: 0x14 */
  volatile uint32_t OPTCR1;   /*!< FLASH option control register 1 ,  Address offset: 0x18 */
} FlashReg;

/**
 * Define the Flash register map accessor.
 */
#define FLASH_REG_BASE (AHB1PERIPH_BASE + 0x3C00U)
#define FLASH ((FlashReg *) FLASH_REG_BASE)

/**
 * Access Control Register Bit Definitions [3.7.1].
 *
 * @{
 */
BIT_FIELD(FLASH_ACR_LATENCY, 0, 0x0000000F);
BIT_FIELD(FLASH_ACR_PRFTEN,  8, 0x00000100);
BIT_FIELD(FLASH_ACR_ARTEN,   9, 0x00000200);
BIT_FIELD(FLASH_ACR_ARTRST, 11, 0x00000800);
/**
 * @}
 */

#endif
