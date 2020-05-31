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
 *       enabled using spi_enable().
 *
 * @note When software management of the slave select line is not used
 *       (spi_use_software_ss() never gets called), then the SPI module will
 *       drive the NSS line as open-drain. This means that if a strong enough
 *       pull-up isn't used then the NSS line might not return high between
 *       accesses which can cause issues. If ample delays aren't given between
 *       accesses to account for this then it's recommended to use a software
 *       managed slave select to avoid this problem completely (which will drive
 *       the line as push-pull).
 *
 * @param inst The SPI instance to tie this configuration to.
 * @param spi Pointer to the SPI register map for the wanted SPI module.
 * @param cpha The wanted clock phase.
 * @param cpol The wanted clock polarity.
 * @param baud_div A divider for the peripheral clock used to generate the serial
 *    clock (SCK). For SPI2/3, this is a divider on APB1. For SPI1/4/5/6, this is
 *    a divider on APB2.
 * @param lsb_first Whether to send data as MSB or LSB first.
 * @param data_size Size of the data to shift out (4 to 16 bits).
 */
void spi_init(
	SpiInst *inst,
	SpiReg *spi,
	SpiClockPhase cpha,
	SpiClockPolarity cpol,
	SpiBaudRateDiv baud_div,
	SpiLsbFirst lsb_first,
	SpiDataSize data_size)
{
	ASSERT(inst != NULL);
	ASSERT(spi != NULL);

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

	inst->spi = spi;
	inst->cpha = cpha;
	inst->cpol = cpol;
	inst->baud_div = baud_div;
	inst->lsb_first = lsb_first;
	inst->data_size = data_size;
	inst->use_software_ss = false;
	inst->ss_reg = NULL;
	inst->ss_pin = 0;

	spi_reinit(inst);
}

/**
 * Re-initialize the underlying SPI module with the same parameters used in
 * spi_init(). This is useful when two slaves are using the same SPI interface
 * but require different SPI configurations.
 *
 * @note Make sure to call this function before calling any other SPI functions
 *       after talking to a different device on the same SPI interface
 *       as this one.
 *
 * @param inst The SPI instance to re-initialize.
 */
void spi_reinit(SpiInst *inst)
{
	ASSERT(inst != NULL);
	ASSERT(inst->spi != NULL);

	SpiReg * const spi = inst->spi;

	/* You should only modify the SPI configuration when the SPI is disabled. */
	ASSERT(GET_SPI_CR1_SPE(spi->CR1) == 0);

	spi->CR1 = SET_SPI_CR1_CPHA(inst->cpha) |
	           SET_SPI_CR1_CPOL(inst->cpol) |
	           SPI_CR1_MSTR() |
	           SET_SPI_CR1_SSM(inst->use_software_ss ? 1 : 0) |
	           SET_SPI_CR1_BR(inst->baud_div) |
	           SET_SPI_CR1_LSBFIRST(inst->lsb_first);

	spi->CR2 = SET_SPI_CR2_FRXTH((inst->data_size > SPI_DS_8BIT) ? 0 : 1) |
	           SET_SPI_CR2_SSOE(inst->use_software_ss ? 0 : 1) |
	           SET_SPI_CR2_DS(inst->data_size);
}

/**
 * This marks the SPI instance to use software slave select management, and then
 * sets the pin to be used as the slave select (NSS) line.
 *
 * @param inst The SPI instance to tie the pin to.
 * @param ss_reg The GPIO register that controls the wanted pin.
 * @param ss_pin The GPIO pin to use as a slave select.
 */
void spi_use_software_ss(SpiInst *inst, GpioReg *ss_reg, GpioPin ss_pin)
{
	ASSERT(inst != NULL);
	ASSERT(ss_reg != NULL);

	inst->use_software_ss = true;
	inst->ss_reg = ss_reg;
	inst->ss_pin = ss_pin;

	spi_reinit(inst);
}

