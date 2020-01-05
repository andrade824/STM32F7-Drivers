/**
 * @author Devon Andrade
 * @created 11/28/2019 (Happy Thanksgiving!)
 *
 * Driver to control a Nokia 5110 display (using SPI).
 *
 * https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf
 */
#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "spi.h"
#include "spi/nokia5110.h"
#include "system_timer.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Function Set.
 *
 * Can be called when H=0 or H=1 (because this command sets H).
 *
 * @param PD 0 for chip active, 1 for chip in power-down mode.
 * @param V 0 for horizontal addressing, 1 for vertical addressing.
 * @param H 0 to use basic instruction set, 1 to use extended instruction set.
 */
#define NOKIA_CMD_FUNC_SET(PD, V, H) (0x20 | ((H) & 1) | (((V) & 1) << 1) | ((PD) & 1) << 2)

/**
 * Display Control.
 *
 * Can be called with H=0.
 *
 * @param mode These two bits together form a lookup table for the display modes:
 *             00b: Display blank
 *             10b: Normal mode
 *             01b: All display segments on
 *             11b: Inverse video mode
 */
#define NOKIA_CMD_DISP_CTRL(mode) (0x08 | ((mode) & 1) | (((mode) & 2) << 1))

/**
 * Set Y address of RAM.
 *
 * Can be called with H=0.
 *
 * @param Y The memory bank to address from 0-5.
 */
#define NOKIA_CMD_SET_Y(Y) (0x40 | ((Y) & 7))

/**
 * Set X address of RAM.
 *
 * Can be called with H=0.
 *
 * @param X The X-address to set from 0-83.
 */
#define NOKIA_CMD_SET_X(X) (0x80 | ((X) & 0x7F))

/**
 * Set temperature coefficient.
 *
 * Can be called with H=1.
 *
 * @param TC The temperature coefficient.
 */
#define NOKIA_CMD_SET_TC(TC) (0x4 | ((TC) & 0x3))

/**
 * Set Bias System.
 *
 * Can be called with H=1.
 *
 * @param BS The bias value.
 */
#define NOKIA_CMD_SET_BS(BS) (0x10 | ((BS) & 0x7))

/**
 * Set Vop (contrast).
 *
 * Can be called with H=1.
 *
 * @param VOP The contrast value.
 */
#define NOKIA_CMD_SET_VOP(VOP) (0x80 | ((VOP) & 0x7F))

/**
 * Initialize the Nokia5110Inst and the underlying SPI module the display is
 * connected to.
 *
 * @note Before usage, the GPIOs for the SPI module and the CD/RST lines will
 *       need to be setup.
 *
 * @note If software management of the slave select (NSS) line is preferred, then
 *       spi_use_software_ss() must be called after the call to nokia_init() but
 *       before any other functions.
 *
 * @param inst The nokia5110 instance to initialize. There should be one instance
 *             per physical display module.
 * @param spi_reg The underlying SPI module the display module is connected to.
 * @param dc_reg GPIO register for the data/command line.
 * @param dc_pin GPIO pin to be used as the data/command line.
 * @param rst_reg GPIO register used for the reset line.
 * @param rst_pin GPIO pin to be used as the reset line.
 */
void nokia_init(
	Nokia5110Inst *inst,
	SpiReg *spi_reg,
	GpioReg *dc_reg,
	GpioPin dc_pin,
	GpioReg *rst_reg,
	GpioPin rst_pin)
{
	ASSERT(inst != NULL);
	ASSERT(spi_reg != NULL);
	ASSERT(dc_reg != NULL);
	ASSERT(rst_reg != NULL);

	inst->dc_reg = dc_reg;
	inst->dc_pin = dc_pin;

	/* SPI2/3 are on APB1, all other SPI modules are on APB2. */
	const uint32_t pclk = spi_get_periph_clock(spi_reg);
	const SpiBaudRateDiv div = (pclk == APB1_HZ) ? SPI_BR_DIV_16 : SPI_BR_DIV_32;

	/* Enforce that the Nokia 5110 serial clock is 4MHz or less. */
	ASSERT((pclk / (1 << (div + 1))) <= 4000000U);

	spi_init(&inst->spi, spi_reg, SPI_CPHA_0, SPI_CPOL_0, div, SPI_MSBFIRST, SPI_DS_8BIT);

	/* Toggle the reset line before sending commands/data. */
	gpio_set_output(rst_reg, rst_pin, GPIO_LOW);
	sleep(USECS(1));
	gpio_set_output(rst_reg, rst_pin, GPIO_HIGH);
}

