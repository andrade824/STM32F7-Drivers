/**
 * @author Devon Andrade
 * @created 4/7/2018
 *
 * Contains methods for controlling the system timer (the builtin SysTick
 * timer common to all ARM microcontrollers).
 */
#include "debug.h"
#include "interrupt.h"
#include "registers/systick_reg.h"
#include "status.h"
#include "system_timer.h"

#include <stdbool.h>
#include <stdint.h>

void systick_interrupt(void);

static bool timer_running = false;
static uint32_t ticks_left = 0;
static volatile bool timer_complete = false;

/**
 * Initialize the system timer.
 */
status_t init_system_timer(void)
{
    ABORT_IF_NOT(request_interrupt(SysTick_IRQn, systick_interrupt));

    /**
     * Disable the timer, use processor clock, and enable SysTick interrupt.
     */
    CLEAR_FIELD(SYSTICK->CTRL, SYSTICK_CTRL_ENABLE());
    SET_FIELD(SYSTICK->CTRL, SYSTICK_CTRL_TICKINT() |
                             SYSTICK_CTRL_CLKSOURCE());

    /**
     * Clear current count value.
     */
    SYSTICK->VAL = 0;

    return success;
}

/**
 * Example ABORT_TIMEOUT:
 * 
 * ABORT_IF_NOT(start_timer(TIMEOUT));
 * while(!cond && !timer_complete());
 * if(timer_complete) return fail;
 */

/**
 * Trigger the timer to start counting to `ticks` number of CPU cycles.
 * 
 * @param ticks The number of ticks to sleep for.
 */
status_t start_timer(uint32_t ticks)
{
    uint32_t ticks_set = 0;

    ABORT_IF(timer_running);

    /**
     * Set up the timer reload value. The actual number of cycles the timer
     * counts is one more than what the reload value is set to, so we decrement
     * from the wanted tick value to account for this.
     */
    if(ticks > SYSTICK_MAX_TICKS) {
        ticks_set = SYSTICK_MAX_TICKS;
    } else {
        ticks_set = ticks;
    }

    ticks_left = ticks - ticks_set;
    SYSTICK->LOAD = SET_SYSTICK_LOAD_RELOAD(ticks_set - 1);

    timer_complete = false;
    timer_running = true;

    SET_FIELD(SYSTICK->CTRL, SYSTICK_CTRL_ENABLE());

    return success;
}

/**
 * Sleep for `ticks` number of CPU cycles.
 *
 * @param ticks The number of ticks to sleep for.
 */
status_t sleep(uint32_t ticks)
{
    ABORT_IF_NOT(start_timer(ticks));

    while(!timer_complete);

    return success;
}

/**
 * @return true if the timer is done counting, false otherwise.
 */
bool is_timer_complete(void)
{
    return timer_complete;
}

void systick_interrupt(void)
{
    // if ticks_left != 0 then set up reload value with next chunk of ticks
    // set ticks_left -= ticks_set
    // enable timer
    // else, timer_complete = true
}