/**
 * @author Devon Andrade
 * @created 2/17/2019
 *
 * SD/MMC Controller Register Map.
 */
#pragma once

#ifdef INCLUDE_SDMMC_DRIVER

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

typedef struct {
	volatile uint32_t        POWER;         /* SDMMC power control register,    Address offset: 0x00 */
	volatile uint32_t        CLKCR;         /* SDMMC clock control register,    Address offset: 0x04 */
	volatile uint32_t        ARG;           /* SDMMC argument register,         Address offset: 0x08 */
	volatile uint32_t        CMD;           /* SDMMC command register,          Address offset: 0x0C */
	const volatile uint32_t  RESPCMD;       /* SDMMC command response register, Address offset: 0x10 */
	const volatile uint32_t  RESP1;         /* SDMMC response 1 register,       Address offset: 0x14 */
	const volatile uint32_t  RESP2;         /* SDMMC response 2 register,       Address offset: 0x18 */
	const volatile uint32_t  RESP3;         /* SDMMC response 3 register,       Address offset: 0x1C */
	const volatile uint32_t  RESP4;         /* SDMMC response 4 register,       Address offset: 0x20 */
	volatile uint32_t        DTIMER;        /* SDMMC data timer register,       Address offset: 0x24 */
	volatile uint32_t        DLEN;          /* SDMMC data length register,      Address offset: 0x28 */
	volatile uint32_t        DCTRL;         /* SDMMC data control register,     Address offset: 0x2C */
	const volatile uint32_t  DCOUNT;        /* SDMMC data counter register,     Address offset: 0x30 */
	const volatile uint32_t  STA;           /* SDMMC status register,           Address offset: 0x34 */
	volatile uint32_t        ICR;           /* SDMMC interrupt clear register,  Address offset: 0x38 */
	volatile uint32_t        MASK;          /* SDMMC mask register,             Address offset: 0x3C */
	uint32_t                 RESERVED0[2];  /* Reserved, 0x40-0x44                                  */
	const volatile uint32_t  FIFOCNT;       /* SDMMC FIFO counter register,     Address offset: 0x48 */
	uint32_t                 RESERVED1[13]; /* Reserved, 0x4C-0x7C                                  */
	volatile uint32_t        FIFO;          /* SDMMC data FIFO register,        Address offset: 0x80 */
} SdmmcReg;

/* Define the SDMMC Controller register map accessor. */
#define SDMMC1_BASE (APB2PERIPH_BASE + 0x2C00U)
#define SDMMC1      ((SdmmcReg *)SDMMC1_BASE)

/* SDMMC Power Control Register. */
BIT_FIELD2(SDMMC_POWER_PWRCTL, 0, 1);

typedef enum {
	SD_POWER_OFF = 0,
	SD_POWER_ON = 3
} SdPwrCtrl;

/* SDMMC Clock Control Register. */
BIT_FIELD2(SDMMC_CLKCR_CLKDIV,   0, 7);
BIT_FIELD2(SDMMC_CLKCR_CLKEN,    8, 8);
BIT_FIELD2(SDMMC_CLKCR_PWRSAV,   9, 9);
BIT_FIELD2(SDMMC_CLKCR_BYPASS,  10, 10);
BIT_FIELD2(SDMMC_CLKCR_WIDBUS,  11, 12);
BIT_FIELD2(SDMMC_CLKCR_NEGEDGE, 13, 13);
BIT_FIELD2(SDMMC_CLKCR_HWFC_EN, 14, 14);

/* How wide the bus is (WIDBUS field). */
typedef enum {
	SD_1_BIT = 0,
	SD_4_BIT = 1,
	SD_8_BIT = 2
} SdWidBus;

/* SDMMC Argument Register. */
BIT_FIELD2(SDMMC_ARG_CMDARG, 0, 31);

/* SDMMC Command Register. */
BIT_FIELD2(SDMMC_CMD_CMDINDEX,     0, 5);
BIT_FIELD2(SDMMC_CMD_WAITRESP,     6, 7);
BIT_FIELD2(SDMMC_CMD_WAITINT,      8, 8);
BIT_FIELD2(SDMMC_CMD_WAITPEND,     9, 9);
BIT_FIELD2(SDMMC_CMD_CPSMEN,      10, 10);
BIT_FIELD2(SDMMC_CMD_SDIOSUSPEND, 11, 11);

