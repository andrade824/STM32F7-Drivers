/**
 * @author Devon Andrade
 * @created 12/26/2018
 *
 * USART Register map [31].
 */
#ifndef USART_REG_H
#define USART_REG_H

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

/**
 * Type defining the USART module register map.
 */
typedef struct
{
	volatile uint32_t CR1;    /*!< USART Control register 1,                 Address offset: 0x00 */
	volatile uint32_t CR2;    /*!< USART Control register 2,                 Address offset: 0x04 */
	volatile uint32_t CR3;    /*!< USART Control register 3,                 Address offset: 0x08 */
	volatile uint32_t BRR;    /*!< USART Baud rate register,                 Address offset: 0x0C */
	volatile uint32_t GTPR;   /*!< USART Guard time and prescaler register,  Address offset: 0x10 */
	volatile uint32_t RTOR;   /*!< USART Receiver Time Out register,         Address offset: 0x14 */
	volatile uint32_t RQR;    /*!< USART Request register,                   Address offset: 0x18 */
	volatile uint32_t ISR;    /*!< USART Interrupt and status register,      Address offset: 0x1C */
	volatile uint32_t ICR;    /*!< USART Interrupt flag Clear register,      Address offset: 0x20 */
	volatile uint32_t RDR;    /*!< USART Receive Data register,              Address offset: 0x24 */
	volatile uint32_t TDR;    /*!< USART Transmit Data register,             Address offset: 0x28 */
} UsartReg;

/**
 * Define the USART register map accessors.
 */
#define USART1_BASE (APB2PERIPH_BASE + 0x1000U)
#define USART2_BASE (APB1PERIPH_BASE + 0x4400U)
#define USART3_BASE (APB1PERIPH_BASE + 0x4800U)
#define UART4_BASE  (APB1PERIPH_BASE + 0x4C00U)
#define UART5_BASE  (APB1PERIPH_BASE + 0x5000U)
#define USART6_BASE (APB2PERIPH_BASE + 0x1400U)
#define UART7_BASE  (APB1PERIPH_BASE + 0x7800U)
#define UART8_BASE  (APB1PERIPH_BASE + 0x7C00U)

#define USART1 ((UsartReg *) USART1_BASE)
#define USART2 ((UsartReg *) USART2_BASE)
#define USART3 ((UsartReg *) USART3_BASE)
#define UART4  ((UsartReg *) UART4_BASE)
#define UART5  ((UsartReg *) UART5_BASE)
#define USART6 ((UsartReg *) USART6_BASE)
#define UART7  ((UsartReg *) UART7_BASE)
#define UART8  ((UsartReg *) UART8_BASE)

/**
 * Control Register 1 [31.8.1].
 */
BIT_FIELD2(USART_CR1_UE,      0, 0);
BIT_FIELD2(USART_CR1_RE,      2, 2);
BIT_FIELD2(USART_CR1_TE,      3, 3);
BIT_FIELD2(USART_CR1_IDLEIE,  4, 4);
BIT_FIELD2(USART_CR1_RXNEIE,  5, 5);
BIT_FIELD2(USART_CR1_TCIE,    6, 6);
BIT_FIELD2(USART_CR1_TXEIE,   7, 7);
BIT_FIELD2(USART_CR1_PEIE,    8, 8);
BIT_FIELD2(USART_CR1_PS,      9, 9);
BIT_FIELD2(USART_CR1_PCE,    10, 10);
BIT_FIELD2(USART_CR1_WAKE,   11, 11);
BIT_FIELD2(USART_CR1_M0,     12, 12);
BIT_FIELD2(USART_CR1_MME,    13, 13);
BIT_FIELD2(USART_CR1_CMIE,   14, 14);
BIT_FIELD2(USART_CR1_OVER8,  15, 15);
BIT_FIELD2(USART_CR1_DEDT,   16, 20);
BIT_FIELD2(USART_CR1_DEAT,   21, 25);
BIT_FIELD2(USART_CR1_RTOIE,  26, 26);
BIT_FIELD2(USART_CR1_EOBIE,  27, 27);
BIT_FIELD2(USART_CR1_M1,     28, 28);

/* USART Data bit length options. */
typedef enum {
	USART_8_DATA = 0,
	USART_9_DATA = 1,
	USART_7_DATA = 2
} UsartWordLength;

/**
 * Control Register 2 [31.8.2].
 */
