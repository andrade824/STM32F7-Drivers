/**
 * @author Devon Andrade
 * @created 11/25/2019
 *
 * Definitions and functions used to manipulate the SPI module. The only SPI
 * configuration currently supported is full-duplex master mode.
 */
#include "debug.h"
#include "spi.h"

#include "registers/rcc_reg.h"
#include "registers/spi_reg.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * Initialize an SPI module.
 *
 * @note Only configuration currently supported is full-duplex master mode.
 *
 * @note Before usage, the GPIOs for this SPI will need to be setup. After
 *       this init function is called, the spi module will still need to be
 *       enabled using spi_enable(). If use_hardware_ss isn't true, then the
 *       user will need to manage the slave select line themselves.
 *
 * @param spi Pointer to the SPI register map for the wanted SPI module.
 * @param cpha The wanted clock phase.
 * @param cpol The wanted clock polarity.
 * @param baud_div A divider for the peripheral clock used to generate the serial
 *    clock (SCK). For SPI2/3, this is a divider on APB1. For SPI1/4/5/6, this is
 *    a divider on APB2.
 * @param lsb_first Whether to send data as MSB or LSB first.
 * @param data_size Size of the data to shift out (4 to 16 bits).
 * @param use_hardware_ss True to have the NSS pin asserted low when the SPI
 *    module is enabled (with spi_enable) and de-asserted high when the SPI module
 *    is disabled (with spi_disable). False to disable hardware management of the
 *    NSS line and let the user manage the GPIO.
 */
void spi_init(
	SpiReg *spi,
	SpiClockPhase cpha,
	SpiClockPolarity cpol,
	SpiBaudRateDiv baud_div,
	SpiLsbFirst lsb_first,
	SpiDataSize data_size,
	bool use_hardware_ss)
{
	switch((uintptr_t)spi) {
		case SPI1_BASE: { SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_SPI1EN()); break; }
		case SPI2_BASE: { SET_FIELD(RCC->APB1ENR, RCC_APB1ENR_SPI2EN()); break; }
		case SPI3_BASE: { SET_FIELD(RCC->APB1ENR, RCC_APB1ENR_SPI3EN()); break; }
		case SPI4_BASE: { SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_SPI4EN()); break; }
		case SPI5_BASE: { SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_SPI5EN()); break; }
		case SPI6_BASE: { SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_SPI6EN()); break; }
		default: { ABORT("Invalid SPI module passed to %s\n", __FUNCTION__); }
	}

	__asm("dsb");

	spi->CR1 = SET_SPI_CR1_CPHA(cpha) |
	           SET_SPI_CR1_CPOL(cpol) |
	           SPI_CR1_MSTR() |
	           SET_SPI_CR1_SSM(use_hardware_ss ? 0 : 1) |
	           SET_SPI_CR1_BR(baud_div) |
	           SET_SPI_CR1_LSBFIRST(lsb_first);

	spi->CR2 = SET_SPI_CR2_FRXTH((data_size > SPI_DS_8BIT) ? 0 : 1) |
	           SET_SPI_CR2_SSOE(use_hardware_ss ? 1 : 0) |
	           SET_SPI_CR2_DS(data_size);
}

/**
 * Enable the SPI module and get it ready to start transmitting and receiving
 * data.
 *
 * If you passed use_hardware_ss as true in spi_init(), then the slave select
 * (NSS) line will be asserted low until spi_disable() is called.
 *
 * @param spi Pointer to the SPI register map for the wanted SPI module.
 */
void spi_enable(SpiReg *spi)
{
	/**
	 * If the slave select (NSS) line is being managed by software, the SSI bit
	 * needs to be set for the SPI module to start transmitting data. If the NSS
	 * line is managed by hardware, then the SSI bit is ignored.
	 */
	SET_FIELD(spi->CR1, SPI_CR1_SPE() | SPI_CR1_SSI());
}

/**
 * Disable the SPI module.
 *
 * If you passed use_hardware_ss as true in spi_init(), then the slave select
 * (NSS) line will be asserted high until spi_enable() is called again.
 *
 * @param spi Pointer to the SPI register map for the wanted SPI module.
 */
void spi_disable(SpiReg *spi)
{
	/* Wait until there is no more data to transmit. */
	while(GET_SPI_SR_FTLVL(spi->SR) != 0);

	/* Wait until the SPI module has finished all bus transactions. */
	while(GET_SPI_SR_BSY(spi->SR) != 0);

	CLEAR_FIELD(spi->CR1, SPI_CR1_SPE() | SPI_CR1_SSI());

	/* Read out any last chunks of data currently in the FIFO. */
	while(GET_SPI_SR_FRLVL(spi->SR) != 0) {
		(void)spi->DR;
	}
}

/**
 * Send a chunk of data over SPI without reading the response. This is useful
 * for supporting a transmit-only mode (where responses from the slave are
 * ignored).
 *
 * @param spi Pointer to the SPI register map for the wanted SPI module.
 * @param data The data to send over the MOSI line.
 */
void spi_write(SpiReg *spi, uint16_t data)
{
	/* Wait for the last transmission to finish. */
	while(!GET_SPI_SR_TXE(spi->SR));

	/**
	 * If the SPI data size is 8-bits or less, then performing a 16-bit write
	 * will unitentionally fill up two entries in the TX FIFO.
	 */
	if(GET_SPI_CR2_DS(spi->CR2) > SPI_DS_8BIT) {
		spi->DR = data;
	} else {
		*(volatile uint8_t *)&spi->DR = (uint8_t)data;
	}
}

/**
 * Send a chunk of data over SPI and return the response.
 *
 * @param spi Pointer to the SPI register map for the wanted SPI module.
 * @param data The data to send over the MOSI line.
 */
uint16_t spi_send_receive(SpiReg *spi, uint16_t data)
{
	/* Wait for the last transmission to finish. */
	while(!GET_SPI_SR_TXE(spi->SR));

	/**
	 * If the SPI data size is 8-bits or less, then performing a 16-bit write
	 * will unitentionally fill up two entries in the TX FIFO.
	 */
	if(GET_SPI_CR2_DS(spi->CR2) > SPI_DS_8BIT) {
		spi->DR = data;
	} else {
		*(volatile uint8_t *)&spi->DR = (uint8_t)data;
	}

	/* Wait for the response. */
	while(!GET_SPI_SR_RXNE(spi->SR));

	uint16_t result = 0;
	if(GET_SPI_CR2_DS(spi->CR2) > SPI_DS_8BIT) {
		result = spi->DR;
	} else {
		result = *(volatile uint8_t *)&spi->DR;
	}

	return result;
}
