/**
 * @author Devon Andrade
 * @created 3/3/2019
 *
 * Definitions and functions used to manipulate the SDMMC.
 */
#include "config.h"
#include "debug.h"
#include "sdmmc.h"
#include "system.h"
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
	SD_CMD2_ALL_SEND_CID          =  2,
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
} SdCmd;

/* SD Card Application-Specific Commands */
typedef enum {
	SD_ACMD6_SET_BUS_WIDTH           =  6,
	SD_ACMD13_SD_STATUS              = 13,
	SD_ACMD22_SEND_NUM_WR_BLOCKS     = 22,
	SD_ACMD23_SET_WR_BLK_ERASE_COUNT = 23,
	SD_ACMD41_SEND_OP_COND           = 41
} SdAppCmd;

/* Fields within an R1 response */
#define R1_ALL_ERRORS           0xFDF98008U
#define R1_ADDRESS_OUT_OF_RANGE 0x80000000U
#define R1_ADDRESS_MISALIGN     0x40000000U
#define R1_BLOCK_LEN_ERROR      0x20000000U
#define R1_ERASE_SEQ_ERROR      0x10000000U
#define R1_ERASE_PARAM          0x08000000U
#define R1_WP_VIOLATION         0x04000000U
#define R1_LOCK_UNLOCK_FAILED   0x01000000U
#define R1_COM_CRC_ERROR        0x00800000U
#define R1_ILLEGAL_COMMAND      0x00400000U
#define R1_CARD_ECC_FAILED      0x00200000U
#define R1_CC_ERROR             0x00100000U
#define R1_GENERIC_ERROR        0x00080000U
#define R1_CID_CSD_OVERWRITE    0x00010000U
#define R1_WP_ERASE_SKIP        0x00008000U
#define R1_READY_FOR_DATA       0x00000100U
#define R1_AKE_SEQ_ERROR        0x00000008U

/**
 * SDMMC Data timeout value.
 *
 * How long for the SD data path to wait for data to be received from the card
 * (in card bus clock cycles).
 */
#define SDMMC_DATA_TIMEOUT 0xFFFFU

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

/* ACMD6 argument to set the bus width to 4-bits. */
#define ACMD6_4BIT_WIDTH 2U

/* Properties of the connected SD Card. */
static SdCard card;

/**
 * Return a read-only copy of the SD card info structure.
 */
SdCard sd_get_card_info(void)
{
	return card;
}

/**
 * Clear all status flags that may have been set.
 */
static void clear_all_flags(void)
{
	SDMMC->ICR = SDMMC_ICR_CCRCFAILC() | SDMMC_ICR_DCRCFAILC() | SDMMC_ICR_CTIMEOUTC() |
	             SDMMC_ICR_DTIMEOUTC() | SDMMC_ICR_TXUNDERRC() | SDMMC_ICR_RXOVERRC() |
	             SDMMC_ICR_CMDRENDC() | SDMMC_ICR_CMDSENTC() | SDMMC_ICR_DATAENDC() |
	             SDMMC_ICR_DBCKENDC() | SDMMC_ICR_SDIOITC();
}

/**
 * Send a command to the SD card.
 *
 * @param cmd_index The index of the command to send.
 * @param arg The 32-bit argument to the command.
 * @param type The type of the response (none, short, or long).
 * @param resp The returned response from the SD Card.
 *
 * @note If there is no expected response, set "resp" to NULL. A "short" response
 * must be a uint32_t. A "long" response must be an array of four uint32_t's.
 */
static SdStatus send_cmd(
	uint8_t cmd_index,
	uint32_t arg,
	SdResp type,
	uint32_t *resp)
{
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

	if((type == SD_SHORT_RESP) && !is_acmd41 &&
	   (GET_SDMMC_RESPCMD_RESPCMD(SDMMC->RESPCMD) != cmd_index)) {
		dbprintf("[SDMMC] Received invalid RESPCMD: %u %lu\n", cmd_index, SDMMC->RESPCMD);
		return SD_INCORRECT_RESPCMD;
	}

	return SD_SUCCESS;
}

