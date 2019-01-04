#include "config.h"
#include "debug.h"
#include "fmc_sdram.h"
#include "gpio.h"
#include "graphics.h"
#include "interrupt.h"
#include "status.h"
#include "system.h"

#include "registers/fmc_sdram_reg.h"
#include "registers/usart_reg.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const uint32_t draw_buffer = SDRAM_BASE;
const uint32_t render_buffer = SDRAM_BASE + (LCD_CONFIG_WIDTH * LCD_CONFIG_HEIGHT * LCD_CONFIG_PIXEL_SIZE);

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

void test_usart_reg(void)
{
	dbprintf("USART_CR1_UE: 0x%lx\n", USART_CR1_UE());
	dbprintf("USART_CR1_RE: 0x%lx\n", USART_CR1_RE());
	dbprintf("USART_CR1_TE: 0x%lx\n", USART_CR1_TE());
	dbprintf("USART_CR1_IDLEIE: 0x%lx\n", USART_CR1_IDLEIE());
	dbprintf("USART_CR1_RXNEIE: 0x%lx\n", USART_CR1_RXNEIE());
	dbprintf("USART_CR1_TCIE: 0x%lx\n", USART_CR1_TCIE());
	dbprintf("USART_CR1_TXEIE: 0x%lx\n", USART_CR1_TXEIE());
	dbprintf("USART_CR1_PEIE: 0x%lx\n", USART_CR1_PEIE());
	dbprintf("USART_CR1_PS: 0x%lx\n", USART_CR1_PS());
	dbprintf("USART_CR1_PCE: 0x%lx\n", USART_CR1_PCE());
	dbprintf("USART_CR1_WAKE: 0x%lx\n", USART_CR1_WAKE());
	dbprintf("USART_CR1_M0: 0x%lx\n", USART_CR1_M0());
	dbprintf("USART_CR1_MME: 0x%lx\n", USART_CR1_MME());
	dbprintf("USART_CR1_CMIE: 0x%lx\n", USART_CR1_CMIE());
	dbprintf("USART_CR1_OVER8: 0x%lx\n", USART_CR1_OVER8());
	dbprintf("USART_CR1_DEDT: 0x%lx\n", USART_CR1_DEDT());
	dbprintf("USART_CR1_DEAT: 0x%lx\n", USART_CR1_DEAT());
	dbprintf("USART_CR1_RTOIE: 0x%lx\n", USART_CR1_RTOIE());
	dbprintf("USART_CR1_EOBIE: 0x%lx\n", USART_CR1_EOBIE());
	dbprintf("USART_CR1_M1: 0x%lx\n", USART_CR1_M1());
	dbprintf("GET_USART_CR1_UE: 0x%lx\n", GET_USART_CR1_UE(USART1->CR1));
	dbprintf("SET_USART_CR1_DEAT: 0x%lx\n", SET_USART_CR1_DEAT(0xFFFF));
}

status_t run(void)
{
	ABORT_IF_NOT(init_system());

	dbprintf("System Initialized\n");
	test_usart_reg();
	ABORT_IF_NOT(init_fmc_sdram());
	ABORT_IF_NOT(init_graphics(render_buffer, draw_buffer));

	gfx_clear_screen(PIXEL(0,0,0));
	ABORT_IF_NOT(gfx_text_set_cursor(20, 10));
	gfx_text_foreground(PIXEL(0, 255, 0));
	gfx_text_background(PIXEL(0, 0, 0));

	gfx_swap_buffers();

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

#if 0
	uint16_t x = 0;
	const uint16_t y = 100;
	const uint16_t rect_width = 50;
	const uint16_t rect_height = 50;
#endif

	while(1)
	{
		if(led_ctrl == low)
			led_ctrl = high;
		else
			led_ctrl = low;

		gpio_set_output(GPIO_ARD_D13, led_ctrl);
		sleep(MSECS(100));
		gfx_text_foreground(PIXEL(255, 0, 0));
		ABORT_IF_NOT(gfx_draw_text("Merry "));
		gfx_text_foreground(PIXEL(0, 255, 0));
		ABORT_IF_NOT(gfx_draw_text("Christmas! "));

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
