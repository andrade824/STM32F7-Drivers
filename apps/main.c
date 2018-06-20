#include "debug.h"
#include "fmc_sdram.h"
#include "gpio.h"
#include "interrupt.h"
#include "platform.h"
#include "status.h"
#include "system.h"

#include "registers/fmc_sdram_reg.h"

#include <stdint.h>

status_t run(void)
{
    ABORT_IF_NOT(init_system());

    dbprintf("System Initialized\n");

    ABORT_IF_NOT(init_fmc_sdram());

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
    
    ABORT_IF_NOT(gpio_request_input(GPIO_B_USER, GPIO_NO_PULL));
    ABORT_IF_NOT(gpio_request_output(GPIO_ARD_D13, low));
    ABORT_IF_NOT(gpio_request_output(GPIO_LCD_BL_CTRL, low));
    gpio_set_otype(GPIO_LCD_BL_CTRL, GPIO_OPEN_DRAIN);

    dbprintf("I'm doing things for twice!!!\n");
    DigitalState led_ctrl = low;

    while(1)
    {
        DigitalState btn = gpio_get_input(GPIO_B_USER);

        /**
         * Set LCD backlight enable to the button input.
         */
        gpio_set_output(GPIO_LCD_BL_CTRL, btn);

        sleep(MSECS(200));

        if(led_ctrl == low)
            led_ctrl = high;
        else
            led_ctrl = low;

        gpio_set_output(GPIO_ARD_D13, led_ctrl);
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