/**
 * Check the error bits in an R1 response and return an error if any of them
 * are set.
 */
static SdStatus check_r1_resp(uint32_t resp)
{
	if((resp & R1_ALL_ERRORS) == 0)         { return SD_SUCCESS; }
	else if(resp & R1_ADDRESS_OUT_OF_RANGE) { return SD_ADDRESS_OUT_OF_RANGE; }
	else if(resp & R1_ADDRESS_MISALIGN)     { return SD_ADDRESS_MISALIGN; }
	else if(resp & R1_BLOCK_LEN_ERROR)      { return SD_BLOCK_LEN_ERROR; }
	else if(resp & R1_ERASE_SEQ_ERROR)      { return SD_ERASE_SEQ_ERROR; }
	else if(resp & R1_ERASE_PARAM)          { return SD_ERASE_PARAM; }
	else if(resp & R1_WP_VIOLATION)         { return SD_WP_VIOLATION; }
	else if(resp & R1_LOCK_UNLOCK_FAILED)   { return SD_LOCK_UNLOCK_FAILED; }
	else if(resp & R1_COM_CRC_ERROR)        { return SD_COM_CRC_ERROR; }
	else if(resp & R1_ILLEGAL_COMMAND)      { return SD_ILLEGAL_COMMAND; }
	else if(resp & R1_CARD_ECC_FAILED)      { return SD_CARD_ECC_FAILED; }
	else if(resp & R1_CC_ERROR)             { return SD_CC_ERROR; }
	else if(resp & R1_GENERIC_ERROR)        { return SD_GENERIC_ERROR; }
	else if(resp & R1_CID_CSD_OVERWRITE)    { return SD_CID_CSD_OVERWRITE; }
	else if(resp & R1_WP_ERASE_SKIP)        { return SD_WP_ERASE_SKIP; }
	else if(resp & R1_AKE_SEQ_ERROR)        { return SD_AKE_SEQ_ERROR; }

	ABORT("[SDMMC] Reached end of R1 response checking which should be impossible... 0x%lx\n", resp);

	return SD_FAIL;
}

/**
 * CMD8 is sent as the second command in the inititialization sequence to
 * determine whether a card supports the v1 or v2 protocols. If there's no
 * response, then version 1 is assumed.
 */
static SdStatus send_cmd8_send_if_cond(void)
{
	SdStatus status = SD_SUCCESS;
	uint32_t resp = 0;

	ASSERT(card.state == SD_IDENT_STATE);
	card.state = SD_DETERMINE_VERSION_STATE;

	status = send_cmd(SD_CMD8_SEND_IF_COND, CMD8_ARG, SD_SHORT_RESP, &resp);
	if(status != SD_SUCCESS) {
		dbprintf("[SDMMC] No SD Card inserted\n");
		return SD_FAIL;
	} else {
		/* Verify the "check pattern" matches what was sent. */
		if(resp != CMD8_ARG) {
			ABORT("[SDMMC] Response from CMD8 didn't match sent check pattern.\n");
		}
	}

	return SD_SUCCESS;
}

/**
 * Send ACMD41 in a loop as required by the SD protocol. This command needs to
 * be sent until the SD Card is finished powering up (as indicated by a status
 * bit in the response).
 */
