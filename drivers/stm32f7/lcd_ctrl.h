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

#include "registers/lcd_ctrl_reg.h"

#include <stdint.h>

/**
 * Polarity settings used for HSPOL, VSPOL, DEPOL, and PCPOL in LTDC_GCR.
 */
typedef enum {
	ACTIVE_LOW = 0,
	ACTIVE_HIGH = 1
} LcdPolarity;

status_t init_lcd_ctrl(uint32_t framebuffer, void (*callback) (void));

#endif
#endif