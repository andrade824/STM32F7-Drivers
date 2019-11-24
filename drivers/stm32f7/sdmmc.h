/**
 * @author Devon Andrade
 * @created 3/3/2019
 *
 * Definitions and functions used to manipulate the SDMMC.
 */
#pragma once

#if INCLUDE_SDMMC_DRIVER

#include "registers/sdmmc_reg.h"

#include <stdint.h>

/* SD State Machine */
typedef enum {
	SD_IDENT_STATE,
	SD_DETERMINE_VERSION_STATE,
	SD_WAIT_POWER_UP_STATE,
	SD_GET_CID_STATE,
	SD_GET_RCA_STATE,
	SD_GET_CSD_STATE,
	SD_TRANSFER_STATE,
	SD_READ_STATE,
	SD_WRITE_STATE
} SdState;

/* Custom error values for SDMMC functions. */
typedef enum {
	SD_FAIL                 = 0,
	SD_SUCCESS              = 1,

	/* Errors generated sending commands */
	SD_STATUS_CCRCFAIL      = 2,
	SD_STATUS_CTIMEOUT      = 3,
	SD_INCORRECT_RESPCMD    = 4,
	SD_STATUS_RXOVERR       = 5,
	SD_STATUS_TXUNDERR      = 6,
	SD_STATUS_DCRCFAIL      = 7,
	SD_STATUS_DTIMEOUT      = 8,

	/* Errors returned in R1 response */
	SD_ADDRESS_OUT_OF_RANGE = 9,
	SD_ADDRESS_MISALIGN     = 10,
	SD_BLOCK_LEN_ERROR      = 11,
	SD_ERASE_SEQ_ERROR      = 12,
	SD_ERASE_PARAM          = 13,
	SD_WP_VIOLATION         = 14,
	SD_LOCK_UNLOCK_FAILED   = 15,
	SD_COM_CRC_ERROR        = 16,
	SD_ILLEGAL_COMMAND      = 17,
	SD_CARD_ECC_FAILED      = 18,
	SD_CC_ERROR             = 19,
	SD_GENERIC_ERROR        = 20,
	SD_CID_CSD_OVERWRITE    = 21,
	SD_WP_ERASE_SKIP        = 22,
	SD_AKE_SEQ_ERROR        = 23
} SdStatus;

/* SD Card Properties. */
typedef struct {
	SdState state;
	uint32_t rca;                /* Relative Card Address */
	uint8_t manufacturer_id;
	char oem_id[3];
	char product_name[6];
	uint8_t product_rev;
	uint32_t serial_num;
	uint16_t manufacturing_date;
	uint64_t total_size;         /* Total size in bytes */
	uint32_t total_blocks;       /* Total blocks availables on device */
	uint16_t block_len;          /* Block length */
} SdCard;

SdStatus sdmmc_init();

SdStatus sd_read_data(void *data, uint32_t block_addr, uint16_t num_blocks);
SdStatus sd_write_data(void *data, uint32_t block_addr, uint16_t num_blocks);

SdCard sd_get_card_info(void);

#endif /* INCLUDE_SDMMC_DRIVER */