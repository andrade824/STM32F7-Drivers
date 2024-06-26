/**
 * @author Devon Andrade
 * @created 6/16/2018
 *
 * Definitions and functions used to manipulate the FMC SDRAM module.
 */
#include "config.h"
#include "debug.h"
#include "fmc_sdram.h"
#include "gpio.h"
#include "system.h"
#include "system_timer.h"

#include "registers/fmc_sdram_reg.h"
#include "registers/rcc_reg.h"

#if ENABLE_SDRAM

/* Maximum amount of time to wait for an SDRAM command to complete. */
#define SDRAM_TIMEOUT MSECS(1)

/**
 * The GPIO alternate function for the FMC SDRAM pins. Check section 3 of the
 * datasheet for more details.
 */
#define SDRAM_ALT_FUNC AF12

/**
 * Initialize the SDRAM Controller.
 */
void fmc_sdram_init(void)
{
	/* Request all of the required GPIOs. */
	gpio_request_alt(GPIO_FMC_A0, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A1, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A2, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A3, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A4, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A5, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A6, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A7, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A8, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A9, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A10, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_A11, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);

	gpio_request_alt(GPIO_FMC_BA0, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_BA1, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);

	gpio_request_alt(GPIO_FMC_NBL0, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_NBL1, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);

	gpio_request_alt(GPIO_FMC_D0, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D1, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D2, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D3, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D4, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D5, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D6, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D7, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D8, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D9, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D10, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D11, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D12, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D13, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D14, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_D15, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);

	gpio_request_alt(GPIO_FMC_SDNWE, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_SDNCAS, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_SDNRAS, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_SDNE0, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_SDCKE0, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);
	gpio_request_alt(GPIO_FMC_SDCLK, SDRAM_ALT_FUNC, GPIO_OSPEED_100MHZ);

	/* Enable the FMC clock. */
	SET_FIELD(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN());
	DSB();

	/* Configure the SDRAM bank (memory features and timing). */
	SDRAM->SDCR[SDRAM_FMC_BANK] = SET_SDRAM_SDCR_NC(SDCR_NC) |
	                              SET_SDRAM_SDCR_NR(SDCR_NR) |
	                              SET_SDRAM_SDCR_MWID(SDCR_MWID) |
	                              SET_SDRAM_SDCR_NB(SDCR_NB) |
	                              SET_SDRAM_SDCR_CAS(SDCR_CAS) |
	                              SET_SDRAM_SDCR_SDCLK(SDCR_SDCLK) |
	                              SDRAM_SDCR_RBURST();

	SDRAM->SDTR[SDRAM_FMC_BANK] = SET_SDRAM_SDTR_TMRD(SDTR_TMRD) |
	                              SET_SDRAM_SDTR_TXSR(SDTR_TXSR) |
	                              SET_SDRAM_SDTR_TRAS(SDTR_TRAS) |
	                              SET_SDRAM_SDTR_TRC(SDTR_TRC) |
	                              SET_SDRAM_SDTR_TWR(SDTR_TWR) |
	                              SET_SDRAM_SDTR_TRP(SDTR_TRP) |
	                              SET_SDRAM_SDTR_TRCD(SDTR_TRCD);

	/* Send the clock enable command. */
	SET_FIELD(SDRAM->SDCMR, SET_SDRAM_SDCMR_MODE(SDRAM_SDCMR_CLOCK_CFG) |
	                        SET_SDRAM_SDCMR_CTB2(SDCMR_CTB2) |
	                        SET_SDRAM_SDCMR_CTB1(SDCMR_CTB1));
	ABORT_TIMEOUT(GET_SDRAM_SDSR_BUSY(SDRAM->SDSR) == SDSR_READY, SDRAM_TIMEOUT);

	/* Wait for the SDRAM to power up (usually 100us). */
	sleep(SDRAM_INIT_DELAY);

	/* Send the Precharge-All command. */
	SET_FIELD(SDRAM->SDCMR, SET_SDRAM_SDCMR_MODE(SDRAM_SDCMR_PALL) |
	                        SET_SDRAM_SDCMR_CTB2(SDCMR_CTB2) |
	                        SET_SDRAM_SDCMR_CTB1(SDCMR_CTB1));
	ABORT_TIMEOUT(GET_SDRAM_SDSR_BUSY(SDRAM->SDSR) == SDSR_READY, SDRAM_TIMEOUT);

	/* Send the Auto-Refresh command. */
	SET_FIELD(SDRAM->SDCMR, SET_SDRAM_SDCMR_MODE(SDRAM_SDCMR_AUTO_REFRESH) |
	                        SET_SDRAM_SDCMR_CTB2(SDCMR_CTB2) |
	                        SET_SDRAM_SDCMR_CTB1(SDCMR_CTB1) |
	                        SET_SDRAM_SDCMR_NRFS(SDCMR_NRFS));
	ABORT_TIMEOUT(GET_SDRAM_SDSR_BUSY(SDRAM->SDSR) == SDSR_READY, SDRAM_TIMEOUT);

	/* Send the Load Mode Register command. */
	SET_FIELD(SDRAM->SDCMR, SET_SDRAM_SDCMR_MODE(SDRAM_SDCMR_LOAD_MODE_REG) |
	                        SET_SDRAM_SDCMR_CTB2(SDCMR_CTB2) |
	                        SET_SDRAM_SDCMR_CTB1(SDCMR_CTB1) |
	                        SET_SDRAM_SDCMR_MRD(SDCMR_MRD));
	ABORT_TIMEOUT(GET_SDRAM_SDSR_BUSY(SDRAM->SDSR) == SDSR_READY, SDRAM_TIMEOUT);

	/* Set the refresh rate. */
	SET_FIELD(SDRAM->SDCMR, SET_SDRAM_SDRTR_COUNT(SDRTR_COUNT));

	/* Ensure that write protection is disabled. */
	CLEAR_FIELD(SDRAM->SDCR[SDRAM_FMC_BANK], SDRAM_SDCR_WP());
}

#endif /* ENABLE_SDRAM */
