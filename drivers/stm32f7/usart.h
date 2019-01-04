/**
 * @author Devon Andrade
 * @created 12/25/2018 (Merry Christmas!)
 *
 * Definitions and functions used to manipulate the USART [31].
 */
#if INCLUDE_USART_DRIVER
#ifndef USART_H
#define USART_H

#include "gpio.h"
#include "status.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * Settings used to initialize a USART module.
 */
typedef struct {
	UsartReg *reg; // MAKE THIS CONFIG, add an IRQn as well, and the RCC mask
	uint32_t baud;
	UsartWordLength data_bits;
	UsartStopBits stop_bits;
	void (*rx_callback) (uint8_t); // Triggered whenever a byte is received, NULL for no callback
	GpioReg *gpio; // MAKE ALL OF THESE CONFIG
	GpioPin rx_pin;
	GpioPin tx_pin;
	uint8_t rx_gpio_alt;
	uint8_t tx_gpio_alt;
} UsartSettings;

/* USART Stop bit length options. */
typedef enum {
	USART_1_STOP = 0,
	USART_2_STOP = 1,
	USART_0_5_STOP = 2,
	USART_1_5_STOP = 3
} UsartStopBits;

/* USART Data bit length options. */
typedef enum {
	USART_8_DATA = 0,
	USART_9_DATA = 1,
	USART_7_DATA = 2
} UsartWordLength;

status_t init_usart(UsartReg *usart,
                    uint32_t baud,
                    UsartWordLength data_bits,
                    UsartStopBits stop_bits);
void usart_enable_tx(UsartReg *usart, bool enable);
void usart_enable_rx(UsartReg *usart, bool enable);

void usart_send_byte(UsartReg *usart, uint8_t data);
void usart_send(UsartReg *usart, uint8_t *data, uint32_t num_bytes);

void usart_register_rx_isr(UsartReg *usart, IRQn_Type irq, void (*rx_callback) (uint8_t));
uint8_t usart_receive(UsartReg *usart);

#endif /* USART_H */
#endif /* INCLUDE_USART_DRIVER */
