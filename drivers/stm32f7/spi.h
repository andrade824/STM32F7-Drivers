/**
 * @author Devon Andrade
 * @created 11/25/2019
 *
 * Definitions and functions used to manipulate the SPI module. The only SPI
 * configuration currently supported is full-duplex master mode.
 */
#pragma once

#include "gpio.h"

#include "registers/spi_reg.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * Structure used to represent a single configuration for a SPI module. Multiple
 * configurations can exist that use a single SPI module (e.g., multiple devices
 * are connected to the same SPI module), the user just has to make sure to call
 * the spi_reinit() function before talking to each individual device.
 */
typedef struct {
	/* SPI module this instance uses. */
	SpiReg *spi;

	/* SPI clock phase. */
	SpiClockPhase cpha;

	/* SPI clock polarity. */
	SpiClockPolarity cpol;

	/* A divider for the peripheral clock used to generate the serial clock (SCK). */
	SpiBaudRateDiv baud_div;

	/* Whether to send data as MSB or LSB first. */
	SpiLsbFirst lsb_first;

	/* Size of the data to shift out (4 to 16bits). */
	SpiDataSize data_size;

	/* True to use software-managed slave select, false otherwise. */
	bool use_software_ss;

	/* If using software-managed slave select, this is the GPIO slave select pin. */
	GpioReg *ss_reg;
	GpioPin ss_pin;
} SpiInst;

void spi_init(
	SpiInst *inst,
	SpiReg *spi,
	SpiClockPhase cpha,
	SpiClockPolarity cpol,
	SpiBaudRateDiv baud_div,
	SpiLsbFirst lsb_first,
	SpiDataSize data_size);
void spi_reinit(SpiInst *inst);
void spi_use_software_ss(SpiInst *inst, GpioReg *ss_reg, GpioPin ss_pin);
void spi_enable(SpiInst *inst);
void spi_disable(SpiInst *inst);
uint32_t spi_get_periph_clock(SpiReg *spi);

void spi_write(SpiInst *inst, uint16_t data);
uint16_t spi_send_receive(SpiInst *inst, uint16_t data);