static SdStatus send_acmd41_send_op_cond(void)
{
	SdStatus status = SD_SUCCESS;
	uint32_t resp = 0;

	ASSERT(card.state == SD_DETERMINE_VERSION_STATE);
	card.state = SD_WAIT_POWER_UP_STATE;

	/**
	 * The argument to ACMD41 indicates the wanted voltage window and whether
	 * the card is expected to be high capacity (based on its version).
	 */
	const uint32_t acmd41_arg = ACMD41_VOLTAGE | ACMD41_HIGH_CAPACITY;

	uint32_t acmd41_tries = 0;
	while(acmd41_tries < ACMD41_MAX_TRIES) {
		/* Send APP_CMD, default RCA is 0. */
		if(send_cmd(SD_CMD55_APP_CMD, 0, SD_SHORT_RESP, &resp) != SD_SUCCESS) {
			return SD_FAIL;
		}

		if(check_r1_resp(resp) != SD_SUCCESS) {
			return SD_FAIL;
		}

		/* Send ACMD41 */
		status = send_cmd(SD_ACMD41_SEND_OP_COND, acmd41_arg, SD_SHORT_RESP, &resp);
		if(status != SD_SUCCESS) {
			return SD_FAIL;
		}

		if(resp & ACMD41_BUSY_BIT) {
			break;
		}

		acmd41_tries++;
	}

	if(acmd41_tries == ACMD41_MAX_TRIES) {
		dbprintf("[SDMMC] Timed out waiting for ACMD41 to set power up status bit.\n");
		return SD_FAIL;
	}

	if(resp & ACMD41_HIGH_CAPACITY) {
		dbprintf("[SDMMC] Detected SDHC/SDXC card.\n");
	} else {
		dbprintf("[SDMMC] Detected non-high-capacity card. Only SDHC/SDXC cards are supported.\n");
		return SD_FAIL;
	}

	return SD_SUCCESS;
}

/**
 * Retrieve the Card Identification register and parse it.
 */
static SdStatus send_cmd2_all_send_cid(void)
{
	uint32_t resp[4] = { 0 };

	ASSERT(card.state == SD_WAIT_POWER_UP_STATE);
	card.state = SD_GET_CID_STATE;

	if(send_cmd(SD_CMD2_ALL_SEND_CID, 0, SD_LONG_RESP, resp) != SD_SUCCESS) {
		return SD_FAIL;
	}

	/* Parse the CID register fields. */
	card.manufacturer_id = (resp[3] & 0xFF000000) >> 24;
	card.oem_id[0] = (resp[3] & 0x00FF0000) >> 16;
	card.oem_id[1] = (resp[3] & 0x0000FF00) >> 8;
	card.oem_id[2] = '\0';
	card.product_name[0] = resp[3] & 0x000000FF;
	card.product_name[1] = (resp[2] & 0xFF000000) >> 24;
	card.product_name[2] = (resp[2] & 0x00FF0000) >> 16;
	card.product_name[3] = (resp[2] & 0x0000FF00) >> 8;
	card.product_name[4] = resp[2] & 0x000000FF;
	card.product_name[5] = '\0';
	card.product_rev = (resp[1] & 0xFF000000) >> 24;
	card.serial_num = ((resp[1] & 0x00FFFFFF) << 8) | ((resp[0] & 0xFF000000) >> 24);
	card.manufacturing_date = (resp[0] & 0x000FFF00) >> 8;

	return SD_SUCCESS;
}

/**
 * Dump Card Identification information for diagnostic purposes.
 */
static void dump_cid(void)
{
	dbprintf("[SDMMC] MID: 0x%x, OID: %s, PNM: %s, PRV: 0x%x, PSN: 0x%lx, MDT: 0x%x\n",
	         card.manufacturer_id, card.oem_id, card.product_name, card.product_rev,
	         card.serial_num, card.manufacturing_date);
}

/**
 * Retrieve the Relative Card Address (RCA).
 */
static SdStatus send_cmd3_send_relative_addr(void)
{
	uint32_t resp = 0;

	ASSERT(card.state == SD_GET_CID_STATE);
	card.state = SD_GET_RCA_STATE;

	if(send_cmd(SD_CMD3_SEND_RELATIVE_ADDR, 0, SD_SHORT_RESP, &resp) != SD_SUCCESS) {
		return SD_FAIL;
	}

	card.rca = resp & 0xFFFF0000;

	return SD_SUCCESS;
}

