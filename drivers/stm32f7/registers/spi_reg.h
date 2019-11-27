/**
 * @author Devon Andrade
 * @created 11/25/2019
 *
 * SPI/I2S Register map.
 */
#pragma once

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

/* Type defining the SPI module register map. */
typedef struct {
	volatile uint32_t CR1;     /* SPI control register 1 (not used in I2S mode),      Address offset: 0x00 */
	volatile uint32_t CR2;     /* SPI control register 2,                             Address offset: 0x04 */
	volatile uint32_t SR;      /* SPI status register,                                Address offset: 0x08 */
	volatile uint32_t DR;      /* SPI data register,                                  Address offset: 0x0C */
	volatile uint32_t CRCPR;   /* SPI CRC polynomial register (not used in I2S mode), Address offset: 0x10 */
	volatile uint32_t RXCRCR;  /* SPI RX CRC register (not used in I2S mode),         Address offset: 0x14 */
	volatile uint32_t TXCRCR;  /* SPI TX CRC register (not used in I2S mode),         Address offset: 0x18 */
	volatile uint32_t I2SCFGR; /* SPI_I2S configuration register,                     Address offset: 0x1C */
	volatile uint32_t I2SPR;   /* SPI_I2S prescaler register,                         Address offset: 0x20 */
} SpiReg;

/* Define the USART register map accessors. */
#define SPI1_BASE (APB2PERIPH_BASE + 0x3000U)
#define SPI2_BASE (APB1PERIPH_BASE + 0x3800U)
#define SPI3_BASE (APB1PERIPH_BASE + 0x3C00U)
#define SPI4_BASE (APB2PERIPH_BASE + 0x3400U)
#define SPI5_BASE (APB2PERIPH_BASE + 0x5000U)
#define SPI6_BASE (APB2PERIPH_BASE + 0x5400U)

#define SPI1 ((SpiReg *) SPI1_BASE)
#define SPI2 ((SpiReg *) SPI2_BASE)
#define SPI3 ((SpiReg *) SPI3_BASE)
#define SPI4 ((SpiReg *) SPI4_BASE)
#define SPI5 ((SpiReg *) SPI5_BASE)
#define SPI6 ((SpiReg *) SPI6_BASE)

/* Control Register 1. */
BIT_FIELD2(SPI_CR1_CPHA,      0, 0);
BIT_FIELD2(SPI_CR1_CPOL,      1, 1);
BIT_FIELD2(SPI_CR1_MSTR,      2, 2);
BIT_FIELD2(SPI_CR1_BR,        3, 5);
BIT_FIELD2(SPI_CR1_SPE,       6, 6);
BIT_FIELD2(SPI_CR1_LSBFIRST,  7, 7);
BIT_FIELD2(SPI_CR1_SSI,       8, 8);
BIT_FIELD2(SPI_CR1_SSM,       9, 9);
BIT_FIELD2(SPI_CR1_RXONLY,   10, 10);
BIT_FIELD2(SPI_CR1_CRCL,     11, 11);
BIT_FIELD2(SPI_CR1_CRCNEXT,  12, 12);
BIT_FIELD2(SPI_CR1_CRCEN,    13, 13);
BIT_FIELD2(SPI_CR1_BIDIOE,   14, 14);
BIT_FIELD2(SPI_CR1_BIDIMODE, 15, 15);

/* SPI_CR1_CPHA (Clock Phase) field options. */
typedef enum {
	SPI_CPHA_0 = 0,
	SPI_CPHA_1 = 1
} SpiClockPhase;

/* SPI_CR1_CPOL (Clock Polarity) field options. */
typedef enum {
	SPI_CPOL_0 = 0,
	SPI_CPOL_1 = 1
} SpiClockPolarity;

/* SPI_CR1_BR (Baud rate control) field options. */
typedef enum {
	SPI_BR_DIV_2   = 0,
	SPI_BR_DIV_4   = 1,
	SPI_BR_DIV_8   = 2,
	SPI_BR_DIV_16  = 3,
	SPI_BR_DIV_32  = 4,
	SPI_BR_DIV_64  = 5,
	SPI_BR_DIV_128 = 6,
	SPI_BR_DIV_256 = 7
} SpiBaudRateDiv;

