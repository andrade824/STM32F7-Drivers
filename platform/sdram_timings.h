/**
 * @author Devon Andrade
 * @created 6/13/2018
 *
 * Timings and settings used for the SDRAM chip (MT48LC4M32B2) on the STM32F7
 * Discovery board.
 */
#ifdef INCLUDE_SDRAM_DRIVER
#ifndef SDRAM_TIMINGS_H
#define SDRAM_TIMINGS_H

/**
 *  The SDRAM chip on the STM32F7 Discovery board is connected to the first
 *  SDRAM bank (the FMC's bank 5).
 */
#define SDRAM_FMC_BANK 0

/* SDRAM Control Register settings */
#define SDCR_SDCLK  0x2 /* The SDRAM clock is half of the CPU clock (108MHz) */
#define SDCR_CAS    0x3 /* Three clock cycle latency */
#define SDCR_NB     0x1 /* Four internal memory banks */
#define SDCR_MWID   0x1 /* 16-bit memory width */
#define SDCR_NR     0x1 /* 12-bit Row Address */
#define SDCR_NC     0x3 /* 11-bit Column Address */

/**
 * These SDRAM timings were calculated using a CPU clock of 216MHz and a memory
 * clock of 108MHz.
 *
 * The number of cycles each parameter uses is one more than its value.
 *
 * For Example: SDTR_TRCD is "0x1" or 2 cycles.
 */
#define SDTR_TRCD   0x1 /* Row to column delay */
#define SDTR_TRP    0x1 /* Row precharge delay */
#define SDTR_TWR    0x1 /* Write recovery delay */
#define SDTR_TRC    0x6 /* Row cycle delay */
#define SDTR_TRAS   0x4 /* Self refresh time */
#define SDTR_TXSR   0x7 /* Exit Self-refresh delay */
#define SDTR_TMRD   0x1 /* Load Mode Register to Active */

/**
 * Refresh Timer Register [13.7.5]
 *
 * The datasheet is a little confusing as to how to compute this value. By
 * reverse engineering their example, this is the equation I've come up with:
 *
 * SDRTR = (((Refresh Rate) / (Number of Rows)) * (SDRAM Clock Frequency)) - 20
 *
 * The refresh rate must be increased by 20 SDRAM clock cycles to obtain a safe
 * margin if an internal refresh occurs when a read request has been accepted.
 *
 * For this device:
 * SDRTR = ((64ms / 4096 rows) * 108MHz) - 20 = TRUNCATE(1667.5) = 1667
 */
#define SDRTR_COUNT 1667

#endif
#endif