/* The type of response expected from a sent command (WAITRESP field). */
typedef enum {
	SD_NO_RESP = 0,
	SD_SHORT_RESP = 1,
	SD_LONG_RESP = 3
} SdResp;

/* SDMMC Command Response Register. */
BIT_FIELD2(SDMMC_RESPCMD_RESPCMD, 0, 5);

/* SDMMC Response Register. */
BIT_FIELD2(SDMMC_RESP_CARDSTATUS, 0, 31);

/* SDMMC Data Timer Register. */
BIT_FIELD2(SDMMC_DTIMER_DATATIME, 0, 31);

/* SDMMC Data Length Register. */
BIT_FIELD2(SDMMC_DLEN_DATALENGTH, 0, 24);

/* SDMMC Data Control Register. */
BIT_FIELD2(SDMMC_DCTRL_DTEN,       0, 0);
BIT_FIELD2(SDMMC_DCTRL_DTDIR,      1, 1);
BIT_FIELD2(SDMMC_DCTRL_DTMODE,     2, 2);
BIT_FIELD2(SDMMC_DCTRL_DMAEN,      3, 3);
BIT_FIELD2(SDMMC_DCTRL_DBLOCKSIZE, 4, 7);
BIT_FIELD2(SDMMC_DCTRL_RWSTART,    8, 8);
BIT_FIELD2(SDMMC_DCTRL_RWSTOP,     9, 9);
BIT_FIELD2(SDMMC_DCTRL_RWMOD,     10, 10);
BIT_FIELD2(SDMMC_DCTRL_SDIOEN,    11, 11);

/* Direction of the data transfer (DTDIR field). */
typedef enum {
	SD_TO_CARD   = 0,
	SD_FROM_CARD = 1
} SdTransferDir;

/* Block or stream data transfer (DTMODE field). */
typedef enum {
	SD_BLOCK_TRANSFER  = 0,
	SD_STREAM_TRANSFER = 1
} SdTransferMode;

/* Whether the DMA is enabled for a transfer (DMAEN field). */
typedef enum {
	SD_DMA_DISABLED = 0,
	SD_DMA_ENABLED  = 1
} SdDmaEnabled;

/* The data block size (parameters for the DBLOCKSIZE field). */
typedef enum {
	SD_1_BYTE      = 0,
	SD_2_BYTES     = 1,
	SD_4_BYTES     = 2,
	SD_8_BYTES     = 3,
	SD_16_BYTES    = 4,
	SD_32_BYTES    = 5,
	SD_64_BYTES    = 6,
	SD_128_BYTES   = 7,
	SD_256_BYTES   = 8,
	SD_512_BYTES   = 9,
	SD_1024_BYTES  = 10,
	SD_2048_BYTES  = 11,
	SD_4096_BYTES  = 12,
	SD_8192_BYTES  = 13,
	SD_16384_BYTES = 14
} SdBlockSize;

/* SDMMC Data Counter Register. */
BIT_FIELD2(SDMMC_DCOUNT_DATACOUNT, 0, 24);

/* SDMMC Status Register. */
BIT_FIELD2(SDMMC_STA_CCRCFAIL,  0, 0);
BIT_FIELD2(SDMMC_STA_DCRCFAIL,  1, 1);
BIT_FIELD2(SDMMC_STA_CTIMEOUT,  2, 2);
BIT_FIELD2(SDMMC_STA_DTIMEOUT,  3, 3);
BIT_FIELD2(SDMMC_STA_TXUNDERR,  4, 4);
BIT_FIELD2(SDMMC_STA_RXOVERR,   5, 5);
BIT_FIELD2(SDMMC_STA_CMDREND,   6, 6);
BIT_FIELD2(SDMMC_STA_CMDSENT,   7, 7);
BIT_FIELD2(SDMMC_STA_DATAEND,   8, 8);
BIT_FIELD2(SDMMC_STA_DBCKEND,  10, 10);
BIT_FIELD2(SDMMC_STA_CMDACT,   11, 11);
BIT_FIELD2(SDMMC_STA_TXACT,    12, 12);
BIT_FIELD2(SDMMC_STA_RXACT,    13, 13);
BIT_FIELD2(SDMMC_STA_TXFIFOHE, 14, 14);
BIT_FIELD2(SDMMC_STA_RXFIFOHF, 15, 15);
BIT_FIELD2(SDMMC_STA_TXFIFOF,  16, 16);
BIT_FIELD2(SDMMC_STA_RXFIFOF,  17, 17);
BIT_FIELD2(SDMMC_STA_TXFIFOE,  18, 18);
BIT_FIELD2(SDMMC_STA_RXFIFOE,  19, 19);
BIT_FIELD2(SDMMC_STA_TXDAVL,   20, 20);
BIT_FIELD2(SDMMC_STA_RXDAVL,   21, 21);
BIT_FIELD2(SDMMC_STA_SDIOIT,   22, 22);

