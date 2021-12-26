/**
 * @author Devon Andrade
 * @created 12/18/2021
 *
 * Definitions and functions used to manipulate the Nested Vectored Interrupt
 * Controller.
 */
#pragma once

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

/* Type defining the NVIC module register map. */
typedef struct
{
	volatile uint32_t ISER[8U]; /*!< Offset: 0x000 (R/W) Interrupt Set Enable Register */
	uint32_t RESERVED0[24U];
	volatile uint32_t ICER[8U]; /*!< Offset: 0x080 (R/W) Interrupt Clear Enable Register */
	uint32_t RSERVED1[24U];
	volatile uint32_t ISPR[8U]; /*!< Offset: 0x100 (R/W) Interrupt Set Pending Register */
	uint32_t RESERVED2[24U];
	volatile uint32_t ICPR[8U]; /*!< Offset: 0x180 (R/W) Interrupt Clear Pending Register */
	uint32_t RESERVED3[24U];
	volatile uint32_t IABR[8U]; /*!< Offset: 0x200 (R/W) Interrupt Active bit Register */
	uint32_t RESERVED4[56U];
	volatile uint8_t IP[240U]; /*!< Offset: 0x300 (R/W) Interrupt Priority Register (8Bit wide) */
	uint32_t RESERVED5[644U];
	volatile uint32_t STIR;     /*!< Offset: 0xE00 ( /W) Software Trigger Interrupt Register */
} NVICReg;

/* Define the NVIC register map accessor. */
#define NVIC ((NVICReg *) NVIC_BASE)

/**
 * Helper macros for selecting the correct register and bit within that register
 * for the registers that contain 1-bit per interrupt (ISER, ICER, etc.).
 */
#define NVIC_REG_SELECT(irq) (irq / 32U)
#define NVIC_BIT_SELECT(irq) (irq & 0x1F)

/**
 * Interrupt Set-Enable Register.
 */
BIT_FIELD2(NVIC_ISER_SETENA, 0, 31);

/**
 * Interrupt Clear-Enable Register.
 */
BIT_FIELD2(NVIC_ICER_CLRENA, 0, 31);

/**
 * Interrupt Set Pending Register.
 */
BIT_FIELD2(NVIC_ISPR_SETPEND, 0, 31);

/**
 * Interrupt Clear Pending Register.
 */
BIT_FIELD2(NVIC_ICPR_CLRPEND, 0, 31);

/**
 * Interrupt Active Bit Register.
 */
BIT_FIELD2(NVIC_IABR_ACTIVE, 0, 31);

/**
 * Interrupt Priority Register.
 */
BIT_FIELD2(NVIC_IP_PRIORITY, 0, 7);

/**
 * Software Trigger Interrupt Register.
 */
BIT_FIELD2(NVIC_STIR_INTID, 0, 8);
