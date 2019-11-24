/**
 * @author Devon Andrade
 * @created 12/25/2018 (Merry Christmas!)
 *
 * Definitions and functions used to manipulate the USART.
 */
#pragma once

#if INCLUDE_USART_DRIVER

#include "gpio.h"

#include "registers/usart_reg.h"

#include <stdbool.h>
#include <stdint.h>

void usart_init(
	UsartReg *usart,
	uint32_t baud,
	UsartWordLength data_bits,
	UsartStopBits stop_bits);
void usart_enable_tx(UsartReg *usart, bool enable);
void usart_enable_rx(UsartReg *usart, bool enable);

void usart_send_byte(UsartReg *usart, uint8_t data);
void usart_send(UsartReg *usart, uint8_t *data, uint32_t num_bytes);

uint8_t usart_receive(UsartReg *usart);

#endif /* INCLUDE_USART_DRIVER */
