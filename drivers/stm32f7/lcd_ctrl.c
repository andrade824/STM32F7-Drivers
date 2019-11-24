/**
 * @author Devon Andrade
 * @created 6/21/2018
 *
 * Definitions and functions used to manipulate the LCD Controller [18].
 *
 * Don't call into this module directly, use the dedicated "graphics" module.
 */
#ifdef INCLUDE_LCD_CTRL_DRIVER

#include "debug.h"
#include "gpio.h"
#include "interrupt.h"
#include "lcd_ctrl.h"

#include "registers/lcd_ctrl_reg.h"
#include "registers/rcc_reg.h"

#include <stdint.h>

/**
 * The GPIO alternate functions for the LCD pins. Check section 3 of the
 * datasheet for more details.
 */
#define LCD_ALT14_FUNC AF14
#define LCD_ALT9_FUNC AF9

/* The callback function to call when a vertical blanking period occurs. */
static void (*vblank_callback) (void) = NULL;

/**
 * Triggered during every vertical blanking period.
 */
static void vblank_isr(void)
{
	if(GET_LTDC_ISR_LIF(LTDC->ISR)) {
		SET_FIELD(LTDC->ICR, LTDC_ICR_CLIF());

		if(vblank_callback != NULL) {
			vblank_callback();
		}
	}
}

/**
 * Default error ISR that spins when an error occurs.
 */
void lcd_ctrl_error_isr(void)
{
	if(GET_LTDC_ISR_FUIF(LTDC->ISR)) {
		SET_FIELD(LTDC->ICR, LTDC_ICR_CFUIF());
		ABORT("LCD Error: FIFO Underrun error\n");
	}

	if(GET_LTDC_ISR_TERRIF(LTDC->ISR)) {
		SET_FIELD(LTDC->ICR, LTDC_ICR_CTERRIF());
		ABORT("LCD Error: Transfer error\n");
	}
}

/**
 * Initialize the LCD Controller.
 *
 * Default to one layer enabled that fills up the entire screen.
 *
 * @param framebuffer The framebuffer to render the screen from.
 * @param callback A callback function to trigger during every vertical blanking
 *                 period (or NULL for no callback).
 */
