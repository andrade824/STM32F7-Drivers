/**
 * @author Devon Andrade
 * @created 3/3/2019
 *
 * Definitions and functions used to manipulate the SDMMC.
 */
#if INCLUDE_SDMMC_DRIVER

#include "config.h"
#include "debug.h"
#include "sdmmc.h"
#include "status.h"
#include "system_timer.h"

#include "registers/rcc_reg.h"
#include "registers/sdmmc_reg.h"

/**
 * Set the default SDMMC module to use.
 *
 * This can be overridden in the config file.
 */
#ifndef SDMMC
	#define SDMMC SDMMC1
#endif

/* The clock speed of the SD Card during card initialization. */
#define SD_INIT_HZ 400000U

/* The clock divider value required to achieve the SD_INIT_HZ speed. */
#define SD_CLKDIV SDMMC_HZ / SD_INIT_HZ

/* How long to wait after the SD clock has been enabled. */
#define SD_POWER_ON_DELAY MSECS(2)

/* SD Card Commands */
typedef enum {
	SD_CMD0_GO_IDLE_STATE         =  0,
	SD_CMD1_ALL_SEND_CID          =  2,
	SD_CMD3_SEND_RELATIVE_ADDR    =  3,
	SD_CMD7_SELECT_CARD           =  7,
	SD_CMD8_SEND_IF_COND          =  8,
	SD_CMD9_SEND_CSD              =  9,
	SD_CMD10_SEND_CID             = 10,
	SD_CMD12_STOP_TRANSMISSION    = 12,
	SD_CMD13_SEND_STATUS          = 13,
	SD_CMD15_GO_INACTIVE_STATE    = 15,
	SD_CMD16_SET_BLOCKLEN         = 16,
	SD_CMD17_READ_SINGLE_BLOCK    = 17,
	SD_CMD18_READ_MULTIPLE_BLOCK  = 18,
	SD_CMD23_SET_BLOCK_COUNT      = 23,
	SD_CMD24_WRITE_BLOCK          = 24,
	SD_CMD25_WRITE_MULTIPLE_BLOCK = 25,
	SD_CMD55_APP_CMD              = 55
} sd_cmd_t;

/* SD Card Application-Specific Commands */
typedef enum {
	SD_ACMD6_SET_BUS_WIDTH           =  6,
	SD_ACMD13_SD_STATUS              = 13,
	SD_ACMD22_SEND_NUM_WR_BLOCKS     = 22,
	SD_ACMD23_SET_WR_BLK_ERASE_COUNT = 23,
	SD_ACMD41_SEND_OP_COND           = 41
} sd_app_cmd_t;

/**
 * CMD8 Fields.
 *
 * The check pattern can be any 8-bit value. This exact value should be returned
 * in the response.
 *
 * A supply voltage of "0x1" represents "2.7-3.6V".
 */
#define CMD8_CHECK_PATTERN 0xAA
#define CMD8_VOLTAGE       0x100
#define CMD8_ARG           (CMD8_CHECK_PATTERN | CMD8_VOLTAGE)

/**
 * ACMD41 Fields.
 *
 * ACMD41_MAX_TRIES determines how many times to attempt to send the ACMD41
 * command (and receiving the busy bit) before giving up.
 *
 * ACMD41_VOLTAGE is the voltage window the card should adhere to.
 *
 * ACMD41_HIGH_CAPACITY is set if the card is a version 2.0 card.
 *
 * ACMD41_BUSY_BIT is low for as long as the card is still powering up.
 */
#define ACMD41_MAX_TRIES     0xFFFFU
#define ACMD41_VOLTAGE       0x00100000U /* 3.2-3.3V window */
#define ACMD41_HIGH_CAPACITY 0x40000000U
#define ACMD41_BUSY_BIT      0x80000000U /* Low = Busy, High = Powered Up */

#define SDMMC_R1_ERRORBITS 0xFDFFE008U

/* Properties of the connected SD Card. */
static SdCard card;

/**
 * Clear all status flags that may have been set.
 */