/**
 * Return a pointer to the SPI instance used by this nokia instance. This is
 * useful for setting a software managed slave select pin or reconfiguring the
 * underlying SPI module between transactions when multiple devices are using
 * the same SPI interface.
 *
 * @param inst The nokia5110 instance to extract the SPI instance from.
 *
 * @return The SPI instance used by the passed in Nokia instance.
 */
SpiInst* nokia_get_spi_inst(Nokia5110Inst *inst)
{
	ASSERT(inst != NULL);

	return &inst->spi;
}

/**
 * Send a stream of commands or data to the Nokia display.
 *
 * @param inst The nokia5110 instance to send commands to.
 * @param dc True to send data, false to send commands.
 * @param cmds Array of commands to send to the display.
 * @param num_cmds Number of commands in the cmds array.
 */
static void nokia_send_cmds(Nokia5110Inst *inst, bool dc, const uint8_t *cmds, uint8_t num_cmds)
{
	ASSERT(inst != NULL);

	/* Set the Command/Data line. */
	gpio_set_output(inst->dc_reg, inst->dc_pin, dc ? GPIO_HIGH : GPIO_LOW);

	spi_enable(&inst->spi);

	for(int i = 0; i < num_cmds; ++i) {
		spi_write(&inst->spi, cmds[i]);
	}

	spi_disable(&inst->spi);
}

/**
 * Set various paramaters used by the display. This should be called before
 * writing data into the graphics RAM.
 *
 * @param inst The nokia5110 instance to send commands to.
 * @param vop The Vop voltage that controls the LCD contrast. Recommended: 0xBF
 * @param tc The temperature coefficient. Recommended: 0
 * @param bs The LCD bias voltage. Recommended: 0x14
 */
void nokia_set_params(Nokia5110Inst *inst, uint8_t vop, uint8_t tc, uint8_t bs)
{
	const uint8_t cmds[] = {
		NOKIA_CMD_FUNC_SET(0, 0, 1),
		NOKIA_CMD_SET_VOP(vop),
		NOKIA_CMD_SET_TC(tc),
		NOKIA_CMD_SET_BS(bs)
	};

	nokia_send_cmds(inst, false, cmds, sizeof(cmds) / sizeof(cmds[0]));
}

/**
 * Set the display mode.
 *
 * @param inst The nokia5110 instance to set the display mode to.
 * @param mode The mode to set it to.
 */
void nokia_set_disp_mode(Nokia5110Inst *inst, Nokia5110DispMode mode)
{
	const uint8_t cmds[] = {
		NOKIA_CMD_FUNC_SET(0, 0, 0),
		NOKIA_CMD_DISP_CTRL(mode)
	};

	nokia_send_cmds(inst, false, cmds, sizeof(cmds) / sizeof(cmds[0]));
}

/**
 * Set every pixel on the screen to zero.
 *
 * @param inst The nokia instance to clear out.
 */
void nokia_clear_screen(Nokia5110Inst *inst)
{
	/* Reset the position back to (0,0). */
	const uint8_t cmds[] = {
		NOKIA_CMD_FUNC_SET(0, 0, 0),
		NOKIA_CMD_SET_X(0),
		NOKIA_CMD_SET_Y(0)
	};

	nokia_send_cmds(inst, false, cmds, sizeof(cmds) / sizeof(cmds[0]));

	/* Clear out every column of data on the screen. */
	const uint8_t zero = 0;
	for(unsigned int x = 0; x < NOKIA_WIDTH_PIXELS; x++) {
		for(unsigned int y = 0; y < NOKIA_HEIGHT_BANKS; ++y) {
			nokia_send_cmds(inst, true, &zero, 1);
		}
	}
}

/**
 * Set the X (column) and Y (bank) position of the next drawn column of pixels.
 *
 * @param inst The nokia5110 instance to set the position on.
 * @param column The X-position of the column of pixels to set (0-83).
 * @param bank The Y-position of the bank to set (0-5).
 */
void nokia_set_position(Nokia5110Inst *inst, uint8_t column, uint8_t bank)
{
	ASSERT(column < NOKIA_WIDTH_PIXELS);
	ASSERT(bank < NOKIA_HEIGHT_BANKS);

	const uint8_t cmds[] = {
		NOKIA_CMD_FUNC_SET(0, 0, 0),
		NOKIA_CMD_SET_X(column),
		NOKIA_CMD_SET_Y(bank)
	};

	nokia_send_cmds(inst, false, cmds, sizeof(cmds) / sizeof(cmds[0]));
}

/**
 * Send a column of pixels to the screen at the current position.
 *
 * @param inst The nokia instance to display onto.
 * @param data The 8 pixels to send to the screen.
 */
void nokia_set_column(Nokia5110Inst *inst, uint8_t data)
{
	nokia_send_cmds(inst, true, &data, 1);
}
