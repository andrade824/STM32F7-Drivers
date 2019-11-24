/**
 * @author Devon Andrade
 * @created 12/22/2016
 *
 * Definitions and functions used to manipulate the Reset and Clock Control
 * module.
 */
#pragma once

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

/**
 * Type defining the RCC module register map.
 */
typedef struct {
	volatile uint32_t CR;           /* RCC clock control register,                                  Address offset: 0x00 */
	volatile uint32_t PLLCFGR;      /* RCC PLL configuration register,                              Address offset: 0x04 */
	volatile uint32_t CFGR;         /* RCC clock configuration register,                            Address offset: 0x08 */
	volatile uint32_t CIR;          /* RCC clock interrupt register,                                Address offset: 0x0C */
	volatile uint32_t AHB1RSTR;     /* RCC AHB1 peripheral reset register,                          Address offset: 0x10 */
	volatile uint32_t AHB2RSTR;     /* RCC AHB2 peripheral reset register,                          Address offset: 0x14 */
	volatile uint32_t AHB3RSTR;     /* RCC AHB3 peripheral reset register,                          Address offset: 0x18 */
	uint32_t          RESERVED0;    /* Reserved, 0x1C                                                                    */
	volatile uint32_t APB1RSTR;     /* RCC APB1 peripheral reset register,                          Address offset: 0x20 */
	volatile uint32_t APB2RSTR;     /* RCC APB2 peripheral reset register,                          Address offset: 0x24 */
	uint32_t          RESERVED1[2]; /* Reserved, 0x28-0x2C                                                               */
	volatile uint32_t AHB1ENR;      /* RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
	volatile uint32_t AHB2ENR;      /* RCC AHB2 peripheral clock register,                          Address offset: 0x34 */
	volatile uint32_t AHB3ENR;      /* RCC AHB3 peripheral clock register,                          Address offset: 0x38 */
	uint32_t          RESERVED2;    /* Reserved, 0x3C                                                                    */
	volatile uint32_t APB1ENR;      /* RCC APB1 peripheral clock enable register,                   Address offset: 0x40 */
	volatile uint32_t APB2ENR;      /* RCC APB2 peripheral clock enable register,                   Address offset: 0x44 */
	uint32_t          RESERVED3[2]; /* Reserved, 0x48-0x4C                                                               */
	volatile uint32_t AHB1LPENR;    /* RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50 */
	volatile uint32_t AHB2LPENR;    /* RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54 */
	volatile uint32_t AHB3LPENR;    /* RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58 */
	uint32_t          RESERVED4;    /* Reserved, 0x5C                                                                    */
	volatile uint32_t APB1LPENR;    /* RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60 */
	volatile uint32_t APB2LPENR;    /* RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64 */
	uint32_t          RESERVED5[2]; /* Reserved, 0x68-0x6C                                                               */
	volatile uint32_t BDCR;         /* RCC Backup domain control register,                          Address offset: 0x70 */
	volatile uint32_t CSR;          /* RCC clock control & status register,                         Address offset: 0x74 */
	uint32_t          RESERVED6[2]; /* Reserved, 0x78-0x7C                                                               */
	volatile uint32_t SSCGR;        /* RCC spread spectrum clock generation register,               Address offset: 0x80 */
	volatile uint32_t PLLI2SCFGR;   /* RCC PLLI2S configuration register,                           Address offset: 0x84 */
	volatile uint32_t PLLSAICFGR;   /* RCC PLLSAI configuration register,                           Address offset: 0x88 */
	volatile uint32_t DCKCFGR1;     /* RCC Dedicated Clocks configuration register1,                Address offset: 0x8C */
	volatile uint32_t DCKCFGR2;     /* RCC Dedicated Clocks configuration register 2,               Address offset: 0x90 */
} RccReg;

/* Define the RCC register map accessor. */
#define RCC_BASE (AHB1PERIPH_BASE + 0x3800U)
#define RCC ((RccReg *) RCC_BASE)

