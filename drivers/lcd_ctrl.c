/**
 * @author Devon Andrade
 * @created 6/21/2018
 *
 * Definitions and functions used to manipulate the LCD Controller [18].
 */
#ifdef INCLUDE_SDRAM_DRIVER

#include "debug.h"
#include "gpio.h"
#include "lcd_ctrl.h"
#include "registers/lcd_ctrl_reg.h"
#include "registers/rcc_reg.h"
#include "status.h"

/**
 * Initialize the LCD Controller.
 */
status_t init_lcd_ctrl(void)
{
    /**
     * Request all of the required GPIOs.
     */
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R0, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R1, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R2, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R3, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R4, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R5, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R6, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R7, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));

    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G0, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G1, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G2, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G3, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G4, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G5, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G6, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G7, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));

    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B0, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B1, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B2, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B3, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B4, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B5, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B6, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B7, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));

    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_CLK, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_HSYNC, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_VSYNC, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_DE, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_BL_CTRL, LCD_ALT_FUNC, GPIO_OSPEED_25MHZ));

    /**
     * Enable the LCD APB2 clock.
     */
    SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_LTDCEN());
    __asm("dsb");

    return Success;
}

#endif