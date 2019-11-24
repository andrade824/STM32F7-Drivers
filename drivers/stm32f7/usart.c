/**
 * @author Devon Andrade
 * @created 12/27/2018
 *
 * Definitions and functions used to manipulate the USART [31].
 */
#if INCLUDE_USART_DRIVER

#include "config.h"
#include "debug.h"
#include "usart.h"

#include "registers/rcc_reg.h"
#include "registers/usart_reg.h"

#include <stdint.h>

/**
 * Initialize a USART module with some basic settings.
 *
 * @note Before usage, the GPIOs for this USART will need to be setup. After
 *       usage, the transmitter and/or receiver will also need to be enabled
 *       using the usart_enable_tx()/usart_enable_rx() methods respectively.
 *
 * @param usart Pointer to the USART register map for the wanted USART module.
 * @param baud The wanted baud rate.
 * @param data_bits The wanted number of data bits.
 * @param stop_bits The wanted number of stop bits.
 */
void usart_init(
	UsartReg *usart,
	uint32_t baud,
	UsartWordLength data_bits,
	UsartStopBits stop_bits)
{
	uint8_t usart_num = 0;

	if(usart == USART1) {
		usart_num = 0;
		SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_USART1EN());
	} else if(usart == USART2) {
		usart_num = 1;
		SET_FIELD(RCC->APB1ENR, RCC_APB1ENR_USART2EN());
	} else if(usart == USART3) {
		usart_num = 2;
		SET_FIELD(RCC->APB1ENR, RCC_APB1ENR_USART3EN());
	} else if(usart == UART4) {
		usart_num = 3;
		SET_FIELD(RCC->APB1ENR, RCC_APB1ENR_UART4EN());
	} else if(usart == UART5) {
		usart_num = 4;
		SET_FIELD(RCC->APB1ENR, RCC_APB1ENR_UART5EN());
	} else if(usart == USART6) {
		usart_num = 5;
		SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_USART6EN());
	} else if(usart == UART7) {
		usart_num = 6;
		SET_FIELD(RCC->APB1ENR, RCC_APB1ENR_UART7EN());
	} else if(usart == UART8) {
		usart_num = 7;
		SET_FIELD(RCC->APB1ENR, RCC_APB1ENR_UART8EN());
	} else {
		ABORT("Invalid USART module passed to %s\n", __FUNCTION__);
	}

	/* The USART clock selection fields are contiguous in RCC->DCKCFGR2[15:0]. */
	RCC->DCKCFGR2 |= (RCC_USARTSEL_SYSCLK) << (usart_num * 2);

	__asm("dsb");

	SET_FIELD(usart->BRR, SET_USART_BRR_BRR(CPU_HZ / baud));
	SET_FIELD(usart->CR3, SET_USART_CR3_OVRDIS(1));
	SET_FIELD(usart->CR2, SET_USART_CR2_STOP(stop_bits));
	SET_FIELD(usart->CR1, SET_USART_CR1_M0(data_bits & 1) |
	                      SET_USART_CR1_M1((data_bits >> 1) & 1) |
	                      SET_USART_CR1_UE(1));
}

/**
 * Enable the USART transmitter.
 *
 * @param usart Pointer to the USART register map for the wanted USART module.
 * @param enable True to enable the transmitter, false to disable it.
 */
void usart_enable_tx(UsartReg *usart, bool enable)
{
	CLEAR_FIELD(usart->CR1, USART_CR1_TE());
	SET_FIELD(usart->CR1, SET_USART_CR1_TE((enable) ? 1 : 0));
}

/**
 * Enable the USART receiver.
 *
 * @param usart Pointer to the USART register map for the wanted USART module.
 * @param enable True to enable the receiver, false to disable it.
 */
void usart_enable_rx(UsartReg *usart, bool enable)
{
	CLEAR_FIELD(usart->CR1, USART_CR1_RE());
	SET_FIELD(usart->CR1, SET_USART_CR1_RE((enable) ? 1 : 0));
}

/**
 * Send a single byte of data over the USART.
 *
 * @param usart Pointer to the USART register map for the wanted USART module.
 * @param data The byte of data to send.
 */
void usart_send_byte(UsartReg *usart, uint8_t data)
{
	/* Wait for any current transmissions to complete. */
	while(!GET_USART_ISR_TXE(usart->ISR));

	usart->TDR = data;
}

/**
 * Send a multiple bytes of data over the USART.
 *
 * @param usart Pointer to the USART register map for the wanted USART module.
 * @param data The bytes of data to send.
 * @param num_bytes The number of bytes of data to send.
 */
void usart_send(UsartReg *usart, uint8_t *data, uint32_t num_bytes)
{
	for(uint32_t i = 0; i < num_bytes; ++i) {
		usart_send_byte(usart, data[i]);
	}
}

/**
 * Block until a character is receiver over the USART and return it.
 *
 * @param usart Pointer to the USART register map for the wanted USART module.
 *
 * @retval The received byte of data.
 */
uint8_t usart_receive(UsartReg *usart)
{
	/* Wait for a character to be received. */
	while(!GET_USART_ISR_RXNE(usart->ISR));

	return (uint8_t)usart->RDR;
}

#endif /* INCLUDE_USART_DRIVER */
