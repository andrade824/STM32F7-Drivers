/**
 * @author Devon Andrade
 * @created 6/21/2018
 *
 * Timings and settings used for the LCD on the STM32F7 Discovery board.
 */
#ifdef INCLUDE_LCD_CTRL_DRIVER
#ifndef LCD_TIMINGS_H
#define LCD_TIMINGS_H

#include "drivers/lcd_ctrl.h"

/**
 * Settings used to initialize the LCD used on the STM32F7 Discovery board.
 */
LcdSettings default_lcd_settings = {
    .hsync = 1,
    .vsync = 1,
    .horiz_back_porch = 43,
    .vert_back_porch = 12,
    .active_width = 480,
    .active_height = 272,
    .horiz_front_porch = 8,
    .vert_front_porch = 4,
    .hsync_pol = ACTIVE_LOW,
    .vsync_pol = ACTIVE_LOW,
    .data_enable_pol = ACTIVE_LOW,
    .pixel_clk_pol = ACTIVE_LOW
};

#endif
#endif