/**
 * Retrieve the Card-Specific-Data register and parse it.
 */
static SdStatus send_cmd9_send_csd(void)
{
	uint32_t resp[4] = { 0 };

	ASSERT(card.state == SD_GET_RCA_STATE);
	card.state = SD_GET_CSD_STATE;

	if(send_cmd(SD_CMD9_SEND_CSD, card.rca, SD_LONG_RESP, resp) != SD_SUCCESS) {
		return SD_FAIL;
	}

	/* Parse the CSD register fields. */
	uint32_t c_size = ((resp[2] & 0x3F) << 16) | ((resp[1] & 0xFFFF0000) >> 16);
	card.total_blocks = (c_size + 1UL) << 10UL;
	card.total_size = (c_size + 1ULL) << 19ULL;
	card.block_len = 1 << ((resp[2] & 0xF0000) >> 16);

	return SD_SUCCESS;
}

/**
 * Move an SD Card into the Transfer state.
 */
static SdStatus send_cmd7_select_card(void)
{
	uint32_t resp = 0;

	ASSERT(card.state == SD_GET_CSD_STATE);
	card.state = SD_TRANSFER_STATE;

	if(send_cmd(SD_CMD7_SELECT_CARD, card.rca, SD_SHORT_RESP, &resp) != SD_SUCCESS) {
		return SD_FAIL;
	}

	if(check_r1_resp(resp) != SD_SUCCESS) {
		return SD_FAIL;
	}

	return SD_SUCCESS;
}

/**
 * Set the bus width to 4-bits wide.
 */
static SdStatus send_acmd6_set_bus_width(void)
{
	SdStatus status = SD_SUCCESS;
	uint32_t resp = 0;

	ASSERT(card.state == SD_TRANSFER_STATE);

	/* Send APP_CMD */
	if(send_cmd(SD_CMD55_APP_CMD, card.rca, SD_SHORT_RESP, &resp) != SD_SUCCESS) {
		return SD_FAIL;
	}

	if(check_r1_resp(resp) != SD_SUCCESS) {
		return SD_FAIL;
	}

	/* Send ACMD6 */
	status = send_cmd(SD_ACMD6_SET_BUS_WIDTH, ACMD6_4BIT_WIDTH, SD_SHORT_RESP, &resp);
	if(status != SD_SUCCESS) {
		return SD_FAIL;
	}

	if(check_r1_resp(resp) != SD_SUCCESS) {
		return SD_FAIL;
	}

	/* Tell SDMMC controller hardware to expect 4-bit data. */
	SET_FIELD(SDMMC->CLKCR, SET_SDMMC_CLKCR_WIDBUS(SD_4_BIT));

	return SD_SUCCESS;
}

/**
 * Get the card status (R1 response).
 *
 * @param card_status A 32-bit integer to hold the returned card status.
 */
static SdStatus send_cmd13_send_status(uint32_t *card_status)
{
	SdStatus status = send_cmd(SD_CMD13_SEND_STATUS, card.rca, SD_SHORT_RESP, card_status);
	if(status != SD_SUCCESS) {
		return status;
	}

	return SD_SUCCESS;
}

/**
 * Wait for the card to become ready to receive/send more data.
 */
static SdStatus wait_for_card_ready(void)
{
	SdStatus status = SD_SUCCESS;
	uint32_t card_status = 0;

	do {
		status = send_cmd13_send_status(&card_status);
		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] Failed to send CMD13_SEND_STATUS %d\n", status);
			return status;
		}
	} while(!(card_status & R1_READY_FOR_DATA));

	return SD_SUCCESS;
}

/**
 * Attempt to initialize a connected SD Card. If there's an error while
 * communicating with the card then Failure is returned.
 *
 * @note Before usage, the SDMMC GPIOs and 48MHz clock will need to be setup.
 *
 * @return SD_FAIL if an SD card isn't present, SD_SUCCESS otherwise.
 */
