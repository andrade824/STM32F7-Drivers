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
#define LCD_CONFIG_HSYNC     1U
#define LCD_CONFIG_VSYNC     1U
#define LCD_CONFIG_HORIZ_BP  43U
#define LCD_CONFIG_VERT_BP   12U
#define LCD_CONFIG_WIDTH     480U
#define LCD_CONFIG_HEIGHT    272U
#define LCD_CONFIG_HORIZ_FP  8U
#define LCD_CONFIG_VERT_FP   4U
#define LCD_CONFIG_HSYNC_POL ACTIVE_LOW
#define LCD_CONFIG_VSYNC_POL ACTIVE_LOW
#define LCD_CONFIG_DE_POL    ACTIVE_LOW
#define LCD_CONFIG_CLK_POL   ACTIVE_LOW

#endif
#endif
