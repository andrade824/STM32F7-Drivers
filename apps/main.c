#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "stm32f7_tests.h"
#include "system.h"

#include "spi.h"
#include "spi/rfm69_radio.h"

#include <string.h>

int main(void)
{
	system_init();
	dbprintf("System Initialized\n");

#ifdef config_stm32f7_dev_board
	/* SPI2 pins on the STM32F7 dev board. */
	#define GPIO_ARD_D13 GPIO_PB13 /* SCK */
	#define GPIO_ARD_D12 GPIO_PB14 /* MISO */
	#define GPIO_ARD_D11 GPIO_PB15 /* MOSI */
	#define GPIO_ARD_D5  GPIO_PB12 /* NSS */
	#define GPIO_ARD_D2  GPIO_PB11 /* RST */
	#define GPIO_ARD_D1  GPIO_PA12 /* TEST */
#endif

	gpio_request_alt(GPIO_ARD_D13, AF5, GPIO_OSPEED_25MHZ); /* SPI2 SCK */
	gpio_request_alt(GPIO_ARD_D12, AF5, GPIO_OSPEED_25MHZ); /* SPI2 MISO */
	gpio_request_alt(GPIO_ARD_D11, AF5, GPIO_OSPEED_25MHZ); /* SPI2 MOSI */

	gpio_request_output(GPIO_ARD_D5, GPIO_HIGH);            /* SPI2 NSS */
	gpio_request_output(GPIO_ARD_D2, GPIO_HIGH);            /* RFM69 RST */
	gpio_request_output(GPIO_ARD_D1, GPIO_LOW);

	Rfm69Inst radio;
	rfm69_init_radio(&radio, SPI2, GPIO_ARD_D2, GPIO_ARD_D5);
	rfm69_set_payload_length(&radio, 1);
	rfm69_set_power_mode(&radio, RFM69_PA1, 31); /* 13dBm */

#ifdef config_stm32f7_dev_board
	gpio_request_output(GPIO_LED_USER, GPIO_LOW);
	DigitalState led_ctrl = GPIO_LOW;

	uint8_t data = 0;
	while(1) {
		sleep(MSECS(1000));
		rfm69_send(&radio, &data, 1);
		data++;

		if(led_ctrl == GPIO_LOW) {
			led_ctrl = GPIO_HIGH;
		} else {
			led_ctrl = GPIO_LOW;
		}

		gpio_set_output(GPIO_LED_USER, led_ctrl);
	}
#else
	uint8_t num_read = 0;
	uint8_t data = 0;
	while(1) {
		num_read = rfm69_receive(&radio, &data, 1);
		ASSERT(num_read == 1);
		dbprintf("Data: %d | RSSI: %d dBm\n", data, rfm69_get_last_rssi(&radio));
	}
#endif

#if 0
	uint8_t num_read = 0;
	while(1) {
		sleep(MSECS(1000));
		rfm69_send(&radio, &data, 1);
		num_read = rfm69_receive(&radio, &data, 1);
		ASSERT(num_read == 1);

		dbprintf("Data: %d | RSSI: %d dBm\n", data, rfm69_get_last_rssi(&radio));
		data++;
	}
#endif

#if 0
	gpio_request_input(GPIO_BTN_USER, GPIO_NO_PULL);
	gpio_request_output(GPIO_LED_USER, GPIO_LOW);

	DigitalState led_ctrl = GPIO_LOW;

	while(1) {
		if(gpio_get_input(GPIO_BTN_USER) == GPIO_LOW) {
			if(led_ctrl == GPIO_LOW) {
				led_ctrl = GPIO_HIGH;
			} else {
				led_ctrl = GPIO_LOW;
			}
		} else {
			dbprintf("Button pressed!\n");
		}

		gpio_set_output(GPIO_LED_USER, led_ctrl);
		sleep(MSECS(500));
	}
#else
	while(1) {
		// Loop forever
	}
#endif

	return 0;
}