SdStatus sdmmc_init() {
	SdStatus status = 0;

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
	DSB();

	/* Enable the 400KHz SD clock. */
	SET_FIELD(SDMMC->POWER, SET_SDMMC_POWER_PWRCTL(SD_POWER_ON));
	SET_FIELD(SDMMC->CLKCR, SET_SDMMC_CLKCR_CLKDIV(SD_CLKDIV) | SDMMC_CLKCR_CLKEN());

	sleep(SD_POWER_ON_DELAY);

	/* Place card into IDLE state. */
	card.state = SD_IDENT_STATE;
	status = send_cmd(SD_CMD0_GO_IDLE_STATE, 0, SD_NO_RESP, NULL);
	if(status != SD_SUCCESS) {
		ABORT("[SDMMC] Failed to send CMD0 %u\n", status);
	}

	/**
	 * Send CMD8 to determine if a v2 card is present.
	 *
	 * The init will fail here instead of aborting to allow higher layers to
	 * handle the case when an SD card doesn't exist.
	 */
	if(send_cmd8_send_if_cond() != SD_SUCCESS) {
		return SD_FAIL;
	}

	/* Send ACMD41 to verify voltage and wait for the card to power up. */
	if(send_acmd41_send_op_cond() != SD_SUCCESS) {
		ABORT("[SDMMC] Error while waiting for the SD card to power up.\n");
	}

	/* Send CMD2 to retrieve the Card Identification Register. */
	if(send_cmd2_all_send_cid() != SD_SUCCESS) {
		ABORT("[SDMMC] Error retrieving the CID.\n");
	}

	dump_cid();

	/* Send CMD3 to retrieve the card's relative address. */
	if(send_cmd3_send_relative_addr() != SD_SUCCESS) {
		ABORT("[SDMMC] Error retrieving the RCA.\n");
	}

	/* Bump up the SDMMC clock to 48MHz now that the card is in Data Transfer Mode. */
	SET_FIELD(SDMMC->CLKCR, SDMMC_CLKCR_BYPASS());

	/* Send CMD9 to retrieve the card specific data (e.g., card capacity). */
	if(send_cmd9_send_csd() != SD_SUCCESS) {
		ABORT("[SDMMC] Error retrieving the CSD.\n");
	}

	/* Send CMD7 to move the card into the transfer state */
	if(send_cmd7_select_card() != SD_SUCCESS) {
		ABORT("[SDMMC] CMD7 failed to select SD Card.\n");
	}

	/* Set the bus width to 4-bits wide. */
	if(send_acmd6_set_bus_width() != SD_SUCCESS) {
		ABORT("[SDMMC] ACMD6 failed to set bus width to 4-bits.\n");
	}

	return SD_SUCCESS;
}

/**
 * Read one or more blocks of data from an SD card.
 *
 * @param data A buffer to read data into (must be num_blocks * 512 bytes in size).
 * @param block_addr Start address of the block to read.
 * @param num_blocks The number of blocks to read.
 *
 * @return SD_SUCCESS if data was read correctly, otherwise an error value.
 */