/* Control Register Bit Definitions. */
BIT_FIELD(RCC_CR_HSION,      0, 0x00000001);
BIT_FIELD(RCC_CR_HSIRDY,     1, 0x00000002);
BIT_FIELD(RCC_CR_HSITRIM,    3, 0x000000F8);
BIT_FIELD(RCC_CR_HSICAL,     8, 0x0000FF00);
BIT_FIELD(RCC_CR_HSEON,     16, 0x00010000);
BIT_FIELD(RCC_CR_HSERDY,    17, 0x00020000);
BIT_FIELD(RCC_CR_HSEBYP,    18, 0x00040000);
BIT_FIELD(RCC_CR_CSSON,     19, 0x00080000);
BIT_FIELD(RCC_CR_PLLON,     24, 0x01000000);
BIT_FIELD(RCC_CR_PLLRDY,    25, 0x02000000);
BIT_FIELD(RCC_CR_PLLI2SON,  26, 0x04000000);
BIT_FIELD(RCC_CR_PLLI2SRDY, 27, 0x08000000);
BIT_FIELD(RCC_CR_PLLSAION,  28, 0x10000000);
BIT_FIELD(RCC_CR_PLLSAIRDY, 29, 0x20000000);

/* PLL Configuration Register Bit Definitions. */
BIT_FIELD(RCC_PLLCFGR_PLLM,    0, 0x0000003F);
BIT_FIELD(RCC_PLLCFGR_PLLN,    6, 0x00007FC0);
BIT_FIELD(RCC_PLLCFGR_PLLP,   16, 0x00030000);
BIT_FIELD(RCC_PLLCFGR_PLLSRC, 22, 0x00400000);
BIT_FIELD(RCC_PLLCFGR_PLLQ,   24, 0x0F000000);

/* Clock Configuration Register Bit Definitions. */
BIT_FIELD(RCC_CFGR_SW,       0, 0x00000003);
BIT_FIELD(RCC_CFGR_SWS,      2, 0x0000000C);
BIT_FIELD(RCC_CFGR_HPRE,     4, 0x000000F0);
BIT_FIELD(RCC_CFGR_PPRE1,   10, 0x00001C00);
BIT_FIELD(RCC_CFGR_PPRE2,   13, 0x00007000);
BIT_FIELD(RCC_CFGR_RTCPRE,  16, 0x001F0000);
BIT_FIELD(RCC_CFGR_MCO1,    21, 0x00600000);
BIT_FIELD(RCC_CFGR_I2SSRC,  23, 0x00800000);
BIT_FIELD(RCC_CFGR_MCO1PRE, 24, 0x07000000);
BIT_FIELD(RCC_CFGR_MCO2PRE, 27, 0x38000000);
BIT_FIELD(RCC_CFGR_MCO2,    30, 0xC0000000);

/* RCC AHB1 Peripheral Clock Register Bit Definitions. */
BIT_FIELD(RCC_AHB1ENR_GPIOAEN,      0, 0x00000001);
BIT_FIELD(RCC_AHB1ENR_GPIOBEN,      1, 0x00000002);
BIT_FIELD(RCC_AHB1ENR_GPIOCEN,      2, 0x00000004);
BIT_FIELD(RCC_AHB1ENR_GPIODEN,      3, 0x00000008);
BIT_FIELD(RCC_AHB1ENR_GPIOEEN,      4, 0x00000010);
BIT_FIELD(RCC_AHB1ENR_GPIOFEN,      5, 0x00000020);
BIT_FIELD(RCC_AHB1ENR_GPIOGEN,      6, 0x00000040);
BIT_FIELD(RCC_AHB1ENR_GPIOHEN,      7, 0x00000080);
BIT_FIELD(RCC_AHB1ENR_GPIOIEN,      8, 0x00000100);
BIT_FIELD(RCC_AHB1ENR_GPIOJEN,      9, 0x00000200);
BIT_FIELD(RCC_AHB1ENR_GPIOKEN,     10, 0x00000400);
BIT_FIELD(RCC_AHB1ENR_CRCEN,       12, 0x00001000);
BIT_FIELD(RCC_AHB1ENR_BKPSRAMEN,   18, 0x00040000);
BIT_FIELD(RCC_AHB1ENR_DTCMRAMEN,   20, 0x00100000);
BIT_FIELD(RCC_AHB1ENR_DMA1EN,      21, 0x00200000);
BIT_FIELD(RCC_AHB1ENR_DMA2EN,      22, 0x00400000);
BIT_FIELD(RCC_AHB1ENR_DMA2DEN,     23, 0x00800000);
BIT_FIELD(RCC_AHB1ENR_ETHMACEN,    25, 0x02000000);
BIT_FIELD(RCC_AHB1ENR_ETHMACTXEN,  26, 0x04000000);
BIT_FIELD(RCC_AHB1ENR_ETHMACRXEN,  27, 0x08000000);
BIT_FIELD(RCC_AHB1ENR_ETHMACPTPEN, 28, 0x10000000);
BIT_FIELD(RCC_AHB1ENR_OTGHSEN,     29, 0x20000000);
BIT_FIELD(RCC_AHB1ENR_OTGHSULPIEN, 30, 0x40000000);

