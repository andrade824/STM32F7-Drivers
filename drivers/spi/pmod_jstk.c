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
#include "spi/pmod_jstk.h"
#include "spi.h"
#include "system_timer.h"

#include "registers/spi_reg.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* How many bytes to retrieve from the joystick in one transaction. */
#define JOYSTICK_DATA_LENGTH 5

/**
 * Initialize the PmodJstkInst and the underlying SPI module the joystick is
 * connected to.
 *
 * @note Before usage, the GPIOs for the SPI module will need to be setup.
 *
 * @param inst The joystick instance to initialize. There should be one instance
 *             per physical joystick module.
 * @param spi The underlying SPI module the joystick module is connected to.
 * @param use_hardware_ss True to let the SPI module manage the slave select
 *                        line. False to let this PmodJstk module handle the
 *                        slave select. If you pass false here you must call
 *                        jstk_set_ss_pin() before any other method is called.
 */
void jstk_init(PmodJstkInst *inst, SpiReg *spi, bool use_hardware_ss)
{
	ASSERT(inst != NULL);
	ASSERT(spi != NULL);

	inst->spi = spi;
	inst->use_hardware_ss = use_hardware_ss;
	inst->ss_reg = NULL;
	inst->ss_pin = 0;

	/* SPI2/3 are on APB1, all other SPI modules are on APB2. */
	const uint32_t pclk = (((uintptr_t)spi == SPI2_BASE) || ((uintptr_t)spi == SPI3_BASE)) ? APB1_HZ : APB2_HZ;
	const SpiBaudRateDiv div = (pclk == APB1_HZ) ? SPI_BR_DIV_64 : SPI_BR_DIV_128;

	/* Enforce that the joystick serial clock is 1MHz or less. */
	ASSERT((pclk / (1 << (div + 1))) <= 1000000U);

	spi_init(spi, SPI_CPHA_0, SPI_CPOL_0, div, SPI_MSBFIRST, SPI_DS_8BIT, use_hardware_ss);
}

/**
 * If software slave select management is being used, this sets the pin to be
 * used as the slave select (NSS) line.
 *
 * @param inst The joystick instance to tie the pin to.
 * @param ss_reg The GPIO register that controls the wanted pin.
 * @param ss_pin The GPIO pin to use as a slave select.
 */
void jstk_set_ss_pin(PmodJstkInst *inst, GpioReg *ss_reg, GpioPin ss_pin)
{
	ASSERT(inst != NULL);
	ASSERT(ss_reg != NULL);
	ASSERT(!inst->use_hardware_ss);

	inst->ss_reg = ss_reg;
	inst->ss_pin = ss_pin;
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
	ASSERT(inst->spi != NULL);

	spi_enable(inst->spi);

	/**
	 * If the SPI module is handling the NSS line, it will get asserted by
	 * spi_enable. Otherwise, manually assert the NSS line.
	 */
	if(!inst->use_hardware_ss) {
		ASSERT(inst->ss_reg != NULL);
		gpio_set_output(inst->ss_reg, inst->ss_pin, GPIO_LOW);
	}

	/* Must wait 15uS after slave select line gets asserted. */
	sleep(USECS(15));

	for(int i = 0; i < JOYSTICK_DATA_LENGTH; ++i) {
		data[i] = spi_send_receive(inst->spi, data[i]);
		sleep(USECS(10)); /* Must wait 10uS between reading bytes. */
	}

	spi_disable(inst->spi);

	if(!inst->use_hardware_ss) {
		gpio_set_output(inst->ss_reg, inst->ss_pin, GPIO_HIGH);
	}

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

void jstk_set_leds(PmodJstkInst *inst, bool led1, bool led2)
{
	ASSERT(inst != NULL);

	uint8_t data[JOYSTICK_DATA_LENGTH] = { 0 };
	data[0] = 0x80 | led1 | (led2 << 1);
	jstk_spi_transfer(inst, data);
}