SdStatus sd_read_data(void *data, uint32_t block_addr, uint16_t num_blocks)
{
	uint32_t resp = 0;
	SdStatus status = SD_SUCCESS;
	uint32_t *buffer = (uint32_t*)data;

	ASSERT(card.state == SD_TRANSFER_STATE);
	ASSERT((block_addr + num_blocks) < card.total_blocks); /* Assert address isn't out of range */
	ASSERT(num_blocks <= 512); /* Maximum number of blocks that can be sent in one go */

	/* Wait for the card to become ready to send data. */
	status = wait_for_card_ready();
	if(status != SD_SUCCESS) {
		return status;
	}

	card.state = SD_READ_STATE;

	/* Set up data path state machine to wait for data. */
	SDMMC->DTIMER = SDMMC_DATA_TIMEOUT;
	SDMMC->DLEN = num_blocks * card.block_len;
	SDMMC->DCTRL = SET_SDMMC_DCTRL_DBLOCKSIZE(SD_512_BYTES) |
	               SET_SDMMC_DCTRL_DMAEN(SD_DMA_DISABLED) |
	               SET_SDMMC_DCTRL_DTMODE(SD_BLOCK_TRANSFER) |
	               SET_SDMMC_DCTRL_DTDIR(SD_FROM_CARD) |
	               SET_SDMMC_DCTRL_DTEN(1);

	/* Send the Read Block(s) command. */
	if(num_blocks == 1) {
		status = send_cmd(SD_CMD17_READ_SINGLE_BLOCK, block_addr, SD_SHORT_RESP, &resp);
		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] Failed to send CMD17_READ_SINGLE_BLOCK %d\n", status);
			return status;
		}
	} else {
		status = send_cmd(SD_CMD18_READ_MULTIPLE_BLOCK, block_addr, SD_SHORT_RESP, &resp);
		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] Failed to send CMD18_READ_MULTIPLE_BLOCK %d\n", status);
			return status;
		}
	}

	status = check_r1_resp(resp);
	if(status != SD_SUCCESS) {
		dbprintf("[SDMMC] R1 response from CMD17/CMD18 (Read Blocks) contains errors: %d\n", status);
		return status;
	}

	/* Read the data coming from the card. */
	const uint32_t flags_mask = SDMMC_STA_RXOVERR() | SDMMC_STA_DCRCFAIL() |
	                            SDMMC_STA_DTIMEOUT() | SDMMC_STA_DATAEND();
	while(!(SDMMC->STA & flags_mask)) {
		if(SDMMC->STA & SDMMC_STA_RXFIFOHF()) {
			for(int i = 0; i < 8; ++i) {
				*buffer++ = SDMMC->FIFO;
			}
		}
	}

	const uint32_t flags = SDMMC->STA;
	clear_all_flags();

	/* Send STOP command for multi-block transfers. */
	if((GET_SDMMC_STA_DATAEND(flags)) && (num_blocks > 1)) {
		status = send_cmd(SD_CMD12_STOP_TRANSMISSION, 0, SD_SHORT_RESP, &resp);
		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] Failed to send CMD12_STOP_TRANSMISSION %d\n", status);
			return status;
		}

		status = check_r1_resp(resp);
		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] R1 response from CMD12 (Stop Transmission) contains errors: %d\n", status);
			return status;
		}
	}

	/* Handle any data transfer errors. */
	if(GET_SDMMC_STA_RXOVERR(flags)) { return SD_STATUS_RXOVERR; }
	else if(GET_SDMMC_STA_DCRCFAIL(flags)) { return SD_STATUS_DCRCFAIL; }
	else if(GET_SDMMC_STA_DTIMEOUT(flags)) { return SD_STATUS_DTIMEOUT; }

	/* Finish reading any leftover bytes in the FIFO. */
	while(SDMMC->STA & SDMMC_STA_RXDAVL()) {
		*buffer++ = SDMMC->FIFO;
	}

	card.state = SD_TRANSFER_STATE;

	return SD_SUCCESS;
}

/**
 * Write one or more blocks of data to an SD card.
 *
 * @param data A buffer of data to write (must be num_blocks * 512 bytes in size).
 * @param block_addr Start address of the block to write.
 * @param num_blocks The number of blocks to write.
 *
 * @return SD_SUCCESS if data was written correctly, otherwise an error value.
 */
