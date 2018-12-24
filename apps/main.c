#include "config.h"
#include "debug.h"
#include "fmc_sdram.h"
#include "gpio.h"
#include "graphics.h"
#include "interrupt.h"
#include "status.h"
#include "system.h"

#include "registers/fmc_sdram_reg.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const uint32_t draw_buffer = SDRAM_BASE;
const uint32_t render_buffer = SDRAM_BASE + (LCD_CONFIG_WIDTH * LCD_CONFIG_HEIGHT * LCD_CONFIG_PIXEL_SIZE);

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

status_t run(void)
{
	ABORT_IF_NOT(init_system());

	dbprintf("System Initialized\n");

	ABORT_IF_NOT(init_fmc_sdram());
	ABORT_IF_NOT(init_graphics(render_buffer, draw_buffer));

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

	srand(37); // Number chosen randomly through multiple dice throws.

	ABORT_IF_NOT(gpio_request_input(GPIO_B_USER, GPIO_NO_PULL));
	ABORT_IF_NOT(gpio_request_output(GPIO_ARD_D13, low));

	DigitalState led_ctrl = low;

	uint16_t x = 0;
	const uint16_t y = 100;
	const uint16_t rect_width = 50;
	const uint16_t rect_height = 50;

	while(1)
	{
		if(led_ctrl == low)
			led_ctrl = high;
		else
			led_ctrl = low;

		gpio_set_output(GPIO_ARD_D13, led_ctrl);

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

		/* Wait for the DMA transfer to complete. */
		gfx_swap_buffers();
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