/**
 * Enable the SPI module and get it ready to start transmitting and receiving
 * data.
 *
 * The slave select (NSS) line will be asserted low until spi_disable() is
 * called. If spi_use_software_ss() was called, then that specified NSS pin will
 * be used. Otherwise the hardware-defined NSS line will be used.
 *
 * @param inst Pointer to the SPI instance for the wanted SPI module.
 */
void spi_enable(SpiInst *inst)
{
	/**
	 * If the slave select (NSS) line is being managed by software, the SSI bit
	 * needs to be set for the SPI module to start transmitting data. If the NSS
	 * line is managed by hardware, then the SSI bit is ignored.
	 */
	SET_FIELD(inst->spi->CR1, SPI_CR1_SPE() | SPI_CR1_SSI());

	/* Software management of the slave select line. */
	if(inst->use_software_ss) {
		ASSERT(inst->ss_reg != NULL);
		gpio_set_output(inst->ss_reg, inst->ss_pin, GPIO_LOW);
	}
}

/**
 * Disable the SPI module.
 *
 * The slave select (NSS) line will be asserted high until spi_enable() is
 * called. If spi_use_software_ss() was called, then that specified NSS pin will
 * be used. Otherwise the hardware-defined NSS line will be used.
 *
 * @param inst Pointer to the SPI instance for the wanted SPI module.
 */
void spi_disable(SpiInst *inst)
{
	ASSERT(inst != NULL);
	ASSERT(inst->spi != NULL);

	SpiReg * const spi = inst->spi;

	/* Wait until there is no more data to transmit. */
	while(GET_SPI_SR_FTLVL(spi->SR) != 0);

	/* Wait until the SPI module has finished all bus transactions. */
	while(GET_SPI_SR_BSY(spi->SR) != 0);

	CLEAR_FIELD(spi->CR1, SPI_CR1_SPE() | SPI_CR1_SSI());

	/* Software management of the slave select line. */
	if(inst->use_software_ss) {
		ASSERT(inst->ss_reg != NULL);
		gpio_set_output(inst->ss_reg, inst->ss_pin, GPIO_HIGH);
	}

	/* Read out any last chunks of data currently in the FIFO. */
	while(GET_SPI_SR_FRLVL(spi->SR) != 0) {
		(void)spi->DR;
	}
}

/**
 * Get the peripheral clock the wanted SPI module is connected to. This is useful
 * for higher layer drivers to dynamically determine the correct divider to get
 * the serial clock speed for the device being interfaced with.
 *
 * @param spi Pointer to the SPI register map for the wanted SPI module.
 *
 * @return The speed of the peripheral clock driving the wanted SPI module.
 */
uint32_t spi_get_periph_clock(SpiReg *spi)
{
	ASSERT(spi != NULL);

	/* SPI2/3 are on APB1, all other SPI modules are on APB2. */
	return (((uintptr_t)spi == SPI2_BASE) ||
	    ((uintptr_t)spi == SPI3_BASE)) ? APB1_HZ : APB2_HZ;
}

/**
 * Send a chunk of data over SPI without reading the response. This is useful
 * for supporting a transmit-only mode (where responses from the slave are
 * ignored).
 *
 * @param inst Pointer to the SPI instance for the wanted SPI module.
 * @param data The data to send over the MOSI line.
 */
void spi_write(SpiInst *inst, uint16_t data)
{
	ASSERT(inst != NULL);
	ASSERT(inst->spi != NULL);
	SpiReg * const spi = inst->spi;

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
 * @param inst Pointer to the SPI instance for the wanted SPI module.
 * @param data The data to send over the MOSI line.
 */
uint16_t spi_send_receive(SpiInst *inst, uint16_t data)
{
	ASSERT(inst != NULL);
	ASSERT(inst->spi != NULL);
	SpiReg * const spi = inst->spi;

	/* Wait for the last transmission to finish. */
	while(!GET_SPI_SR_TXE(spi->SR));

	/**
	 * If the SPI data size is 8-bits or less, then performing a 16-bit write
	 * will unintentionally fill up two entries in the TX FIFO.
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
