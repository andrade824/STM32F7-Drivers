#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "status.h"
#include "stm32f7_tests.h"
#include "system.h"

int main(void)
{
	init_system();
	dbprintf("System Initialized\n");

	usart_gfx_test();

#ifdef config_stm32f7_dev_board
#define GPIO_B_USER GPIO_BTN_USER
#define GPIO_ARD_D13 GPIO_LED_USER
#endif

	gpio_request_input(GPIO_B_USER, GPIO_NO_PULL);
	gpio_request_output(GPIO_ARD_D13, low);

	DigitalState led_ctrl = low;

	while(1)
	{
		if(gpio_get_input(GPIO_B_USER) == low) {
			if(led_ctrl == low) {
				led_ctrl = high;
			} else {
				led_ctrl = low;
			}
		} else {
			dbprintf("Button pressed!\n");
		}

		gpio_set_output(GPIO_ARD_D13, led_ctrl);
		sleep(MSECS(500));
	}

	return 0;
}