void lcd_ctrl_init(uint32_t framebuffer, void (*callback) (void))
{
	/* Enable the LCD APB2 clock. */
	SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_LTDCEN());
	__asm("dsb");

	/* Request all of the required GPIOs. */
	gpio_request_alt(GPIO_LCD_R0, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_R1, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_R2, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_R3, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_R4, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_R5, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_R6, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_R7, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);

	gpio_request_alt(GPIO_LCD_G0, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_G1, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_G2, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_G3, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_G4, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_G5, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_G6, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_G7, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);

	gpio_request_alt(GPIO_LCD_B0, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_B1, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_B2, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_B3, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_B4, LCD_ALT9_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_B5, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_B6, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_B7, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);

	gpio_request_alt(GPIO_LCD_CLK, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_HSYNC, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_VSYNC, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_LCD_DE, LCD_ALT14_FUNC, GPIO_OSPEED_50MHZ);
	gpio_request_output(GPIO_LCD_DISP, GPIO_LOW);
	gpio_request_output(GPIO_LCD_BL_CTRL, GPIO_LOW);
	gpio_set_otype(GPIO_LCD_BL_CTRL, GPIO_OPEN_DRAIN);

	/* Set the LCD timing parameters. */
	const uint16_t vsync = LCD_CONFIG_VSYNC - 1;
	const uint16_t hsync = LCD_CONFIG_HSYNC - 1;
	SET_FIELD(LTDC->SSCR, SET_LTDC_SSCR_VSH(vsync) |
	                      SET_LTDC_SSCR_HSW(hsync));

	const uint16_t accumulated_vbp = vsync + LCD_CONFIG_VERT_BP;
	const uint16_t accumulated_hbp = hsync + LCD_CONFIG_HORIZ_BP;
	SET_FIELD(LTDC->BPCR, SET_LTDC_BPCR_AVBP(accumulated_vbp) |
	                      SET_LTDC_BPCR_AHBP(accumulated_hbp));

	const uint16_t accumulated_active_height = accumulated_vbp + LCD_CONFIG_HEIGHT;
	const uint16_t accumulated_active_width = accumulated_hbp + LCD_CONFIG_WIDTH;
	SET_FIELD(LTDC->AWCR, SET_LTDC_AWCR_AAH(accumulated_active_height) |
	                      SET_LTDC_AWCR_AAW(accumulated_active_width));

	const uint16_t total_height = accumulated_active_height + LCD_CONFIG_VERT_FP;
	const uint16_t total_width = accumulated_active_width + LCD_CONFIG_HORIZ_FP;
	SET_FIELD(LTDC->TWCR, SET_LTDC_TWCR_TOTALH(total_height) |
	                      SET_LTDC_TWCR_TOTALW(total_width));

	/* Set signal polarities. */
	SET_FIELD(LTDC->GCR, SET_LTDC_GCR_PCPOL(LCD_CONFIG_CLK_POL) |
	                     SET_LTDC_GCR_DEPOL(LCD_CONFIG_DE_POL) |
	                     SET_LTDC_GCR_VSPOL(LCD_CONFIG_VSYNC_POL) |
	                     SET_LTDC_GCR_HSPOL(LCD_CONFIG_HSYNC_POL) |
	                     LTDC_GCR_DEN());

	/* Set default background color. */
	SET_FIELD(LTDC->BCCR, SET_LTDC_BCCR_BCGREEN(0) |
	                      SET_LTDC_BCCR_BCBLUE(0) |
	                      SET_LTDC_BCCR_BCRED(0));

	/* Setup LCD vblank and error interrupts. */
	vblank_callback = callback;
	if(vblank_callback) {
		interrupt_request(LTDC_IRQn, vblank_isr);
		SET_FIELD(LTDC->LIPCR, SET_LTDC_LIPCR_LIPOS(LCD_CONFIG_HEIGHT));
		SET_FIELD(LTDC->IER, LTDC_IER_LIE());
	}

	interrupt_request(LTDC_ER_IRQn, lcd_ctrl_error_isr);
	SET_FIELD(LTDC->IER, LTDC_IER_FUIE() | LTDC_IER_TERRIE());

	/**
	 * Set the default layer settings (one layer that spans the entire active
	 * display area).
	 */
	SET_FIELD(LTDC_LAYER_REG(LTDC_LAYER1)->WHPCR,
	          SET_LTDC_LWHPCR_WHSTPOS(accumulated_hbp + 1) |
	          SET_LTDC_LWHPCR_WHSPPOS(accumulated_active_width));

	SET_FIELD(LTDC_LAYER_REG(LTDC_LAYER1)->WVPCR,
	          SET_LTDC_LWVPCR_WVSTPOS(accumulated_vbp + 1) |
	          SET_LTDC_LWVPCR_WVSPPOS(accumulated_active_height));

	SET_FIELD(LTDC_LAYER_REG(LTDC_LAYER1)->PFCR, SET_LTDC_LPFCR_PF(LCD_CONFIG_PIXEL_FORMAT));

	SET_FIELD(LTDC_LAYER_REG(LTDC_LAYER1)->CACR, SET_LTDC_LCACR_CONSTA(0xFF));

	CLEAR_FIELD(LTDC_LAYER_REG(LTDC_LAYER1)->BFCR, LTDC_LBFCR_BF1() |
	                                               LTDC_LBFCR_BF2());
	SET_FIELD(LTDC_LAYER_REG(LTDC_LAYER1)->BFCR, SET_LTDC_LBFCR_BF1(0x4) |
	                                             SET_LTDC_LBFCR_BF2(0x5));

	SET_FIELD(LTDC_LAYER_REG(LTDC_LAYER1)->CFBAR, SET_LTDC_LCFBAR_CFBADD(framebuffer));

	/* The screen width is multiplied by the sizeof a single pixel. */
	SET_FIELD(LTDC_LAYER_REG(LTDC_LAYER1)->CFBLR,
	          SET_LTDC_LCFBLR_CFBLL((LCD_CONFIG_WIDTH * LCD_CONFIG_PIXEL_SIZE) + 3) |
	          SET_LTDC_LCFBLR_CFBP(LCD_CONFIG_WIDTH * LCD_CONFIG_PIXEL_SIZE));

	SET_FIELD(LTDC_LAYER_REG(LTDC_LAYER1)->CFBLNR,
	          SET_LTDC_LCFBLNR_CFBLNBR(LCD_CONFIG_HEIGHT));

	SET_FIELD(LTDC_LAYER_REG(LTDC_LAYER1)->CR, LTDC_LCR_LEN());

	/* Reload the LTDC registers immediately instead of waiting for vblank. */
	SET_FIELD(LTDC->SRCR, LTDC_SRCR_IMR());

	/* Enable the LTDC controller. */
	SET_FIELD(LTDC->GCR, LTDC_GCR_LTDCEN());

	/**
	 * Enable backlight and set the LCD_DISP pin high (which tells the LCD
	 * module to start displaying data).
	 */
	gpio_set_output(GPIO_LCD_BL_CTRL, GPIO_HIGH);
	gpio_set_output(GPIO_LCD_DISP, GPIO_HIGH);
}

#endif
