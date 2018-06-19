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

static uint32_t volatile ticks_left = 0;
static volatile bool timer_complete = true;

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

    return Success;
}

/**
 * Trigger the timer to start counting to `ticks` number of CPU cycles.
 * 
 * @note This is not meant for cycle-accurate timing. There will be overhead
 *       associated with setup and interrupt processing that isn't accounted
 *       for when setting up the timer. If you need cycle-accurate timing then
 *       use a dedicated hardware timer.
 *
 * @param ticks The number of ticks to sleep for.
 */
status_t start_timer(uint32_t ticks)
{
    uint32_t ticks_set = 0;

    /**
     * Ensure the timer hasn't already started and that the user is requesting
     * a valid number of ticks to delay for.
     */
    ABORT_IF(ticks_left != 0 || !timer_complete);
    ABORT_IF(ticks == 0);

    ticks_set = (ticks > SYSTICK_MAX_TICKS) ? SYSTICK_MAX_TICKS : ticks;
    ticks_left = ticks - ticks_set;
    SYSTICK->LOAD = SET_SYSTICK_LOAD_RELOAD(ticks_set);

    timer_complete = false;

    SET_FIELD(SYSTICK->CTRL, SYSTICK_CTRL_ENABLE());

    return Success;
}

/**
 * Stop the timer prematurely and reset the internal context to a safe state.
 */
void stop_timer(void)
{
    CLEAR_FIELD(SYSTICK->CTRL, SYSTICK_CTRL_ENABLE());
    SYSTICK->VAL = 0;

    timer_complete = true;
    ticks_left = 0;
}

/**
 * Sleep for at least `ticks` number of CPU cycles.
 *
 * @note This is not meant for cycle-accurate timing. There will be overhead
 *       associated with setup and interrupt processing that isn't accounted
 *       for when setting up the timer. If you need cycle-accurate timing then
 *       use a dedicated hardware timer.
 *
 * @param ticks The number of ticks to sleep for.
 */
status_t sleep(uint32_t ticks)
{
    ABORT_IF_NOT(start_timer(ticks));

    while(!timer_complete);

    return Success;
}

/**
 * @return true if the timer is done counting, false otherwise.
 */
bool is_timer_complete(void)
{
    return timer_complete;
}

/**
 * SysTick interrupt.
 */
void systick_interrupt(void)
{
    uint32_t ticks_set = 0;

    CLEAR_FIELD(SYSTICK->CTRL, SYSTICK_CTRL_ENABLE());

    if(ticks_left != 0) {
        ticks_set = (ticks_left > SYSTICK_MAX_TICKS) ? SYSTICK_MAX_TICKS : ticks_left;
        ticks_left -= ticks_set;

        SYSTICK->LOAD = SET_SYSTICK_LOAD_RELOAD(ticks_set);
        SYSTICK->VAL = 0;
        SET_FIELD(SYSTICK->CTRL, SYSTICK_CTRL_ENABLE());
    } else {
        timer_complete = true;
    }
}