/**
 * @author Devon Andrade
 * @created 6/21/2018
 *
 * Timings and settings used for the LCD on the STM32F7 Discovery board.
 */
#ifdef INCLUDE_LCD_CTRL_DRIVER
#ifndef LCD_TIMINGS_H
#define LCD_TIMINGS_H

#include "lcd_ctrl.h"

/**
 * Settings used to initialize the LCD used on the STM32F7 Discovery board.
 */
const LcdSettings default_lcd_settings = {
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

/**
 * SSCR
 * VSH: 9
 * HSW: 40
 *
 * BPCR
 * AVBP: 11
 * AHBP: 53
 *
 * AWCR
 * AAH: 283
 * AAW: 533
 *
 * TWCR
 * TOTALH: 285
 * TOTALW: 565
 *
 * They have DEN enabled in GCR, dithering set to "2" for all colors
 *
 * LIPCR->LIPOS set to zero???
 *
 * WHPCR
 * WHSTPOS: 54
 * WHSPPOS: 533
 *
 * WVPCR
 * WVSTPOS: 12
 * WVSPPOS: 283
 *
 * ARGB8888
 *
 * BFCR
 * BF2 = 0x7
 * BF1 = 0x6
 *
 * CFBAR->CFBADD -> 0xC02FF000
 *
 * CFBLR
 * CFBLL: 1923, 0x783
 * CFBP: 1920, 0x780
 *
 * CFBLRN->CFBLNBR = 0x110, 272
 */
#endif
#endif