/* RCC AHB3 Peripheral Clock Register Bit Definitions. */
BIT_FIELD(RCC_AHB3ENR_FMCEN,   0, 0x00000001);
BIT_FIELD(RCC_AHB3ENR_QSPIBEN, 1, 0x00000002);

/* RCC APB1 Peripheral Clock Register Bit Definitions. */
BIT_FIELD(RCC_APB1ENR_TIM2EN,     0, 0x00000001);
BIT_FIELD(RCC_APB1ENR_TIM3EN,     1, 0x00000002);
BIT_FIELD(RCC_APB1ENR_TIM4EN,     2, 0x00000004);
BIT_FIELD(RCC_APB1ENR_TIM5EN,     3, 0x00000008);
BIT_FIELD(RCC_APB1ENR_TIM6EN,     4, 0x00000010);
BIT_FIELD(RCC_APB1ENR_TIM7EN,     5, 0x00000020);
BIT_FIELD(RCC_APB1ENR_TIM12EN,    6, 0x00000040);
BIT_FIELD(RCC_APB1ENR_TIM13EN,    7, 0x00000080);
BIT_FIELD(RCC_APB1ENR_TIM14EN,    8, 0x00000100);
BIT_FIELD(RCC_APB1ENR_LPTIM1EN,   9, 0x00000200);
BIT_FIELD(RCC_APB1ENR_WWDGEN,    11, 0x00000800);
BIT_FIELD(RCC_APB1ENR_SPI2EN,    14, 0x00004000);
BIT_FIELD(RCC_APB1ENR_SPI3EN,    15, 0x00008000);
BIT_FIELD(RCC_APB1ENR_SPDIFRXEN, 16, 0x00010000);
BIT_FIELD(RCC_APB1ENR_USART2EN,  17, 0x00020000);
BIT_FIELD(RCC_APB1ENR_USART3EN,  18, 0x00040000);
BIT_FIELD(RCC_APB1ENR_UART4EN,   19, 0x00080000);
BIT_FIELD(RCC_APB1ENR_UART5EN,   20, 0x00100000);
BIT_FIELD(RCC_APB1ENR_I2C1EN,    21, 0x00200000);
BIT_FIELD(RCC_APB1ENR_I2C2EN,    22, 0x00400000);
BIT_FIELD(RCC_APB1ENR_I2C3EN,    23, 0x00800000);
BIT_FIELD(RCC_APB1ENR_I2C4EN,    24, 0x01000000);
BIT_FIELD(RCC_APB1ENR_CAN1EN,    25, 0x02000000);
BIT_FIELD(RCC_APB1ENR_CAN2EN,    26, 0x04000000);
BIT_FIELD(RCC_APB1ENR_CECEN,     27, 0x08000000);
BIT_FIELD(RCC_APB1ENR_PWREN,     28, 0x10000000);
BIT_FIELD(RCC_APB1ENR_DACEN,     29, 0x20000000);
BIT_FIELD(RCC_APB1ENR_UART7EN,   30, 0x40000000);
BIT_FIELD(RCC_APB1ENR_UART8EN,   31, 0x80000000);

