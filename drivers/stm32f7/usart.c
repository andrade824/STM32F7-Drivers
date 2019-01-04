/**
 * @author Devon Andrade
 * @created 12/27/2018
 *
 * Definitions and functions used to manipulate the USART [31].
 */
#if INCLUDE_USART_DRIVER

/**
 * Initialize a USART module with some basic settings.
 *
 * @note Before usage, the GPIOs for this USART will need to be setup. The
 *       transmitter and/or receiver will also need to be enabled using the
 *       usart_enable_tx() usart_enable_rx() methods respectively.
 *
 * @param usart Pointer to the USART register map for the wanted USART module.
 * @param baud The wanted baud rate.
 * @param data_bits The wanted number of data bits.
 * @param stop_bits The wanted number of stop bits.
 */
status_t init_usart(UsartReg *usart,
                    uint32_t baud,
                    UsartWordLength data_bits,
                    UsartStopBits stop_bits)
{

}

void usart_enable_tx(UsartReg *usart, bool enable)
{

}

void usart_enable_rx(UsartReg *usart, bool enable)
{

}

void usart_send_byte(UsartReg *usart, uint8_t data)
{

}

void usart_send(UsartReg *usart, uint8_t *data, uint32_t num_bytes)
{

}

/**
 * Instead of this, maybe just functions for checking flags and clearing them, and enabling the interrupt.
 */
void usart_register_rx_isr(UsartReg *usart, IRQn_Type irq, void (*rx_callback) (uint8_t))
{

}

// ASSERT(rx_callback == NULL);
uint8_t usart_receive(UsartReg *usart)
{

}

#endif /* INCLUDE_USART_DRIVER */
