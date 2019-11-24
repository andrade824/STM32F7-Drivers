/**
 * @author Devon Andrade
 * @created 12/23/2018
 *
 * A single software module used to draw graphics onto the STM32F7 Discovery
 * board's LCD module. This module encapsulates the LCD and 2D DMA controllers.
 */
#ifdef INCLUDE_GRAPHICS_MODULE

#if !defined(INCLUDE_DMA2D_DRIVER) || !defined(INCLUDE_LCD_CTRL_DRIVER) || !defined(INCLUDE_FONT_TABLE)
#error Need to include the DMA2D, LCD Controller, and Font Table drivers for graphics module support.
#endif

#include "config.h"
#include "debug.h"
#include "dma2d.h"
#include "font.h"
#include "graphics.h"
#include "lcd_ctrl.h"

#include "registers/lcd_ctrl_reg.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* Number of characters per line. */
#define NUM_CHARS (LCD_CONFIG_WIDTH / (FONT_WIDTH + 1U))

/* Number of lines of text that can fill the screen. */
#define NUM_LINES (LCD_CONFIG_HEIGHT / (FONT_HEIGHT + 1U))

/* Set true to trigger a DMA copy to occur. */
static volatile bool trigger_dma_copy = false;

/* The framebuffer that the LCD controller reads from. */
static uint32_t frontbuffer = 0;

/* The framebuffer that the application draws into. */
static uint32_t backbuffer = 0;

/* Text foreground and background color. */
static uint32_t foreground_color = PIXEL(255, 255, 255);
static uint32_t background_color = PIXEL(0, 0, 0);

/* Text cursor location (in pixels). */
static uint16_t cursor_col = 0;
static uint16_t cursor_row = 0;

/**
 * The callback that gets called when a DMA transfer completes.
 */
static void dma2d_callback(void)
{
	trigger_dma_copy = false;
}

/**
 * The callback that gets triggered during every vertical blanking period.
 *
 * Causes the DMA2D to perform a transfer when one has been triggered (by
 * gfx_swap_buffers()) and there currently aren't any other DMA transfers
 * happening.
 */
