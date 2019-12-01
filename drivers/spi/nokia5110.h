/**
 * @author Devon Andrade
 * @created 11/28/2019 (Happy Thanksgiving!)
 *
 * Driver to control a Nokia 5110 display (using SPI).
 *
 * https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf
 */
#pragma once

#include "gpio.h"
#include "spi.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * The nokia display is 84 pixels wide by 48 pixels tall but it has a strange
 * memory structure which affects how it gets programmed. The graphics RAM is
 * broken up into six banks (or rows) of memory, where each bank consists of
 * 84, 8-pixel tall columns.
 *
 * Programming the display involves setting the x-address (the column) and the
 * y-address (the memory bank) then sending a byte of data representing a column
 * of pixels in that bank.
 */

/* Number of bytes needed to represent a Nokia 5110 framebuffer. */
#define NOKIA_FRAMEBUFFER_SIZE 504U

/* Width/Height of the Nokia display in pixels. */
#define NOKIA_WIDTH_PIXELS  84U
#define NOKIA_HEIGHT_PIXELS 48U

/* Height of the Nokia display in memory banks. */
#define NOKIA_HEIGHT_BANKS 6U

/* Different available display modes. */
typedef enum {
	NOKIA_DISP_BLANK   = 0,
	NOKIA_DISP_ALL_ON  = 1,
	NOKIA_DISP_NORMAL  = 2,
	NOKIA_DISP_INVERSE = 3
} Nokia5110DispMode;

/* Structure used to represent a single Nokia 5110 display. */
typedef struct {
	/* SPI module the display is connected to. */
	SpiReg *spi;

	/* True to use hardware-managed slave select, false otherwise. */
	bool use_hardware_ss;

	/* If using software-managed slave select, this is the GPIO slave select pin. */
	GpioReg *ss_reg;
	GpioPin ss_pin;

	/* GPIO used to control the Data/Command (DC) line. */
	GpioReg *dc_reg;
	GpioPin dc_pin;
} Nokia5110Inst;

void nokia_init(
	Nokia5110Inst *inst,
	SpiReg *spi,
	GpioReg *dc_reg,
	GpioPin dc_pin,
	GpioReg *rst_reg,
	GpioPin rst_pin,
	bool use_hardware_ss);
void nokia_set_ss_pin(Nokia5110Inst *inst, GpioReg *ss_reg, GpioPin ss_pin);
void nokia_reinit_spi(Nokia5110Inst *inst);

void nokia_set_params(Nokia5110Inst *inst, uint8_t vop, uint8_t tc, uint8_t bs);
void nokia_set_disp_mode(Nokia5110Inst *inst, Nokia5110DispMode mode);

void nokia_clear_screen(Nokia5110Inst *inst);

void nokia_set_position(Nokia5110Inst *inst, uint8_t column, uint8_t bank);
void nokia_set_column(Nokia5110Inst *inst, uint8_t data);
