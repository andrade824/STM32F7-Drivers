/**
 * @author Devon Andrade
 * @created 6/21/2018
 *
 * Definitions and functions used to manipulate the LCD Controller [18].
 */
#ifdef INCLUDE_LCD_CTRL_DRIVER

#include "debug.h"
#include "gpio.h"
#include "lcd_ctrl.h"
#include "registers/lcd_ctrl_reg.h"
#include "registers/rcc_reg.h"
#include "status.h"

/**
 * Initialize the LCD Controller.
 *
 * Default to one layer enabled that fills up the entire screen.
 */
status_t init_lcd_ctrl(LcdSettings lcd, uint32_t framebuffer)
{
    /**
     * Enable the LCD APB2 clock.
     */
    SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_LTDCEN());
    __asm("dsb");

    /**
     * Request all of the required GPIOs.
     */
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R0, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R1, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R2, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R3, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R4, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R5, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R6, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_R7, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));

    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G0, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G1, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G2, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G3, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G4, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G5, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G6, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_G7, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));

    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B0, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B1, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B2, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B3, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B4, LCD_ALT9_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B5, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B6, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_B7, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));

    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_CLK, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_HSYNC, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_VSYNC, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_alt(GPIO_LCD_DE, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ));
    ABORT_IF_NOT(gpio_request_output(GPIO_LCD_DISP, low));
    ABORT_IF_NOT(gpio_request_output(GPIO_LCD_BL_CTRL, low));
    gpio_set_otype(GPIO_LCD_BL_CTRL, GPIO_OPEN_DRAIN);

    /**
     * Set the LCD timing parameters.
     */
    const uint16_t vsync = lcd.vsync - 1;
    const uint16_t hsync = lcd.hsync - 1;
    SET_FIELD(LTDC->SSCR, SET_LTDC_SSCR_VSH(vsync) |
                          SET_LTDC_SSCR_HSW(hsync));

    const uint16_t accumulated_vbp = vsync + lcd.vert_back_porch;
    const uint16_t accumulated_hbp = hsync + lcd.horiz_back_porch;
    SET_FIELD(LTDC->BPCR, SET_LTDC_BPCR_AVBP(accumulated_vbp) |
                          SET_LTDC_BPCR_AHBP(accumulated_hbp));

    const uint16_t accumulated_active_height = accumulated_vbp + lcd.active_height;
    const uint16_t accumulated_active_width = accumulated_hbp + lcd.active_width;
    SET_FIELD(LTDC->AWCR, SET_LTDC_AWCR_AAH(accumulated_active_height) |
                          SET_LTDC_AWCR_AAW(accumulated_active_width));

    const uint16_t total_height = accumulated_active_height + lcd.vert_front_porch;
    const uint16_t total_width = accumulated_active_width + lcd.horiz_front_porch;
    SET_FIELD(LTDC->TWCR, SET_LTDC_TWCR_TOTALH(total_height) |
                          SET_LTDC_TWCR_TOTALW(total_width));

    /**
     * Set signal polarities.
     */
    SET_FIELD(LTDC->GCR, SET_LTDC_GCR_PCPOL(lcd.pixel_clk_pol) |
                         SET_LTDC_GCR_DEPOL(lcd.data_enable_pol) |
                         SET_LTDC_GCR_VSPOL(lcd.vsync_pol) |
                         SET_LTDC_GCR_HSPOL(lcd.hsync_pol) |
                         LTDC_GCR_DEN());

    /**
     * Set default background color.
     */
    SET_FIELD(LTDC->BCCR, SET_LTDC_BCCR_BCGREEN(0xFF) |
                          SET_LTDC_BCCR_BCBLUE(0) |
                          SET_LTDC_BCCR_BCRED(0));

    /**
     * Setup vertical blanking interrupt.
     */
    SET_FIELD(LTDC->LIPCR, SET_LTDC_LIPCR_LIPOS(lcd.active_height));
    // SET_FIELD(LTDC->IER, LTDC_IER_LIE());
    SET_FIELD(LTDC->IER, LTDC_IER_FUIE() | LTDC_IER_TERRIE());

    /**
     * Set the default layer settings (one layer that spans the entire active
     * display area).
     */
    SET_FIELD(LTDC_LAYER_REG(LAYER1)->WHPCR,
              SET_LTDC_LWHPCR_WHSTPOS(accumulated_hbp + 1) |
              SET_LTDC_LWHPCR_WHSPPOS(accumulated_active_width));

    SET_FIELD(LTDC_LAYER_REG(LAYER1)->WVPCR,
              SET_LTDC_LWVPCR_WVSTPOS(accumulated_vbp + 1) |
              SET_LTDC_LWVPCR_WVSPPOS(accumulated_active_height));

    SET_FIELD(LTDC_LAYER_REG(LAYER1)->PFCR, SET_LTDC_LPFCR_PF(PF_ARGB8888));

    SET_FIELD(LTDC_LAYER_REG(LAYER1)->CACR, SET_LTDC_LCACR_CONSTA(0xFF));

    CLEAR_FIELD(LTDC_LAYER_REG(LAYER1)->BFCR, LTDC_LBFCR_BF1() |
                                              LTDC_LBFCR_BF2());
    SET_FIELD(LTDC_LAYER_REG(LAYER1)->BFCR, SET_LTDC_LBFCR_BF1(0x4) |
                                            SET_LTDC_LBFCR_BF2(0x5));

    SET_FIELD(LTDC_LAYER_REG(LAYER1)->CFBAR, SET_LTDC_LCFBAR_CFBADD(framebuffer));

    /**
     * The screen width is multiplied by the sizeof a single pixel. If you
     * change pixel formats, you'll need to update this value as well.
     */
    SET_FIELD(LTDC_LAYER_REG(LAYER1)->CFBLR,
              SET_LTDC_LCFBLR_CFBLL((lcd.active_width * 4) + 3) |
              SET_LTDC_LCFBLR_CFBP(lcd.active_width * 4));

    SET_FIELD(LTDC_LAYER_REG(LAYER1)->CFBLNR,
              SET_LTDC_LCFBLNR_CFBLNBR(lcd.active_height));

    SET_FIELD(LTDC_LAYER_REG(LAYER1)->CR, LTDC_LCR_LEN());

    /**
     * Reload the LTDC registers immediately instead of waiting for vblank.
     */
    SET_FIELD(LTDC->SRCR, LTDC_SRCR_IMR());

    /**
     * Enable the LTDC controller.
     */
    SET_FIELD(LTDC->GCR, LTDC_GCR_LTDCEN());

    /**
     * Enable backlight and set the LCD_DISP pin high (which tells the LCD
     * module to start displaying data).
     */
    gpio_set_output(GPIO_LCD_BL_CTRL, high);
    gpio_set_output(GPIO_LCD_DISP, high);

    return Success;
}

#endif
