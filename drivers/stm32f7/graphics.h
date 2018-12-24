/**
 * @author Devon Andrade
 * @created 12/23/2018
 *
 * A single software module used to draw graphics onto the STM32F7 Discovery
 * board's LCD module. This module encapsulates the LCD and 2D DMA controllers.
 */
#ifdef INCLUDE_DMA2D_DRIVER
#ifdef INCLUDE_LCD_CTRL_DRIVER
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "status.h"

#include <stdint.h>

/**
 * Create a compact pixel value out of three separate color values.
 */
#define PIXEL(r,g,b) PIXEL_8888(r,g,b)
#define PIXEL_8888(r,g,b) (0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF))
#define PIXEL_565(r,g,b) (((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F))

status_t init_graphics(uint32_t frontbuf, uint32_t backbuf);

void gfx_swap_buffers(void);

status_t gfx_set_pixel(uint16_t col, uint16_t row, uint32_t color);
status_t gfx_draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
status_t gfx_clear_screen(uint32_t color);

uint16_t gfx_width(void);
uint16_t gfx_height(void);

#endif
#endif
#endif