/**
 * @author Devon Andrade
 * @created 3/3/2019
 *
 * Definitions and functions used to manipulate the SDMMC.
 */
#if INCLUDE_SDMMC_DRIVER
#ifndef SDMMC_H
#define SDMMC_H

#include "status.h"

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
	SD_TRANSFER_STATE
} sd_state_t;

/* Custom error values for SDMMC functions. */
typedef enum {
	SD_SUCCESS,
	SD_FAIL,

	/* Errors generated sending commands */
	SD_STATUS_CCRCFAIL,
	SD_STATUS_CTIMEOUT,
	SD_INCORRECT_RESPCMD,

	/* Errors returned in R1 response */
	SD_ADDRESS_OUT_OF_RANGE,
	SD_ADDRESS_MISALIGN,
	SD_BLOCK_LEN_ERROR,
	SD_ERASE_SEQ_ERROR,
	SD_ERASE_PARAM,
	SD_WP_VIOLATION,
	SD_LOCK_UNLOCK_FAILED,
	SD_COM_CRC_ERROR,
	SD_ILLEGAL_COMMAND,
	SD_CARD_ECC_FAILED,
	SD_CC_ERROR,
	SD_GENERIC_ERROR,
	SD_CID_CSD_OVERWRITE,
	SD_WP_ERASE_SKIP,
	SD_AKE_SEQ_ERROR
} sd_status_t;

/* SD Card Properties. */
typedef struct {
	sd_state_t state;
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

status_t sdmmc_init();

sd_status_t send_cmd(uint8_t cmd_index,
                     uint32_t arg,
                     sd_resp_t type,
                     uint32_t *resp);

#endif /* SDMMC_H */
#endif /* INCLUDE_SDMMC_DRIVER */