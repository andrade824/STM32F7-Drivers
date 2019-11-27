#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "stm32f7_tests.h"
#include "system.h"

#include <string.h>

int main(void)
{
	system_init();
	dbprintf("System Initialized\n");

#if 1
	jstk_test();
#else
	fat_dump_file_test("test2.txt");
#endif

#ifdef config_stm32f7_dev_board
#define GPIO_B_USER GPIO_BTN_USER
#define GPIO_ARD_D13 GPIO_LED_USER
#endif

	gpio_request_input(GPIO_B_USER, GPIO_NO_PULL);
	gpio_request_output(GPIO_ARD_D13, GPIO_LOW);

	DigitalState led_ctrl = GPIO_LOW;

	while(1) {
		if(gpio_get_input(GPIO_B_USER) == GPIO_LOW) {
			if(led_ctrl == GPIO_LOW) {
				led_ctrl = GPIO_HIGH;
			} else {
				led_ctrl = GPIO_LOW;
			}
		} else {
			dbprintf("Button pressed!\n");
		}

		gpio_set_output(GPIO_ARD_D13, led_ctrl);
		sleep(MSECS(500));
	}

	return 0;
}