/* RCC APB2 Peripheral Clock Register Bit Definitions. */
BIT_FIELD(RCC_APB2ENR_TIM1EN,    0, 0x00000001);
BIT_FIELD(RCC_APB2ENR_TIM8EN,    1, 0x00000002);
BIT_FIELD(RCC_APB2ENR_USART1EN,  4, 0x00000010);
BIT_FIELD(RCC_APB2ENR_USART6EN,  5, 0x00000020);
BIT_FIELD(RCC_APB2ENR_SDMMC2EN,  7, 0x00000080);
BIT_FIELD(RCC_APB2ENR_ADC1EN,    8, 0x00000100);
BIT_FIELD(RCC_APB2ENR_ADC2EN,    9, 0x00000200);
BIT_FIELD(RCC_APB2ENR_ADC3EN,   10, 0x00000400);
BIT_FIELD(RCC_APB2ENR_SDMMC1EN, 11, 0x00000800);
BIT_FIELD(RCC_APB2ENR_SPI1EN,   12, 0x00001000);
BIT_FIELD(RCC_APB2ENR_SPI4EN,   13, 0x00002000);
BIT_FIELD(RCC_APB2ENR_SYSCFGEN, 14, 0x00004000);
BIT_FIELD(RCC_APB2ENR_TIM9EN,   16, 0x00010000);
BIT_FIELD(RCC_APB2ENR_TIM10EN,  17, 0x00020000);
BIT_FIELD(RCC_APB2ENR_TIM11EN,  18, 0x00040000);
BIT_FIELD(RCC_APB2ENR_SPI5EN,   20, 0x00100000);
BIT_FIELD(RCC_APB2ENR_SPI6EN,   21, 0x00200000);
BIT_FIELD(RCC_APB2ENR_SAI1EN,   22, 0x00400000);
BIT_FIELD(RCC_APB2ENR_SAI2EN,   23, 0x00800000);
BIT_FIELD(RCC_APB2ENR_LTDCEN,   26, 0x04000000);

/* PLLSAI Configuration Register Bit Definitions. */
BIT_FIELD(RCC_PLLSAICFGR_PLLN,     6, 0x00007FC0);
BIT_FIELD(RCC_PLLSAICFGR_PLLSAIP, 16, 0x00030000);
BIT_FIELD(RCC_PLLSAICFGR_PLLSAIQ, 24, 0x0F000000);
BIT_FIELD(RCC_PLLSAICFGR_PLLSAIR, 28, 0x70000000);

/* Dedicated Clocks Configuration Register 1 Bit Definitions. */
BIT_FIELD(RCC_DCKCFGR1_PLLI2SDIV,   0, 0x0000001F);
BIT_FIELD(RCC_DCKCFGR1_PLLSAIDIVQ,  8, 0x00001F00);
BIT_FIELD(RCC_DCKCFGR1_PLLSAIDIVR, 16, 0x00030000);
BIT_FIELD(RCC_DCKCFGR1_SAI1SEL,    20, 0x00300000);
BIT_FIELD(RCC_DCKCFGR1_SAI2SEL,    22, 0x00C00000);
BIT_FIELD(RCC_DCKCFGR1_TIMPRE,     24, 0x01000000);

/* Dedicated Clocks Configuration Register 2 Bit Definitions. */
BIT_FIELD(RCC_DCKCFGR2_USART1SEL,  0, 0x00000003);
BIT_FIELD(RCC_DCKCFGR2_USART2SEL,  2, 0x0000000C);
BIT_FIELD(RCC_DCKCFGR2_USART3SEL,  4, 0x00000030);
BIT_FIELD(RCC_DCKCFGR2_USART4SEL,  6, 0x000000C0);
BIT_FIELD(RCC_DCKCFGR2_USART5SEL,  8, 0x00000300);
BIT_FIELD(RCC_DCKCFGR2_USART6SEL, 10, 0x00000C00);
BIT_FIELD(RCC_DCKCFGR2_USART7SEL, 12, 0x00003000);
BIT_FIELD(RCC_DCKCFGR2_USART8SEL, 14, 0x0000C000);
BIT_FIELD(RCC_DCKCFGR2_I2C1SEL,   16, 0x00030000);
BIT_FIELD(RCC_DCKCFGR2_I2C2SEL,   18, 0x000C0000);
BIT_FIELD(RCC_DCKCFGR2_I2C3SEL,   20, 0x00300000);
BIT_FIELD(RCC_DCKCFGR2_I2C4SEL,   22, 0x00C00000);
BIT_FIELD(RCC_DCKCFGR2_LPTIM1SEL, 24, 0x03000000);
BIT_FIELD(RCC_DCKCFGR2_CECSEL,    26, 0x04000000);
BIT_FIELD(RCC_DCKCFGR2_CK48MSEL,  27, 0x08000000);
BIT_FIELD(RCC_DCKCFGR2_SDMMC1SEL, 28, 0x10000000);

/* Definitions for the USART_SEL fields in the DCKCFGR2 register. */
typedef enum {
	RCC_USARTSEL_PCLK = 0,
	RCC_USARTSEL_SYSCLK = 1,
	RCC_USARTSEL_HSI = 2,
	RCC_USARTSEL_LSE = 3
} RccUsartSel;
