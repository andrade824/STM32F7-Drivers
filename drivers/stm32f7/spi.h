/**
 * @author Devon Andrade
 * @created 11/25/2019
 *
 * Definitions and functions used to manipulate the SPI module. The only SPI
 * configuration currently supported is full-duplex master mode.
 */
#pragma once

#include "registers/spi_reg.h"

#include <stdbool.h>
#include <stdint.h>

void spi_init(
	SpiReg *spi,
	SpiClockPhase cpha,
	SpiClockPolarity cpol,
	SpiBaudRateDiv baud_div,
	SpiLsbFirst lsb_first,
	SpiDataSize data_size,
	bool use_hardware_ss);
void spi_enable(SpiReg *spi);
void spi_disable(SpiReg *spi);

void spi_write(SpiReg *spi, uint16_t data);
uint16_t spi_send_receive(SpiReg *spi, uint16_t data);