void clear_all_flags(void) {
	/* TODO: Figure out why I can't just use individual flags */
	// SDMMC->ICR = SDMMC_ICR_CCRCFAILC() | SDMMC_ICR_DCRCFAILC(), SDMMC_ICR_CTIMEOUTC() |
	//              SDMMC_ICR_DTIMEOUTC() | SDMMC_ICR_TXUNDERRC() | SDMMC_ICR_RXOVERRC() |
	//              SDMMC_ICR_CMDRENDC() | SDMMC_ICR_CMDSENTC() | SDMMC_ICR_DATAENDC() |
	//              SDMMC_ICR_DBCKENDC() | SDMMC_ICR_SDIOITC();
	SDMMC->ICR = 0xFFFFFFFF;
}

/**
 * Check the error bits in an R1 response and return an error if any of them
 * are set.
 */
sd_status_t check_r1_resp(uint32_t resp) {
	/******** TODO: ACTUALLY SET REAL ERROR CODES *********/
	if(resp & SDMMC_R1_ERRORBITS) {
		return SD_FAIL;
	}

	return SD_SUCCESS;
}

/**
 * Attempt to initialize a connected SD Card. If there's an error while
 * communicating with the card then Failure is returned.
 *
 * @note Before usage, the SDMMC GPIOs and 48MHz clock will need to be setup.
 */
status_t sdmmc_init() {
	sd_status_t status = 0;

	/**
	 * Enable the SDMMC APB2 clock.
	 */
	uint32_t rcc_config = 0;
	if(SDMMC == SDMMC1) {
		rcc_config = RCC_APB2ENR_SDMMC1EN();
	} else {
		rcc_config = RCC_APB2ENR_SDMMC2EN();
	}

	SET_FIELD(RCC->APB2ENR, rcc_config);
	__asm("dsb");

	/* Enable the 400KHz SD clock. */
	SET_FIELD(SDMMC->POWER, SET_SDMMC_POWER_PWRCTL(SD_POWER_ON));
	SET_FIELD(SDMMC->CLKCR, SET_SDMMC_CLKCR_CLKDIV(SD_CLKDIV) | SDMMC_CLKCR_CLKEN());

	/* TODO: Is this needed? Can the delay be reduced? */
	ABORT_IF_NOT(sleep(SD_POWER_ON_DELAY));

	/* Place card into IDLE state. */
	status = send_cmd(SD_CMD0_GO_IDLE_STATE, 0, SD_NO_RESP, NULL);
	if(status != SD_SUCCESS) {
		dbprintf("[SDMMC] Failed to send CMD0 %u\n", status);
		return status;
	}

	/** TODO: BREAK THESE INTO SMALLER FUNCTIONS **/

	uint32_t resp = 0;
	status = send_cmd(SD_CMD8_SEND_IF_COND, CMD8_ARG, SD_SHORT_RESP, &resp);
	if(status != SD_SUCCESS) {
		dbprintf("[SDMMC] CMD8 failed, assuming v1.0 card. %d\n", status);
		card.version = SDCARD_V1;
	} else {
		/* Verify the "check pattern" matches what was sent. */
		if(resp != CMD8_ARG) {
			dbprintf("[SDMMC] Response from CMD8 didn't match sent check pattern.\n");
			return Fail;
		}

		dbprintf("[SDMMC] CMD8 succeeded, assuming v2.0 card.\n");
		card.version = SDCARD_V2;
	}

	/**
	 * The argument to ACMD41 indicates the wanted voltage window and whether
	 * the card is expected to be high capacity (based on its version).
	 */
	uint32_t acmd41_arg = ACMD41_VOLTAGE;
	acmd41_arg |= (card.version == SDCARD_V2) ? ACMD41_HIGH_CAPACITY : 0;

	uint32_t acmd41_tries = 0;
	while(acmd41_tries < ACMD41_MAX_TRIES) {
		/* Send APP_CMD, default RCA is 0. */
		status = send_cmd(SD_CMD55_APP_CMD, 0, SD_SHORT_RESP, &resp);
		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] CMD55 failed to send.\n");
			return Fail;
		}

		if(check_r1_resp(resp) != SD_SUCCESS) {
			dbprintf("[SDMMC] Response from CMD55 had error bits set.\n");
			return Fail;
		}

		/* Send ACMD41 */
		status = send_cmd(SD_ACMD41_SEND_OP_COND, acmd41_arg, SD_SHORT_RESP, &resp);

		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] ACMD41 failed to send.\n");
			return Fail;
		}

		if(resp & ACMD41_BUSY_BIT) {
			break;
		}

		acmd41_tries++;
	}

	if(acmd41_tries == ACMD41_MAX_TRIES) {
		dbprintf("[SDMMC] Timed out waiting for ACMD41 to set power up status bit.\n");
		return Fail;
	}

	dbprintf("Number ACMD41 tries: %lu\n", acmd41_tries);

	if(resp & ACMD41_HIGH_CAPACITY) {
		card.type = SDCARD_SDHC_SDXC;
		dbprintf("[SDMMC] Detected SDHC/SDXC card.\n");
	} else {
		card.type = SDCARD_SDSC;
		dbprintf("[SDMMC] Detected SDSC card.\n");
	}

	return Success;
}