/* SDMMC Interrupt Clear Register. */
BIT_FIELD2(SDMMC_ICR_CCRCFAILC, 0, 0);
BIT_FIELD2(SDMMC_ICR_DCRCFAILC, 1, 1);
BIT_FIELD2(SDMMC_ICR_CTIMEOUTC, 2, 2);
BIT_FIELD2(SDMMC_ICR_DTIMEOUTC, 3, 3);
BIT_FIELD2(SDMMC_ICR_TXUNDERRC, 4, 4);
BIT_FIELD2(SDMMC_ICR_RXOVERRC,  5, 5);
BIT_FIELD2(SDMMC_ICR_CMDRENDC,  6, 6);
BIT_FIELD2(SDMMC_ICR_CMDSENTC,  7, 7);
BIT_FIELD2(SDMMC_ICR_DATAENDC,  8, 8);
BIT_FIELD2(SDMMC_ICR_DBCKENDC, 10, 10);
BIT_FIELD2(SDMMC_ICR_SDIOITC,  22, 22);

/* SDMMC Mask Register. */
BIT_FIELD2(SDMMC_MASK_CCRCFAILIE, 0, 0);
BIT_FIELD2(SDMMC_MASK_DCRCFAILIE, 1, 1);
BIT_FIELD2(SDMMC_MASK_CTIMEOUTIE, 2, 2);
BIT_FIELD2(SDMMC_MASK_DTIMEOUTIE, 3, 3);
BIT_FIELD2(SDMMC_MASK_TXUNDERRIE, 4, 4);
BIT_FIELD2(SDMMC_MASK_RXOVERRIE,  5, 5);
BIT_FIELD2(SDMMC_MASK_CMDRENDIE,   6, 6);
BIT_FIELD2(SDMMC_MASK_CMDSENTIE,  7, 7);
BIT_FIELD2(SDMMC_MASK_DATAENDIE,  8, 8);
BIT_FIELD2(SDMMC_MASK_DBCKENDIE, 10, 10);
BIT_FIELD2(SDMMC_MASK_CMDACTIE,  11, 11);
BIT_FIELD2(SDMMC_MASK_TXACTIE,   12, 12);
BIT_FIELD2(SDMMC_MASK_RXACTIE,   13, 13);
BIT_FIELD2(SDMMC_MASK_TXFIFOHE,  14, 14);
BIT_FIELD2(SDMMC_MASK_RXFIFOHF,  15, 15);
BIT_FIELD2(SDMMC_MASK_TXFIFOFIE, 16, 16);
BIT_FIELD2(SDMMC_MASK_RXFIFOFIE, 17, 17);
BIT_FIELD2(SDMMC_MASK_TXFIFOEIE, 18, 18);
BIT_FIELD2(SDMMC_MASK_RXFIFOEIE, 19, 19);
BIT_FIELD2(SDMMC_MASK_TXDAVLIE,  20, 20);
BIT_FIELD2(SDMMC_MASK_RXDAVLIE,  21, 21);
BIT_FIELD2(SDMMC_MASK_SDIOITIE,  22, 22);

/* SDMMC FIFO Counter Register. */
BIT_FIELD2(SDMMC_FIFOCNT_FIFOCOUNT, 0, 23);

/* SDMMC Data FIFO Register. */
BIT_FIELD2(SDMMC_FIFO_FIFODATA, 0, 31);

#endif /* INCLUDE_SDMMC_DRIVER */
