#include "config.h"
#include "debug.h"
#include "fat.h"
#include "gpio.h"
#include "status.h"
#include "stm32f7_tests.h"
#include "system.h"

int main(void)
{
	init_system();
	dbprintf("System Initialized\n");

#if 1
	/* Initialize the SDMMC module */
	gpio_request_alt(GPIO_USD_D0, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D0, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D1, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D1, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D2, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D2, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D3, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D3, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_CLK, AF12, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_USD_CMD, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_CMD, GPIO_PULL_UP);
	ABORT_IF_NOT(sdmmc_init());
	dbprintf("SDMMC appears to have initialized!\n");

	fat_ops_t ops = {
		.total_size = sd_get_card_info().total_size,
		.total_sectors = sd_get_card_info().total_blocks,
		&sd_read_data,
		&sd_write_data
	};
	ABORT_IF_NOT(fat_init(ops));
#else
	sd_read_mbr_test();
#endif

#ifdef config_stm32f7_dev_board
#define GPIO_B_USER GPIO_BTN_USER
#define GPIO_ARD_D13 GPIO_LED_USER
#endif

	gpio_request_input(GPIO_B_USER, GPIO_NO_PULL);
	gpio_request_output(GPIO_ARD_D13, low);

	DigitalState led_ctrl = low;

	while(1) {
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
