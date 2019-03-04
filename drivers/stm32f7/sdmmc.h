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

/* Custom error values for SDMMC functions. */
typedef enum {
	SD_SUCCESS = 0,
	SD_FAIL = 1,
	SD_STATUS_CCRCFAIL = 2,
	SD_STATUS_CTIMEOUT = 3,
	SD_INCORRECT_RESPCMD = 4
} sd_status_t;

/* The SD Card version. */
typedef enum {
	SDCARD_V1,
	SDCARD_V2
} sdcard_version_t;

/* The type of SD Card (denotes capacity). */
typedef enum {
	SDCARD_SDSC,
	SDCARD_SDHC_SDXC
} sdcard_type_t;

/* SD Card Properties. */
typedef struct {
	sdcard_version_t version;
	sdcard_type_t type;
} SdCard;

status_t sdmmc_init();

sd_status_t send_cmd(uint8_t cmd_index,
                     uint32_t arg,
                     sd_resp_t type,
                     uint32_t *resp);

#endif /* SDMMC_H */
#endif /* INCLUDE_SDMMC_DRIVER */