static void vblank_callback(void)
{
	if(trigger_dma_copy && is_dma2d_complete()) {
		dma2d_mem_to_mem(backbuffer,
		                 frontbuffer,
		                 LCD_CONFIG_WIDTH,
		                 LCD_CONFIG_HEIGHT,
		                 &dma2d_callback);
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
void gfx_init(uint32_t frontbuf, uint32_t backbuf)
{
	frontbuffer = frontbuf;
	backbuffer = backbuf;

	dma2d_init();
	lcd_ctrl_init(frontbuffer, &vblank_callback);
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
void gfx_set_pixel(uint16_t col, uint16_t row, uint32_t color)
{
	ASSERT(col < LCD_CONFIG_WIDTH);
	ASSERT(row < LCD_CONFIG_HEIGHT);

	const uint32_t pixel_offset = (row * LCD_CONFIG_WIDTH + col) * LCD_CONFIG_PIXEL_SIZE;
	*(pixel_t*)(backbuffer + pixel_offset) = color;
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
void gfx_draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	ASSERT(x0 <= x1);
	ASSERT(y0 <= y1);
	ASSERT(x1 < LCD_CONFIG_WIDTH);
	ASSERT(y1 < LCD_CONFIG_HEIGHT);

	for(int row = y0; row <= y1; row++) {
		for (int col = x0; col <= x1; col++) {
			gfx_set_pixel(col, row, color);
		}
	}
}

/**
 * Clear the entire screen to the passed in color.
 *
 * @note This also resets the text cursor back to the top left.
 *
 * @param color The color to set the entire screen to.
 */
void gfx_clear_screen(uint32_t color)
{
	gfx_draw_rect(0, 0, LCD_CONFIG_WIDTH - 1, LCD_CONFIG_HEIGHT - 1, color);

	cursor_col = 0;
	cursor_row = 0;
}

/**
 * Set the location of the text rendering cursor.
 *
 * @param col The cursor x-coordinate (in characters).
 * @param row The cursor y-coordinate (in lines).
 */
void gfx_text_set_cursor(uint8_t col, uint8_t row)
{
	ASSERT(col < NUM_CHARS);
	ASSERT(row < NUM_LINES);

	cursor_col = col * (FONT_WIDTH + 1);
	cursor_row = row * (FONT_HEIGHT + 1);
}

/**
 * Set the text foreground color.
 *
 * @param color The new text foreground color.
 */
void gfx_text_foreground(uint32_t color)
{
	foreground_color = color;
}

/**
 * Set the text background color.
 *
 * @param color The new text background color.
 */
void gfx_text_background(uint32_t color)
{
	background_color = color;
}

/**
 * Render an ASCII character at the current cursor location. The cursor will
 * automatically be moved to the next character location. If the end of the
 * screen is reached, a new line will be inserted at the bottom and the rest
 * of the screen will be scrolled up.
 *
 * @param ascii The ASCII character to draw.
 */
void gfx_draw_char(char ascii)
{
	ASSERT(ascii >= FONT_ASCII_OFFSET);
	ASSERT(ascii < (FONT_ASCII_OFFSET + FONT_TOTAL_CHARS));

	const uint16_t font_index = FONT_ASCII_INDEX(ascii);

	for(uint8_t row = 0; row < (FONT_HEIGHT + 1); ++row) {
		/* Each line of a font character is broken into 8-bit chunks */
		for(uint8_t line_idx = 0; line_idx < FONT_LINE_LENGTH; ++line_idx) {
			/* The last line chunk may not be a full 8-bits, and also add a spacer pixel */
			const bool last_chunk = (line_idx == (FONT_LINE_LENGTH - 1));
			const uint8_t line_width = (last_chunk) ? (FONT_WIDTH & 7) + 1 : 8;

			/* Intentionally inject a spacer row after the character is printed. */
			const uint16_t font_idx = font_index + (row * FONT_LINE_LENGTH) + line_idx;
			uint8_t char_line = (row < FONT_HEIGHT) ? font_table[font_idx] : 0;

			for(uint8_t col = (line_idx * 8); col < ((line_idx * 8) + line_width); ++col) {
				const uint32_t color = (char_line & 0x80) ? foreground_color : background_color;
				gfx_set_pixel(cursor_col + col, cursor_row + row, color);
				char_line <<= 1;
			}
		}
	}

	/* Move to the next character on the line */
	cursor_col += FONT_WIDTH + 1;

	/* Check to see if the cursor needs to wrap to the next line. */
	if((cursor_col + FONT_WIDTH) >= LCD_CONFIG_WIDTH) {
		cursor_col = 0;
		cursor_row += FONT_HEIGHT + 1;

		if((cursor_row + FONT_HEIGHT) >= LCD_CONFIG_HEIGHT) {
			/**
			 * Reached end of the screen. Scroll the screen up by a line and set
			 * the cursor to the last line.
			 */
			gfx_text_scroll_line();
			cursor_row = (NUM_LINES - 1) * (FONT_HEIGHT + 1);
		}
	}
}

/**
 * Render an entire string of text at the current cursor location.
 *
 * @param str The string of text to render.
 */
void gfx_draw_text(char *str)
{
	for(int i = 0; str[i] != '\0'; ++i) {
		gfx_draw_char(str[i]);
	}
}

/**
 * Moves all text up by one line. This requires copying lines 1 through
 * NUM_LINES - 1 on top of line 0 and clearing out the bottom line.
 *
 * @note The cursor location does not change.
 */
void gfx_text_scroll_line(void)
{
	/* The number of bytes a single line of text occupies. */
	const uint32_t text_line_bytes = LCD_CONFIG_WIDTH *
	                                 LCD_CONFIG_PIXEL_SIZE *
	                                 (FONT_HEIGHT + 1);

	memmove((void*)backbuffer,
	        (void*)(backbuffer + text_line_bytes),
	        FRAMEBUFFER_SIZE - text_line_bytes);

	gfx_draw_rect(0,
	              (NUM_LINES - 1) * (FONT_HEIGHT + 1),
	              LCD_CONFIG_WIDTH - 1,
	              LCD_CONFIG_HEIGHT - 1,
	              background_color);
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

/**
 * Pixel size (in bytes) getter.
 */
uint8_t gfx_pixel_size(void)
{
	return LCD_CONFIG_PIXEL_SIZE;
}

/**
 * Number of characters per line getter.
 */
uint8_t gfx_num_chars(void)
{
	return NUM_CHARS;
}

/**
 * Number of lines of text that can fill the screen getter.
 */
uint8_t gfx_num_lines(void)
{
	return NUM_LINES;
}

#endif /* INCLUDE_GRAPHICS_MODULE */
