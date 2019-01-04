/**
 * @author Devon Andrade
 * @created 12/26/2016
 *
 * Definitions and functions used to manipulate the Power Controller [4].
 */
#ifndef PWR_REG_H
#define PWR_REG_H

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

/**
 * Type defining the Power Controller module register map.
 */
typedef struct
{
	volatile uint32_t CR1;   /*!< PWR power control register 1,        Address offset: 0x00 */
	volatile uint32_t CSR1;  /*!< PWR power control/status register 1, Address offset: 0x04 */
	volatile uint32_t CR2;   /*!< PWR power control register 2,        Address offset: 0x08 */
	volatile uint32_t CSR2;  /*!< PWR power control/status register 2, Address offset: 0x0C */
} PowerReg;

/**
 * Define the Power Controller register map accessor.
 */
#define PWR_BASE (APB1PERIPH_BASE + 0x7000U)
#define PWR ((PowerReg *) PWR_BASE)

/**
 * Power Control Register 1 Bit Definitions [4.4.1].
 */
BIT_FIELD(PWR_CR1_LPDS,    0, 0x00000001);
BIT_FIELD(PWR_CR1_PDDS,    1, 0x00000002);
BIT_FIELD(PWR_CR1_CSBF,    3, 0x00000008);
BIT_FIELD(PWR_CR1_PVDE,    4, 0x00000010);
BIT_FIELD(PWR_CR1_PLS,     5, 0x000000E0);
BIT_FIELD(PWR_CR1_DBP,     8, 0x00000100);
BIT_FIELD(PWR_CR1_FPDS,    9, 0x00000200);
BIT_FIELD(PWR_CR1_LPUDS,  10, 0x00000400);
BIT_FIELD(PWR_CR1_MRUDS,  11, 0x00000800);
BIT_FIELD(PWR_CR1_ADCDC1, 13, 0x00002000);
BIT_FIELD(PWR_CR1_VOS,    14, 0x0000C000);
BIT_FIELD(PWR_CR1_ODEN,   16, 0x00010000);
BIT_FIELD(PWR_CR1_ODSWEN, 17, 0x00020000);
BIT_FIELD(PWR_CR1_UDEN,   18, 0x000C0000);

/**
 * Power Control/Status Register 1 Bit Definitions [4.4.2].
 */
BIT_FIELD(PWR_CSR1_WUIF,     0, 0x00000001);
BIT_FIELD(PWR_CSR1_SBF,      1, 0x00000002);
BIT_FIELD(PWR_CSR1_PVDO,     2, 0x00000004);
BIT_FIELD(PWR_CSR1_BRR,      3, 0x00000008);
BIT_FIELD(PWR_CSR1_EIWUP,    8, 0x00000100);
BIT_FIELD(PWR_CSR1_BRE,      9, 0x00000200);
BIT_FIELD(PWR_CSR1_VOSRDY,  14, 0x00004000);
BIT_FIELD(PWR_CSR1_ODRDY,   16, 0x00010000);
BIT_FIELD(PWR_CSR1_ODSWRDY, 17, 0x00020000);
BIT_FIELD(PWR_CSR1_UDRDY,   18, 0x000C0000);

#endif
