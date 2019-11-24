/**
 * @author Devon Andrade
 * @created 12/23/2018
 *
 * A single software module used to draw graphics onto the STM32F7 Discovery
 * board's LCD module. This module encapsulates the LCD and 2D DMA controllers.
 */
#pragma once

#include "config.h"

#include <stdint.h>

/* The total size of a framebuffer in bytes. */
#define FRAMEBUFFER_SIZE (LCD_CONFIG_WIDTH * LCD_CONFIG_HEIGHT * LCD_CONFIG_PIXEL_SIZE)

/**
 * Create a compact pixel value out of three separate color values.
 */
#if ENABLE_LCD_GRAPHICS
#if LCD_CONFIG_PIXEL_FORMAT == PF_ARGB8888
#define PIXEL(r,g,b) (0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF))
#elif LCD_CONFIG_PIXEL_FORMAT == PF_RGB565
#define PIXEL(r,g,b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
#else
#error Unsupported pixel format selected.
#endif /* LCD_CONFIG_PIXEL_FORMAT */
#endif /* ENABLE_LCD_GRAPHICS */

void gfx_init(uint32_t frontbuf, uint32_t backbuf);

void gfx_swap_buffers(void);

void gfx_set_pixel(uint16_t col, uint16_t row, uint32_t color);
void gfx_draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
void gfx_clear_screen(uint32_t color);

void gfx_text_set_cursor(uint8_t col, uint8_t row);
void gfx_text_foreground(uint32_t color);
void gfx_text_background(uint32_t color);
void gfx_draw_char(char ascii);
void gfx_draw_text(char *string);
void gfx_text_scroll_line(void);

uint16_t gfx_width(void);
uint16_t gfx_height(void);
uint8_t gfx_pixel_size(void);
uint8_t gfx_num_chars(void);
uint8_t gfx_num_lines(void);
