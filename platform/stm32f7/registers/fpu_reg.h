/**
 * @author Devon Andrade
 * @created 12/31/2021
 *
 * Definitions and functions used to manipulate the Floating Point Unit (FPU).
 */
#pragma once

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

/**
 * Type defining the System Control Block module register map.
 */
typedef struct
{
	uint32_t RESERVED0;
	volatile uint32_t FPCCR;  /*!< Offset: 0x004 (R/W) Floating-Point Context Control Register */
	volatile uint32_t FPCAR;  /*!< Offset: 0x008 (R/W) Floating-Point Context Address Register */
	volatile uint32_t FPDSCR; /*!< Offset: 0x00C (R/W) Floating-Point Default Status Control Register */
	volatile uint32_t MVFR0;  /*!< Offset: 0x010 (R/ ) Media and FP Feature Register 0 */
	volatile uint32_t MVFR1;  /*!< Offset: 0x014 (R/ ) Media and FP Feature Register 1 */
	volatile uint32_t MVFR2;  /*!< Offset: 0x018 (R/ ) Media and FP Feature Register 2 */
} FpuReg;

/**
 * Define the FPU register map accessor.
 */
#define FPU ((FpuReg *) FPU_BASE)

/* Floating-Point Context Control Register. */
BIT_FIELD2(FPU_FPCCR_LSPACT,  0, 0);
BIT_FIELD2(FPU_FPCCR_USER,    1, 1);
BIT_FIELD2(FPU_FPCCR_THREAD,  3, 3);
BIT_FIELD2(FPU_FPCCR_HFRDY,   4, 4);
BIT_FIELD2(FPU_FPCCR_MMRDY,   5, 5);
BIT_FIELD2(FPU_FPCCR_BFRDY,   6, 6);
BIT_FIELD2(FPU_FPCCR_MONRDY,  8, 8);
BIT_FIELD2(FPU_FPCCR_LSPEN,  30, 30);
BIT_FIELD2(FPU_FPCCR_ASPEN,  31, 31);
