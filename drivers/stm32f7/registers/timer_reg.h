/**
 * @author Devon Andrade
 * @created 3/28/2021
 *
 * Timer Register map. Different timers have differing sets of features. This
 * register map contains every possible register a specific timer might have,
 * but each individual timer will most likely only contain a subset of these
 * registers.
 */
#pragma once

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

/* Type defining the Timer module register map. */
typedef struct
{
	volatile uint32_t CR1;   /* TIM control register 1,              Address offset: 0x00 */
	volatile uint32_t CR2;   /* TIM control register 2,              Address offset: 0x04 */
	volatile uint32_t SMCR;  /* TIM slave mode control register,     Address offset: 0x08 */
	volatile uint32_t DIER;  /* TIM DMA/interrupt enable register,   Address offset: 0x0C */
	volatile uint32_t SR;    /* TIM status register,                 Address offset: 0x10 */
	volatile uint32_t EGR;   /* TIM event generation register,       Address offset: 0x14 */
	volatile uint32_t CCMR1; /* TIM capture/compare mode register 1, Address offset: 0x18 */
	volatile uint32_t CCMR2; /* TIM capture/compare mode register 2, Address offset: 0x1C */
	volatile uint32_t CCER;  /* TIM capture/compare enable register, Address offset: 0x20 */
	volatile uint32_t CNT;   /* TIM counter register,                Address offset: 0x24 */
	volatile uint32_t PSC;   /* TIM prescaler,                       Address offset: 0x28 */
	volatile uint32_t ARR;   /* TIM auto-reload register,            Address offset: 0x2C */
	volatile uint32_t RCR;   /* TIM repetition counter register,     Address offset: 0x30 */
	volatile uint32_t CCR1;  /* TIM capture/compare register 1,      Address offset: 0x34 */
	volatile uint32_t CCR2;  /* TIM capture/compare register 2,      Address offset: 0x38 */
	volatile uint32_t CCR3;  /* TIM capture/compare register 3,      Address offset: 0x3C */
	volatile uint32_t CCR4;  /* TIM capture/compare register 4,      Address offset: 0x40 */
	volatile uint32_t BDTR;  /* TIM break and dead-time register,    Address offset: 0x44 */
	volatile uint32_t DCR;   /* TIM DMA control register,            Address offset: 0x48 */
	volatile uint32_t DMAR;  /* TIM DMA address for full transfer,   Address offset: 0x4C */
	volatile uint32_t OR;    /* TIM option register,                 Address offset: 0x50 */
	volatile uint32_t CCMR3; /* TIM capture/compare mode register 3, Address offset: 0x54 */
	volatile uint32_t CCR5;  /* TIM capture/compare mode register 5, Address offset: 0x58 */
	volatile uint32_t CCR6;  /* TIM capture/compare mode register 6, Address offset: 0x5C */
} TimerReg;

/* Define the Timer register map accessors. */
#define TIM1_BASE  (APB2PERIPH_BASE + 0x0000U)
#define TIM2_BASE  (APB1PERIPH_BASE + 0x0000U)
#define TIM3_BASE  (APB1PERIPH_BASE + 0x0400U)
#define TIM4_BASE  (APB1PERIPH_BASE + 0x0800U)
#define TIM5_BASE  (APB1PERIPH_BASE + 0x0C00U)
#define TIM6_BASE  (APB1PERIPH_BASE + 0x1000U)
#define TIM7_BASE  (APB1PERIPH_BASE + 0x1400U)
#define TIM8_BASE  (APB2PERIPH_BASE + 0x0400U)
#define TIM9_BASE  (APB2PERIPH_BASE + 0x4000U)
#define TIM10_BASE (APB2PERIPH_BASE + 0x4400U)
#define TIM11_BASE (APB2PERIPH_BASE + 0x4800U)
#define TIM12_BASE (APB1PERIPH_BASE + 0x1800U)
#define TIM13_BASE (APB1PERIPH_BASE + 0x1C00U)
#define TIM14_BASE (APB1PERIPH_BASE + 0x2000U)

#define TIM1  ((TimerReg *) TIM1_BASE)
#define TIM2  ((TimerReg *) TIM2_BASE)
#define TIM3  ((TimerReg *) TIM3_BASE)
#define TIM4  ((TimerReg *) TIM4_BASE)
#define TIM5  ((TimerReg *) TIM5_BASE)
#define TIM6  ((TimerReg *) TIM6_BASE)
#define TIM7  ((TimerReg *) TIM7_BASE)
#define TIM8  ((TimerReg *) TIM8_BASE)
#define TIM9  ((TimerReg *) TIM9_BASE)
#define TIM10 ((TimerReg *) TIM10_BASE)
#define TIM11 ((TimerReg *) TIM11_BASE)
#define TIM12 ((TimerReg *) TIM12_BASE)
#define TIM13 ((TimerReg *) TIM13_BASE)
#define TIM14 ((TimerReg *) TIM14_BASE)

// /* Control Register 1. */
// BIT_FIELD2(SPI_CR1_CPHA,      0, 0);
// BIT_FIELD2(SPI_CR1_CPOL,      1, 1);
// BIT_FIELD2(SPI_CR1_MSTR,      2, 2);
// BIT_FIELD2(SPI_CR1_BR,        3, 5);
// BIT_FIELD2(SPI_CR1_SPE,       6, 6);
// BIT_FIELD2(SPI_CR1_LSBFIRST,  7, 7);
// BIT_FIELD2(SPI_CR1_SSI,       8, 8);
// BIT_FIELD2(SPI_CR1_SSM,       9, 9);
// BIT_FIELD2(SPI_CR1_RXONLY,   10, 10);
// BIT_FIELD2(SPI_CR1_CRCL,     11, 11);
// BIT_FIELD2(SPI_CR1_CRCNEXT,  12, 12);
// BIT_FIELD2(SPI_CR1_CRCEN,    13, 13);
// BIT_FIELD2(SPI_CR1_BIDIOE,   14, 14);
// BIT_FIELD2(SPI_CR1_BIDIMODE, 15, 15);

// /* SPI_CR1_CPHA (Clock Phase) field options. */
// typedef enum {
// 	SPI_CPHA_0 = 0,
// 	SPI_CPHA_1 = 1
// } SpiClockPhase;
