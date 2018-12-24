/**
 * @author Devon Andrade
 * @created 12/23/2018
 *
 * A single software module used to draw graphics onto the STM32F7 Discovery
 * board's LCD module. This module encapsulates the LCD and 2D DMA controllers.
 */
#ifdef INCLUDE_DMA2D_DRIVER
#ifdef INCLUDE_LCD_CTRL_DRIVER

#include "debug.h"
#include "dma2d.h"
#include "graphics.h"
#include "lcd_ctrl.h"
#include "status.h"

#include "registers/lcd_ctrl_reg.h"

/**
 * Set true to trigger a DMA copy to occur.
 */
static volatile bool trigger_dma_copy = false;

/* The framebuffer that the LCD controller reads from. */
static uint32_t frontbuffer = 0;

/* The framebuffer that the application draws into. */
static uint32_t backbuffer = 0;

/**
 * The callback that gets called when a DMA transfer completes.
 */
static void dma2d_callback(void)
{
	trigger_dma_copy = false;
}

/**
 * Triggered during every vertical blanking period.
 *
 * Causes the DMA2D to perform a transfer when one has been triggered and there
 * currently aren't any other DMA transfers happening.
 */
static void vblank_isr(void)
{
	if(GET_LTDC_ISR_LIF(LTDC->ISR)) {
		SET_FIELD(LTDC->ICR, LTDC_ICR_CLIF());

		if(trigger_dma_copy && is_dma2d_complete()) {
			dma2d_mem_to_mem(backbuffer,
							 frontbuffer,
							 LCD_CONFIG_WIDTH,
							 LCD_CONFIG_HEIGHT,
							 &dma2d_callback);
		}
	}
}

/**
 * Initialize the graphics subsystem (which encompasses the DMA2D and LCD
 * controller modules).
 *
 * @param frontbuf Starting address of the framebuffer that the LCD controller
 *                 reads from.
 * @param backbuf  Starting address of the framebuffer that the application
 *                 draws into.
 */
status_t init_graphics(uint32_t frontbuf, uint32_t backbuf)
{
	frontbuffer = frontbuf;
	backbuffer = backbuf;

	ABORT_IF_NOT(init_dma2d());

	const LcdSettings lcd_settings = {
		.hsync =             LCD_CONFIG_HSYNC,
		.vsync =             LCD_CONFIG_VSYNC,
		.horiz_back_porch =  LCD_CONFIG_HORIZ_BP,
		.vert_back_porch =   LCD_CONFIG_VERT_BP,
		.active_width =      LCD_CONFIG_WIDTH,
		.active_height =     LCD_CONFIG_HEIGHT,
		.horiz_front_porch = LCD_CONFIG_HORIZ_FP,
		.vert_front_porch =  LCD_CONFIG_VERT_FP,
		.hsync_pol =         LCD_CONFIG_HSYNC_POL,
		.vsync_pol =         LCD_CONFIG_VSYNC_POL,
		.data_enable_pol =   LCD_CONFIG_DE_POL,
		.pixel_clk_pol =     LCD_CONFIG_CLK_POL
	};

	ABORT_IF_NOT(init_lcd_ctrl(lcd_settings, frontbuffer));
	ABORT_IF_NOT(lcd_set_vblank_isr(vblank_isr));

	return Success;
}

/**
 * Trigger the DMA to update the frontbuffer during the next vertical blanking
 * period. Will block until that operation completes.
 */
void gfx_swap_buffers(void)
{
	trigger_dma_copy = true;

	/* Wait until the DMA finishes the transfer. */
	while(trigger_dma_copy) { }
}

/**
 * Set a pixel to a specific color.
 *
 * @param col The X-coordinate of the pixel to set.
 * @param row the Y-coordinate of the pixel to set.
 * @param color The color to set the pixel to.
 */
status_t gfx_set_pixel(uint16_t col, uint16_t row, uint32_t color)
{
	ABORT_IF(col >= LCD_CONFIG_WIDTH);
	ABORT_IF(row >= LCD_CONFIG_HEIGHT);

	const uint32_t pixel_offset = (row * LCD_CONFIG_WIDTH + col) * LCD_CONFIG_PIXEL_SIZE;
	*(uint32_t*)(backbuffer + pixel_offset) = color;

	return Success;
}

/**
 * Draw a rectangle into the backbuffer.
 *
 * @note All coordinates are inclusive.
 *
 * @param x0 Top-left X-coordinate of rectangle.
 * @param y0 Top-left Y-coordinate of rectangle.
 * @param x1 Bottom-right X-coordinate of rectangle.
 * @param y1 Bottom-right Y-coordinate of rectangle.
 * @param color The color of the rectangle.
 */
status_t gfx_draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	ABORT_IF(x0 > x1);
	ABORT_IF(y0 > y1);
	ABORT_IF(x1 >= LCD_CONFIG_WIDTH);
	ABORT_IF(y1 >= LCD_CONFIG_HEIGHT);

	for(int row = y0; row <= y1; row++) {
		for (int col = x0; col <= x1; col++) {
			gfx_set_pixel(col, row, color);
		}
	}

	return Success;
}

/**
 * Clear the entire screen to the passed in color.
 *
 * @param color The color to set the entire screen to.
 */
status_t gfx_clear_screen(uint32_t color)
{
	ABORT_IF_NOT(gfx_draw_rect(0, 0, LCD_CONFIG_WIDTH - 1, LCD_CONFIG_HEIGHT - 1, color));

	return Success;
}

/**
 * LCD Width getter.
 */
uint16_t gfx_width(void)
{
	return LCD_CONFIG_WIDTH;
}

/**
 * LCD Height getter.
 */
uint16_t gfx_height(void)
{
	return LCD_CONFIG_HEIGHT;
}

#endif
#endif