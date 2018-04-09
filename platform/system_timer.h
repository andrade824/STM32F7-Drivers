/**
 * @author Devon Andrade
 * @created 4/7/2018
 *
 * Contains methods for controlling the system timer (the builtin SysTick
 * timer common to all ARM microcontrollers).
 */

#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include "platform.h"
#include "status.h"

/**
 * Converts between ticks and seconds.
 * 
 * Example usage:
 * start_timer(25 MSECS);
 */
#define SECS    * (CPU_HZ)
#define MSECS   * (CPU_HZ / 1000)
#define USECS   * (CPU_HZ / 1000000)

/**
 * The maximum number of CPU cycles the SysTick timer can count to.
 */
#define SYSTICK_MAX_TICKS 0x00FFFFFF

status_t init_system_timer(void);

status_t start_timer(uint32_t ticks);
status_t sleep(uint32_t ticks);

bool is_timer_complete(void);

#endif