BIT_FIELD2(USART_CR2_ADDM7,     4, 4);
BIT_FIELD2(USART_CR2_LBDL,      5, 5);
BIT_FIELD2(USART_CR2_LBDIE,     6, 6);
BIT_FIELD2(USART_CR2_LBCL,      8, 8);
BIT_FIELD2(USART_CR2_CPHA,      9, 9);
BIT_FIELD2(USART_CR2_CPOL,     10, 10);
BIT_FIELD2(USART_CR2_CLKEN,    11, 11);
BIT_FIELD2(USART_CR2_STOP,     12, 13);
BIT_FIELD2(USART_CR2_LINEN,    14, 14);
BIT_FIELD2(USART_CR2_SWAP,     15, 15);
BIT_FIELD2(USART_CR2_RXINV,    16, 16);
BIT_FIELD2(USART_CR2_TXINV,    17, 17);
BIT_FIELD2(USART_CR2_DATAINV,  18, 18);
BIT_FIELD2(USART_CR2_MSBFIRST, 19, 19);
BIT_FIELD2(USART_CR2_ABREN,    20, 20);
BIT_FIELD2(USART_CR2_ABRMOD,   21, 22);
BIT_FIELD2(USART_CR2_RTOEN,    23, 23);
BIT_FIELD2(USART_CR2_ADD,      24, 31);

/* USART Stop bit length options. */
typedef enum {
	USART_1_STOP = 0,
	USART_0_5_STOP = 1,
	USART_2_STOP = 2,
	USART_1_5_STOP = 3
} UsartStopBits;

/**
 * Control Register 3 [31.8.3].
 */
BIT_FIELD2(USART_CR3_EIE,      0, 0);
BIT_FIELD2(USART_CR3_IREN,     1, 1);
BIT_FIELD2(USART_CR3_IRLP,     2, 2);
BIT_FIELD2(USART_CR3_HDSEL,    3, 3);
BIT_FIELD2(USART_CR3_NACK,     4, 4);
BIT_FIELD2(USART_CR3_SCEN,     5, 5);
BIT_FIELD2(USART_CR3_DMAR,     6, 6);
BIT_FIELD2(USART_CR3_DMAT,     7, 7);
BIT_FIELD2(USART_CR3_RTSE,     8, 8);
BIT_FIELD2(USART_CR3_CTSE,     9, 9);
BIT_FIELD2(USART_CR3_CTSIE,   10, 10);
BIT_FIELD2(USART_CR3_ONEBIT,  11, 11);
BIT_FIELD2(USART_CR3_OVRDIS,  12, 12);
BIT_FIELD2(USART_CR3_DDRE,    13, 13);
BIT_FIELD2(USART_CR3_DEM,     14, 14);
BIT_FIELD2(USART_CR3_DEP,     15, 15);
BIT_FIELD2(USART_CR3_SCARCNT, 17, 19);

/**
 * Baud Rate Register [31.8.4].
 */
BIT_FIELD2(USART_BRR_BRR, 0, 15);

/**
 * Interrupt and Status Register [31.8.8].
 */
BIT_FIELD2(USART_ISR_PE,     0, 0);
BIT_FIELD2(USART_ISR_FE,     1, 1);
BIT_FIELD2(USART_ISR_NF,     2, 2);
BIT_FIELD2(USART_ISR_ORE,    3, 3);
BIT_FIELD2(USART_ISR_IDLE,   4, 4);
BIT_FIELD2(USART_ISR_RXNE,   5, 5);
BIT_FIELD2(USART_ISR_TC,     6, 6);
BIT_FIELD2(USART_ISR_TXE,    7, 7);
BIT_FIELD2(USART_ISR_LBDF,   8, 8);
BIT_FIELD2(USART_ISR_CTSIF,  9, 9);
BIT_FIELD2(USART_ISR_CTS,   10, 10);
BIT_FIELD2(USART_ISR_RTOF,  11, 11);
BIT_FIELD2(USART_ISR_EOBF,  12, 12);
BIT_FIELD2(USART_ISR_ABRE,  14, 14);
BIT_FIELD2(USART_ISR_ABRF,  15, 15);
BIT_FIELD2(USART_ISR_BUSY,  16, 16);
BIT_FIELD2(USART_ISR_CMF,   17, 17);
BIT_FIELD2(USART_ISR_SBKF,  18, 18);
BIT_FIELD2(USART_ISR_RWU,   19, 19);
BIT_FIELD2(USART_ISR_TEACK, 21, 21);

/**
 * Interrupt Flag Clear Register [31.8.9].
 */
BIT_FIELD2(USART_ICR_PECF,   0, 0);
BIT_FIELD2(USART_ICR_FECF,   1, 1);
BIT_FIELD2(USART_ICR_NCF,    2, 2);
BIT_FIELD2(USART_ICR_ORECF,  3, 3);
BIT_FIELD2(USART_ICR_IDLECF, 4, 4);
BIT_FIELD2(USART_ICR_TCCF,   6, 6);
BIT_FIELD2(USART_ICR_LBDFCF, 8, 8);
BIT_FIELD2(USART_ICR_CTSCF,  9, 9);
BIT_FIELD2(USART_ICR_RTOCF, 11, 11);
BIT_FIELD2(USART_ICR_EOBCF, 12, 12);
BIT_FIELD2(USART_ICR_CMCF,  17, 17);

/**
 * Receive Data Register [31.8.10].
 */
BIT_FIELD2(USART_RDR_RDR, 0, 8);

/**
 * Transmit Data Register [31.8.11].
 */
BIT_FIELD2(USART_TDR_TDR, 0, 8);

#endif
