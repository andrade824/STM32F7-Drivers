#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "stm32f7_tests.h"
#include "os_tests.h"
#include "os/task.h"
#include "system.h"
#include "interrupt.h"

#include <string.h>

#define STACK_SIZE 512U
STATIC_TASK_ALLOC(task1, STACK_SIZE);
STATIC_TASK_ALLOC(task2, STACK_SIZE);

void task1_func(uint32_t param)
{
	while(1) {
		dbprintf("Task 1 called! %#lx\n", param);
		sleep(MSECS(500));
		set_next_task(&task2_task);
		sched_yield();
	}
}

void task2_func(uint32_t param)
{
	while(1) {
		dbprintf("Task 2 called! %#lx\n", param);
		sleep(MSECS(500));
		set_next_task(&task1_task);
		sched_yield();
	}
}

int main(void)
{
	system_init();
	dbprintf("System Initialized\n");

	mem_alloc_test();

	STATIC_TASK_CREATE(task1, STACK_SIZE, task1_func, (void*)(uintptr_t)0x111);
	STATIC_TASK_CREATE(task2, STACK_SIZE, task2_func, (void*)(uintptr_t)0x222);

	sched_begin();

	dbprintf("Running idle thread!\n");

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
			set_next_task(&task1_task);
			sched_yield();
		}

		gpio_set_output(GPIO_LED_USER, led_ctrl);
		sleep(MSECS(500));
	}

	return 0;
}
