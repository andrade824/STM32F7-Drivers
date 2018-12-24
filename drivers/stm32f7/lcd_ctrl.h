/**
 * @author Devon Andrade
 * @created 6/21/2018
 *
 * Definitions and functions used to manipulate the LCD Controller [18].
 */
#ifdef INCLUDE_LCD_CTRL_DRIVER
#ifndef LCD_CTRL_H
#define LCD_CTRL_H

#include "gpio.h"
#include "interrupt.h"
#include "status.h"

#include <stdint.h>

/**
 * Polarity settings used for HSPOL, VSPOL, DEPOL, and PCPOL in LTDC_GCR.
 */
typedef enum {
	ACTIVE_LOW = 0,
	ACTIVE_HIGH = 1
} LcdPolarity;

/**
 * Settings used to initialize the LCD controller.
 */
typedef struct {
	uint16_t hsync;
	uint16_t vsync;

	uint16_t horiz_back_porch;
	uint16_t vert_back_porch;

	uint16_t active_width;
	uint16_t active_height;

	uint16_t horiz_front_porch;
	uint16_t vert_front_porch;

	LcdPolarity hsync_pol;
	LcdPolarity vsync_pol;
	LcdPolarity data_enable_pol;
	LcdPolarity pixel_clk_pol;
} LcdSettings;

status_t init_lcd_ctrl(LcdSettings lcd, uint32_t framebuffer);
status_t lcd_set_vblank_isr(ISR_Type vblank_isr);

#endif
#endif