#include "debug.h"
#include "dma2d.h"
#include "fmc_sdram.h"
#include "gpio.h"
#include "interrupt.h"
#include "lcd_ctrl.h"
#include "lcd_timings.h"
#include "platform.h"
#include "status.h"
#include "system.h"

#include "registers/dma2d_reg.h"
#include "registers/fmc_sdram_reg.h"
#include "registers/lcd_ctrl_reg.h"

#include <stdint.h>
#include <stdlib.h>

const uint16_t height = default_lcd_settings.active_height - 1;
const uint16_t width = default_lcd_settings.active_width - 1;

const uint32_t draw_buffer = SDRAM_BASE;
const uint32_t render_buffer = SDRAM_BASE + (default_lcd_settings.active_width * default_lcd_settings.active_height * 4);

#define PIXEL(r,g,b) PIXEL_8888(r,g,b)
#define PIXEL_8888(r,g,b) (0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF))
#define PIXEL_565(r,g,b) (((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F))
#define CLEAR_SCREEN() (draw_rect(0, 0, width, height, PIXEL(0xFF, 0, 0xFF)))

// (x0, y0) is top left, (x1, y1) is bottom right
status_t draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
    ABORT_IF(x0 > x1);
    ABORT_IF(y0 > y1);
    ABORT_IF(x1 >= default_lcd_settings.active_width);
    ABORT_IF(y1 >= default_lcd_settings.active_height);

    const uint16_t total_width = default_lcd_settings.active_width;
    for(int row = y0; row <= y1; row++) {
        for (int col = x0; col <= x1; col++) {
            *(uint32_t*)(draw_buffer + ((row * total_width + col) * 4)) = color;
        }
    }

    return Success;
}

status_t draw_random_rect(void) {
    uint8_t red = rand() % 256;
    uint8_t blue = rand() % 256;
    uint8_t green = rand() % 256;

    uint16_t x0 = rand() % (width / 2);
    uint16_t y0 = rand() % (height / 2);
    uint16_t x1 = x0 + (rand() % (width / 2));
    uint16_t y1 = y0 + (rand() % (height / 2));

    ABORT_IF_NOT(draw_rect(x0, y0, x1, y1, PIXEL(red, green, blue)));

    return Success;
}

void vblank_isr(void) {
    if(GET_LTDC_ISR_LIF(LTDC->ISR)) {
        SET_FIELD(LTDC->ICR, LTDC_ICR_CLIF());

        if(is_dma2d_complete()) {
            dma2d_mem_to_mem(draw_buffer, render_buffer, 480, 272);
        }
    }
}

void error_isr(void) {
    if(GET_LTDC_ISR_FUIF(LTDC->ISR)) {
        dbprintf("LCD Error: FIFO Underrun error\n");

        SET_FIELD(LTDC->ICR, LTDC_ICR_CFUIF());
    }

    if(GET_LTDC_ISR_TERRIF(LTDC->ISR)) {
        dbprintf("LCD Error: Transfer error\n");

        SET_FIELD(LTDC->ICR, LTDC_ICR_CTERRIF());
    }
}

status_t run(void)
{
    ABORT_IF_NOT(init_system());

    dbprintf("System Initialized\n");

    ABORT_IF_NOT(init_fmc_sdram());
    ABORT_IF_NOT(init_dma2d());

#if 0
    for(int i = 0; i < 3; i++) {
        dbprintf("Beginning Memcheck...\n");
        for (uint32_t uwIndex = 0; uwIndex < 0x00800000; uwIndex += 4)
        {
            *(volatile uint32_t*) (SDRAM_BASE + uwIndex) = uwIndex;
        }

        for (uint32_t uwIndex = 0; uwIndex < 0x00800000; uwIndex += 4)
        {
            uint32_t value = *(volatile uint32_t*) (SDRAM_BASE + uwIndex);

            if(value != uwIndex)
            {
                dbprintf("Memcheck failure: value 0x%lx != index 0x%lx\n", value, uwIndex);
                gpio_set_output(GPIO_ARD_D13, high);
                while(1) { }
            }

            if(uwIndex % 0x10000 == 0) {
                dbprintf("Checkpoint... value 0x%lx -- index 0x%lx\n", value, uwIndex);
            }

        }
        dbprintf("Memcheck complete!!\n");
    }

#endif

    srand(37); // Number chosen randomly through multiple dice throws.
    CLEAR_SCREEN();
#if 0
    draw_rect(100, 170, 150, 220, PIXEL(100, 0, 0x1F));
    draw_random_rect();
    draw_random_rect();
    draw_random_rect();

    draw_rect(0, 0 + 20, 50, 50 + 20, PIXEL(0, 0, 255));
    draw_rect(0, 51 + 20, 50, 100 + 20, PIXEL(0, 255, 0));
    draw_rect(0, 101 + 20, 50, 150 + 20, PIXEL(255, 0, 0));

    draw_rect(430, 0 + 20, 479, 50 + 20, PIXEL(0, 0, 255));
    draw_rect(430, 51 + 20, 479, 100 + 20, PIXEL(0, 255, 0));
    draw_rect(430, 101 + 20, 479, 150 + 20, PIXEL(255, 0, 0));

    // for(int row = 0; row < default_lcd_settings.active_height; row++) {
    //     *(uint32_t*)(SDRAM_BASE + ((row * default_lcd_settings.active_width) * 4)) = PIXEL(0xFF, 0xFF, 0xFF);
    //     *(uint32_t*)(SDRAM_BASE + ((row * default_lcd_settings.active_width + 479) * 4)) = PIXEL(0xFF, 0xFF, 0xFF);
    // }
#endif
    ABORT_IF_NOT(request_interrupt(LTDC_IRQn, vblank_isr));
    ABORT_IF_NOT(request_interrupt(LTDC_ER_IRQn, error_isr));

    ABORT_IF_NOT(init_lcd_ctrl(default_lcd_settings, render_buffer));

    ABORT_IF_NOT(gpio_request_input(GPIO_B_USER, GPIO_NO_PULL));
    ABORT_IF_NOT(gpio_request_output(GPIO_ARD_D13, low));

    DigitalState led_ctrl = low;

#if 1
    uint16_t x = 0;
    const uint16_t y = 100;
    const uint16_t rect_width = 50;
    const uint16_t rect_height = 50;
#endif

    while(1)
    {
        sleep(MSECS(16));

        if(led_ctrl == low)
            led_ctrl = high;
        else
            led_ctrl = low;

        gpio_set_output(GPIO_ARD_D13, led_ctrl);

#if 1
        CLEAR_SCREEN();
        draw_random_rect();
        draw_rect(0, 0, 50, 50, PIXEL(0, 0, 255));
        draw_rect(100, 170, 150, 220, PIXEL(0, 255, 0));
        draw_rect(x, y, x + rect_width, y + rect_height, PIXEL(255, 0, 0));

        x = ((x + rect_width + 3) > width) ? 0 : x + 3;
#endif
    }

    return Success;
}

int main(void)
{
    if(!run())
    {
        while(1)
        { }
    }

    return 0;
}