/**
 * Send a command to the SD card.
 *
 * @param cmd_index The index of the command to send.
 * @param arg The 32-bit argument to the command.
 * @param type The type of the response (none, short, or long).
 * @param resp This parameter stores the returned response from the SD Card.
 *
 * @note If there is no expected response, set "resp" to NULL. A "short" response
 * must be a uint32_t. A "long" response must be an array of four uint32_t's.
 */
sd_status_t send_cmd(uint8_t cmd_index,
                     uint32_t arg,
                     sd_resp_t type,
                     uint32_t *resp) {
	if(type != SD_NO_RESP) {
		ASSERT(resp != NULL);
	}

	SDMMC->ARG = arg;
	SDMMC->CMD = SET_SDMMC_CMD_CMDINDEX(cmd_index) | SET_SDMMC_CMD_WAITRESP(type) |
	             SDMMC_CMD_CPSMEN();

	/* Wait for the command to send. */
	uint32_t status_flags = SDMMC_STA_CCRCFAIL() | SDMMC_STA_CTIMEOUT();

	if(type == SD_NO_RESP) {
		status_flags |= SDMMC_STA_CMDSENT();
	} else {
		status_flags |= SDMMC_STA_CMDREND();
	}

	while(!(SDMMC->STA & status_flags)) { }

	/* Grab the response. */
	if(type == SD_SHORT_RESP) {
		*resp = SDMMC->RESP1;
	} else if (type == SD_LONG_RESP) {
		resp[3] = SDMMC->RESP1;
		resp[2] = SDMMC->RESP2;
		resp[1] = SDMMC->RESP3;
		resp[0] = SDMMC->RESP4;
	}

	const uint32_t status = SDMMC->STA;
	clear_all_flags();

	/**
	 * ACMD41's R3 response does not contain a cmd index or CRC. This will
	 * always cause a CRC failure and INCORRECT_RESPCMD so just ignore it.
	 */
	const bool is_acmd41 = (cmd_index == SD_ACMD41_SEND_OP_COND);

	if(GET_SDMMC_STA_CCRCFAIL(status) && !is_acmd41) {
		return SD_STATUS_CCRCFAIL;
	} else if (GET_SDMMC_STA_CTIMEOUT(status)) {
		return SD_STATUS_CTIMEOUT;
	}

	if((type != SD_NO_RESP) && !is_acmd41 &&
	   (GET_SDMMC_RESPCMD_RESPCMD(SDMMC->RESPCMD) != cmd_index)) {
		dbprintf("The RESPCMD: %lu\n", SDMMC->RESPCMD);
		return SD_INCORRECT_RESPCMD;
	}

	return SD_SUCCESS;
}


#endif /* INCLUDE_SDMMC_DRIVER */
