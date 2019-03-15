#include "config.h"
#include "debug.h"
#include "fmc_sdram.h"
#include "gpio.h"
#include "graphics.h"
#include "interrupt.h"
#include "sdmmc.h"
#include "status.h"
#include "system.h"
#include "usart.h"

#include "registers/fmc_sdram_reg.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if 0
const uint32_t draw_buffer = SDRAM_BASE;
const uint32_t render_buffer = SDRAM_BASE + (LCD_CONFIG_WIDTH * LCD_CONFIG_HEIGHT * LCD_CONFIG_PIXEL_SIZE);
#endif

#if 0
status_t draw_random_rect(void) {
	uint8_t red = rand() % 256;
	uint8_t blue = rand() % 256;
	uint8_t green = rand() % 256;

	uint16_t x0 = rand() % (gfx_width() / 2);
	uint16_t y0 = rand() % (gfx_height() / 2);
	uint16_t x1 = x0 + (rand() % (gfx_width() / 2));
	uint16_t y1 = y0 + (rand() % (gfx_height() / 2));

	ABORT_IF_NOT(gfx_draw_rect(x0, y0, x1, y1, PIXEL(red, green, blue)));

	return Success;
}
#endif

status_t run(void)
{
	ABORT_IF_NOT(init_system());

	dbprintf("System Initialized\n");
#if 0
	ABORT_IF_NOT(init_fmc_sdram());
	ABORT_IF_NOT(init_graphics(render_buffer, draw_buffer));

	gfx_clear_screen(PIXEL(0,0,0));
	ABORT_IF_NOT(gfx_text_set_cursor(20, 10));
	gfx_text_foreground(PIXEL(0, 255, 0));
	gfx_text_background(PIXEL(0, 0, 0));

	gfx_swap_buffers();
#endif

#if 0
	for(int i = 0; i < 3; i++) {
		dbprintf("Beginning Memcheck...\n");
		for (uint32_t uwIndex = 0; uwIndex < 0x00800000; uwIndex += 4)
		{
			*(volatile uint32_t*) (SDRAM_BASE + uwIndex) = uwIndex;
		}

		for (uint32_t uwIndex = 0; uwIndex < 0x00800000; uwIndex += 4)
		{
			uint32_t value = *(volatile uint32_t*) (SDRAM_BASE + uwIndex);

			if(value != uwIndex)
			{
				dbprintf("Memcheck failure: value 0x%lx != index 0x%lx\n", value, uwIndex);
				gpio_set_output(GPIO_ARD_D13, high);
				while(1) { }
			}

			if(uwIndex % 0x10000 == 0) {
				dbprintf("Checkpoint... value 0x%lx -- index 0x%lx\n", value, uwIndex);
			}

		}
		dbprintf("Memcheck complete!!\n");
	}

#endif

#if 1
#define GPIO_B_USER GPIO_BTN_USER
#define GPIO_ARD_D13 GPIO_LED_USER
#endif

	ABORT_IF_NOT(gpio_request_input(GPIO_B_USER, GPIO_NO_PULL));
	ABORT_IF_NOT(gpio_request_output(GPIO_ARD_D13, low));

	/* Initialize the SDMMC module */
	ABORT_IF_NOT(gpio_request_alt(GPIO_USD_D0, AF12, GPIO_OSPEED_50MHZ));
	gpio_set_pullstate(GPIO_USD_D0, GPIO_PULL_UP);
	ABORT_IF_NOT(gpio_request_alt(GPIO_USD_D1, AF12, GPIO_OSPEED_50MHZ));
	gpio_set_pullstate(GPIO_USD_D1, GPIO_PULL_UP);
	ABORT_IF_NOT(gpio_request_alt(GPIO_USD_D2, AF12, GPIO_OSPEED_50MHZ));
	gpio_set_pullstate(GPIO_USD_D2, GPIO_PULL_UP);
	ABORT_IF_NOT(gpio_request_alt(GPIO_USD_D3, AF12, GPIO_OSPEED_50MHZ));
	gpio_set_pullstate(GPIO_USD_D3, GPIO_PULL_UP);
	ABORT_IF_NOT(gpio_request_alt(GPIO_USD_CLK, AF12, GPIO_OSPEED_50MHZ));
	ABORT_IF_NOT(gpio_request_alt(GPIO_USD_CMD, AF12, GPIO_OSPEED_50MHZ));
	gpio_set_pullstate(GPIO_USD_CMD, GPIO_PULL_UP);
	ABORT_IF_NOT(sdmmc_init());
	dbprintf("SDMMC appears to have initialized!\n");

#if 0
	uint8_t write_data[2048];
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 256; j++) {
			write_data[(i * 256) + j] = j;
		}
	}

	extern SdCard card;
	dbprintf("------Writing Data at %lu...-------\n", card.total_blocks - 5);
	sd_status_t status = sd_write_data(write_data, card.total_blocks - 5, 4);
	if(status != SD_SUCCESS) {
		ABORT("Here's the SD status for write %d\n", status);
	}


	uint8_t read_data[2048];
	status = sd_read_data(read_data, card.total_blocks - 5, 4);
	if(status != SD_SUCCESS) {
		ABORT("Here's the SD status for read %d\n", status);
	}

	dbprintf("------Checking Data at %lu...-------\n", card.total_blocks - 5);
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 256; j++) {
			const size_t index = (i * 256) + j;
			if(read_data[index] != write_data[index]) {
				ABORT("DATA MISMATCH AT INDEX %d, %d != %d\n", index, read_data[index], write_data[index]);
			}
		}
	}

	dbprintf("Data verified correctly!\n");
