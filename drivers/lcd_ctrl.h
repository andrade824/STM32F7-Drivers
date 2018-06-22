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
#include "status.h"

/**
 * The GPIO alternate function for the LCD pins. Check section 3 of the
 * datasheet for more details.
 */
#define LCD_ALT_FUNC AF14

status_t init_lcd_ctrl(void);

#endif
#endif