#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "stm32f7_tests.h"
#include "os_tests.h"
#include "system.h"
#include "interrupt.h"

#include <string.h>

void pendsv_handler(void)
{
	dbprintf("PENDSV WAS TRIGGERED\n");

	/* Trigger a MemManage fault by trying to execute XN memory. */
	// void (*dummy_func)(int);
	// dummy_func = (void (*)(int))0xFFFFFF00;
	// dummy_func(5);

	// /* Trigger a Usage fault by accessing an unaligned address. */
	*(volatile uint32_t*)0xFFFFFFFF = 0x5;

	/* Trigger a Bus fault by accessing a reserved address. */
	// *(volatile uint32_t*)0x2004FF00 = 0x5;
}

int main(void)
{
	system_init();
	dbprintf("System Initialized\n");

	mem_alloc_test();

	intr_register_pendsv(&pendsv_handler, LOWEST_INTR_PRIORITY);
	intr_trigger_pendsv();

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

	return 0;
}