#endif

#if 1
	uint8_t data[2048];
	dbprintf("Reading from LBA: %lu\n", sd_get_card_info().total_blocks - 5);
	sd_status_t status = sd_read_data(data, sd_get_card_info().total_blocks - 5, 4);
	if(status != SD_SUCCESS) {
		ABORT("Here's the SD status %d\n", status);
	}

	for(int i = 0; i < 2048; ++i) {
		if(i % 256 == 0) {
			dbprintf("\n\n");
		}
		dbprintf("0x%X ", data[i]);
	}
	dbprintf("\n");
#endif

#if 0
	uint8_t data[512];
	sd_status_t status = sd_read_data(data, 0, 1);
	if(status != SD_SUCCESS) {
		ABORT("Here's the SD status %d\n", status);
	}
	printf("------MBR Partition:-------\n");
	for(int i = 0; i < 512; ++i) {
		dbprintf("0x%x ", data[i]);
	}
	dbprintf("\n");

	uint32_t fat32_lba = (data[457] << 24) | (data[456] << 16) | (data[455] << 8) | data[454];
	printf("Logical block address: 0x%lx\n", fat32_lba);
	status = sd_read_data(data, fat32_lba, 1);
	if(status != SD_SUCCESS) {
		ABORT("Here's the SD status %d\n", status);
	}
	printf("------FAT32 Partition:-------\n");
	for(int i = 0; i < 512; ++i) {
		dbprintf("0x%x ", data[i]);
	}
	dbprintf("\n");
#endif

#if 0
	/* Initialize the USART module */
	ABORT_IF_NOT(gpio_request_alt(GPIO_PC6, AF8, GPIO_OSPEED_4MHZ));
	ABORT_IF_NOT(gpio_request_alt(GPIO_PC7, AF8, GPIO_OSPEED_4MHZ));
	ABORT_IF_NOT(usart_init(USART6, 115200, USART_8_DATA, USART_1_STOP));
	usart_enable_rx(USART6, true);
	usart_enable_tx(USART6, true);
	usart_send(USART6, (uint8_t*)"Hello There!\r\n", 14);
	uint8_t ascii = 0;
#endif

	DigitalState led_ctrl = low;

#if 0
	srand(37); // Number chosen randomly through multiple dice throws.

	uint16_t x = 0;
	const uint16_t y = 100;
	const uint16_t rect_width = 50;
	const uint16_t rect_height = 50;
#endif

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

#if 0
		ascii = usart_receive(USART6);
		dbprintf("%d\n", ascii);
		usart_send_byte(USART6, ascii);
		if(ascii >= 32 && ascii < 127) {
			ABORT_IF_NOT(gfx_draw_char(ascii));
		}
#endif

#if 0
		gfx_text_foreground(PIXEL(255, 0, 0));
		ABORT_IF_NOT(gfx_draw_text("Merry "));
		gfx_text_foreground(PIXEL(0, 255, 0));
		ABORT_IF_NOT(gfx_draw_text("Christmas! "));
#endif

#if 0
		gfx_clear_screen(PIXEL(255, 255, 255));
		draw_random_rect();
		gfx_draw_rect(0, 0, 50, 50, PIXEL(0, 0, 255));
		gfx_draw_rect(100, 170, 150, 220, PIXEL(0, 255, 0));
		gfx_draw_rect(x, y, x + rect_width, y + rect_height, PIXEL(255, 0, 0));

		x = ((x + rect_width + 1) >= gfx_width()) ? 0 : x + 1;

		/**
		 * With CLEAR_SCREEN: 31.8ms per frame (~31fps)
		 * Without CLEAR_SCREEN: 16.6ms per frame (~60fps)
		 */
#endif

#if 0
		/* Wait for the DMA transfer to complete. */
		gfx_swap_buffers();
#endif

	}

	return Success;
}

int main(void)
{
	if(!run())
	{
		while(1)
		{ }
	}

	return 0;
}
