#include "debug.h"
#include "gpio.h"
#include "interrupt.h"
#include "platform.h"
#include "status.h"
#include "system.h"

#include <stdint.h>

void SysTick_Handlerfsddfs();

status_t run(void)
{
    ABORT_IF_NOT(init_system(), fail);

    dbprintf("System Initialized\n");

    ABORT_IF_NOT(gpio_request_input(GPIO_B_USER, GPIO_NO_PULL), fail);
    ABORT_IF_NOT(gpio_request_output(GPIO_ARD_D13, low), fail);
    ABORT_IF_NOT(gpio_request_output(GPIO_LCD_BL_CTRL, low), fail);
    gpio_set_otype(GPIO_LCD_BL_CTRL, GPIO_OPEN_DRAIN);
    
    ABORT_IF_NOT(SysTick_Config(16000000) == 0, fail);
    ABORT_IF_NOT(request_interrupt(SysTick_IRQn, &SysTick_Handlerfsddfs), fail);

    while(1)
    {
        DigitalState btn = gpio_get_input(GPIO_B_USER);

        /**
         * Set LCD backlight enable to the button input.
         */
        gpio_set_output(GPIO_LCD_BL_CTRL, btn);
    }

    return success;
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

volatile DigitalState led_ctrl = low;

void SysTick_Handlerfsddfs()
{
    if(led_ctrl == low)
        led_ctrl = high;
    else
        led_ctrl = low;
    
    gpio_set_output(GPIO_ARD_D13, led_ctrl);
}