SdStatus sd_write_data(void *data, uint32_t block_addr, uint16_t num_blocks)
{
	uint32_t resp = 0;
	SdStatus status = SD_SUCCESS;
	uint32_t *buffer = (uint32_t*)data;

	ASSERT(card.state == SD_TRANSFER_STATE);
	ASSERT((block_addr + num_blocks) < card.total_blocks); /* Assert address isn't out of range */
	ASSERT(num_blocks <= 512); /* Maximum number of blocks that can be sent in one go */

	/* Wait for the card to become ready to receive data. */
	status = wait_for_card_ready();
	if(status != SD_SUCCESS) {
		return status;
	}

	card.state = SD_WRITE_STATE;

	/* Send the Write Block(s) command. */
	if(num_blocks == 1) {
		status = send_cmd(SD_CMD24_WRITE_BLOCK, block_addr, SD_SHORT_RESP, &resp);
		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] Failed to send CMD24_WRITE_BLOCK %d\n", status);
			return status;
		}
	} else {
		status = send_cmd(SD_CMD25_WRITE_MULTIPLE_BLOCK, block_addr, SD_SHORT_RESP, &resp);
		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] Failed to send CMD25_WRITE_MULTIPLE_BLOCK %d\n", status);
			return status;
		}
	}

	status = check_r1_resp(resp);
	if(status != SD_SUCCESS) {
		dbprintf("[SDMMC] R1 response from CMD24/CMD25 (Write Blocks) contains errors: %d\n", status);
		return status;
	}

	/* Set up data path state machine to wait for data to send. */
	SDMMC->DTIMER = SDMMC_DATA_TIMEOUT;
	SDMMC->DLEN = num_blocks * card.block_len;
	SDMMC->DCTRL = SET_SDMMC_DCTRL_DBLOCKSIZE(SD_512_BYTES) |
	               SET_SDMMC_DCTRL_DMAEN(SD_DMA_DISABLED) |
	               SET_SDMMC_DCTRL_DTMODE(SD_BLOCK_TRANSFER) |
	               SET_SDMMC_DCTRL_DTDIR(SD_TO_CARD) |
	               SET_SDMMC_DCTRL_DTEN(1);

	/* Write the data to the SD Card. */
	const uint32_t flags_mask = SDMMC_STA_TXUNDERR() | SDMMC_STA_DCRCFAIL() |
	                            SDMMC_STA_DTIMEOUT() | SDMMC_STA_DATAEND();
	const uint32_t tx_length = num_blocks * (card.block_len / 4); /* Number of words to send */
	uint32_t word_count = 0;
	while(!(SDMMC->STA & flags_mask)) {
		if(SDMMC->STA & SDMMC_STA_TXFIFOHE()) {
			for(int i = 0; (i < 8) && (word_count < tx_length); ++i, ++word_count) {
				SDMMC->FIFO = *buffer++;
			}
		}
	}

	const uint32_t flags = SDMMC->STA;
	clear_all_flags();

	/* Send STOP command for multi-block transfers. */
	if((GET_SDMMC_STA_DATAEND(flags)) && (num_blocks > 1)) {
		status = send_cmd(SD_CMD12_STOP_TRANSMISSION, 0, SD_SHORT_RESP, &resp);
		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] Failed to send CMD12_STOP_TRANSMISSION %d\n", status);
			return status;
		}

		status = check_r1_resp(resp);
		if(status != SD_SUCCESS) {
			dbprintf("[SDMMC] R1 response from CMD12 (Stop Transmission) contains errors: %d\n", status);
			return status;
		}
	}

	/* Handle any data transfer errors. */
	if(GET_SDMMC_STA_TXUNDERR(flags)) { return SD_STATUS_TXUNDERR; }
	else if(GET_SDMMC_STA_DCRCFAIL(flags)) { return SD_STATUS_DCRCFAIL; }
	else if(GET_SDMMC_STA_DTIMEOUT(flags)) { return SD_STATUS_DTIMEOUT; }

	card.state = SD_TRANSFER_STATE;

	return SD_SUCCESS;
}
