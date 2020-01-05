/**
 * @author Devon Andrade
 * @created 11/27/2019
 *
 * Driver to control the PMOD_JSTK Joystick module (controlled using SPI).
 *
 * https://store.digilentinc.com/pmod-jstk-2-axis-joystick-retired/
 */
#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "spi.h"
#include "spi/pmod_jstk.h"
#include "system_timer.h"

#include "registers/spi_reg.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* How many bytes to retrieve from the joystick in one transaction. */
#define JOYSTICK_DATA_LENGTH 5

/**
 * Initialize the PmodJstkInst and the underlying SPI module the joystick is
 * connected to.
 *
 * @note Before usage, the GPIOs for the SPI module will need to be setup.
 *
 * @note If software management of the slave select (NSS) line is preferred, then
 *       spi_use_software_ss() must be called after the call to jstk_init() but
 *       before any other functions.
 *
 * @param inst The joystick instance to initialize. There should be one instance
 *             per physical joystick module.
 * @param spi_reg The underlying SPI module the joystick module is connected to.
 */
void jstk_init(PmodJstkInst *inst, SpiReg *spi_reg)
{
	ASSERT(inst != NULL);
	ASSERT(spi_reg != NULL);

	/* SPI2/3 are on APB1, all other SPI modules are on APB2. */
	const uint32_t pclk = spi_get_periph_clock(spi_reg);
	const SpiBaudRateDiv div = (pclk == APB1_HZ) ? SPI_BR_DIV_64 : SPI_BR_DIV_128;

	/* Enforce that the joystick serial clock is 1MHz or less. */
	ASSERT((pclk / (1 << (div + 1))) <= 1000000U);

	spi_init(&inst->spi, spi_reg, SPI_CPHA_0, SPI_CPOL_0, div, SPI_MSBFIRST, SPI_DS_8BIT);
}

/**
 * Return a pointer to the SPI instance used by this joystick instance. This is
 * useful for setting a software managed slave select pin or reconfiguring the
 * underlying SPI module between transactions when multiple devices are using
 * the same SPI interface.
 *
 * @param inst The joystick instance to extract the SPI instance from.
 *
 * @return The SPI instance used by the passed in Nokia instance.
 */
SpiInst* jstk_get_spi_inst(PmodJstkInst *inst)
{
	ASSERT(inst != NULL);

	return &inst->spi;
}

/**
 * Helper function for sending/retrieving data to/from the joystick module.
 *
 * @param inst The joystick instance to read data from.
 * @param data Must be an array of length JOYSTICK_DATA_LENGTH. Each byte in
 *             this array will be sent over SPI, and then overwritten with the
 *             results that were read back.
 */
static void jstk_spi_transfer(PmodJstkInst *inst, uint8_t *data)
{
	ASSERT(inst != NULL);

	spi_enable(&inst->spi);

	/* Must wait 15uS after slave select line gets asserted. */
	sleep(USECS(15));

	for(int i = 0; i < JOYSTICK_DATA_LENGTH; ++i) {
		data[i] = spi_send_receive(&inst->spi, data[i]);
		sleep(USECS(10)); /* Must wait 10uS between reading bytes. */
	}

	spi_disable(&inst->spi);

	/* Must wait 25uS after de-asserting SS line before it can get asserted again. */
	sleep(USECS(25));
}

/**
 * Get the joystick and button state out of the module.
 *
 * @param inst The joystick instance to read data from.
 * @param x The x-axis data. Set non-null if this data is wanted, otherwise NULL.
 * @param y The y-axis data. Set non-null if this data is wanted, otherwise NULL.
 * @param btns The button inputs. Set non-null if this data is wanted, otherwise
 *             NULL. Use the JSTK_BTN_* accessors to retrieve individual button
 *             states.
 */
void jstk_get_data(PmodJstkInst *inst, uint16_t *x, uint16_t *y, uint8_t *btns)
{
	ASSERT(inst != NULL);

	uint8_t data[JOYSTICK_DATA_LENGTH] = { 0 };
	jstk_spi_transfer(inst, data);

	if(x != NULL) {
		*x = data[0] | ((data[1] & 0x3) << 8);
	}

	if(y != NULL) {
		*y = data[2] | ((data[3] & 0x3) << 8);
	}

	if(btns != NULL) {
		*btns = data[4];
	}
}

/**
 * Set the LEDs on the joystick module.
 *
 * @param inst The joystick instance to set the LEDs on.
 * @param led1 True to enable LED 1, false otherwise.
 * @param led2 True to enable LED 2, false otherwise.
 */
void jstk_set_leds(PmodJstkInst *inst, bool led1, bool led2)
{
	ASSERT(inst != NULL);

	uint8_t data[JOYSTICK_DATA_LENGTH] = { 0 };
	data[0] = 0x80 | led1 | (led2 << 1);
	jstk_spi_transfer(inst, data);
}