/* SPI_CR1_LSBFIRST field options. */
typedef enum {
	SPI_MSBFIRST = 0,
	SPI_LSBFIRST = 1
} SpiLsbFirst;

/* Control Register 2. */
BIT_FIELD2(SPI_CR2_RXDMAEN,  0, 0);
BIT_FIELD2(SPI_CR2_TXDMAEN,  1, 1);
BIT_FIELD2(SPI_CR2_SSOE,     2, 2);
BIT_FIELD2(SPI_CR2_NSSP,     3, 3);
BIT_FIELD2(SPI_CR2_FRF,      4, 4);
BIT_FIELD2(SPI_CR2_ERRIE,    5, 5);
BIT_FIELD2(SPI_CR2_RXNEIE,   6, 6);
BIT_FIELD2(SPI_CR2_TXEIE,    7, 7);
BIT_FIELD2(SPI_CR2_DS,       8, 11);
BIT_FIELD2(SPI_CR2_FRXTH,   12, 12);
BIT_FIELD2(SPI_CR2_LDMA_RX, 13, 13);
BIT_FIELD2(SPI_CR2_LDMA_TX, 14, 14);

/* SPI_CR2_DS (Data Size) field options. */
typedef enum {
	SPI_DS_4BIT  =  3,
	SPI_DS_5BIT  =  4,
	SPI_DS_6BIT  =  5,
	SPI_DS_7BIT  =  6,
	SPI_DS_8BIT  =  7,
	SPI_DS_9BIT  =  8,
	SPI_DS_10BIT =  9,
	SPI_DS_11BIT = 10,
	SPI_DS_12BIT = 11,
	SPI_DS_13BIT = 12,
	SPI_DS_14BIT = 13,
	SPI_DS_15BIT = 14,
	SPI_DS_16BIT = 15
} SpiDataSize;

/* Status Register. */
BIT_FIELD2(SPI_SR_RXNE,   0, 0);
BIT_FIELD2(SPI_SR_TXE,    1, 1);
BIT_FIELD2(SPI_SR_CHSIDE, 2, 2);
BIT_FIELD2(SPI_SR_UDR,    3, 3);
BIT_FIELD2(SPI_SR_CRCERR, 4, 4);
BIT_FIELD2(SPI_SR_MODF,   5, 5);
BIT_FIELD2(SPI_SR_OVR,    6, 6);
BIT_FIELD2(SPI_SR_BSY,    7, 7);
BIT_FIELD2(SPI_SR_FRE,    8, 8);
BIT_FIELD2(SPI_SR_FRLVL,  9, 10);
BIT_FIELD2(SPI_SR_FTLVL, 11, 12);

/* Data Register. */
BIT_FIELD2(SPI_DR_DR, 0, 15);

/* CRC Polyomial Register. */
BIT_FIELD2(SPI_CRCPR_CRCPOLY, 0, 15);

/* Rx CRC Register. */
BIT_FIELD2(SPI_RXCRCR_RXCRC, 0, 15);

/* Tx CRC Register. */
BIT_FIELD2(SPI_TXCRCR_TXCRC, 0, 15);

/* I2S Configuration Register. */
BIT_FIELD2(SPI_I2SCFGR_CHLEN,    0, 0);
BIT_FIELD2(SPI_I2SCFGR_DATLEN,   1, 2);
BIT_FIELD2(SPI_I2SCFGR_CKPOL,    3, 3);
BIT_FIELD2(SPI_I2SCFGR_I2SSTD,   4, 5);
BIT_FIELD2(SPI_I2SCFGR_PCMSYNC,  7, 7);
BIT_FIELD2(SPI_I2SCFGR_I2SCFG,   8, 9);
BIT_FIELD2(SPI_I2SCFGR_I2SE,    10, 10);
BIT_FIELD2(SPI_I2SCFGR_I2SMOD,  11, 11);
BIT_FIELD2(SPI_I2SCFGR_ASTRTEN, 12, 12);

/* I2S Prescaler Register. */
BIT_FIELD2(SPI_I2SPR_I2SDIV, 0, 7);
BIT_FIELD2(SPI_I2SPR_ODD,    8, 8);
BIT_FIELD2(SPI_I2SPR_MCKOE,  9, 9);
