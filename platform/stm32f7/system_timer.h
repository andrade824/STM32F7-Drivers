/**
 * @author Devon Andrade
 * @created 4/7/2018
 *
 * Contains methods for controlling the system timer (the builtin SysTick
 * timer common to all ARM microcontrollers).
 */

#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include "config.h"
#include "status.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * Converts between ticks and seconds.
 *
 * Example usage (delay for 25ms):
 * start_timer(MSECS(25));
 */
#define SECS(x)  (x * (CPU_HZ))
#define MSECS(x) (x * (CPU_HZ / 1000))
#define USECS(x) (x * (CPU_HZ / 1000000))

/**
 * The maximum number of CPU cycles the SysTick timer can count to.
 */
#define SYSTICK_MAX_TICKS 0x00FFFFFF

status_t init_system_timer(void);

status_t start_timer(uint32_t ticks);
void stop_timer(void);
status_t sleep(uint32_t ticks);

bool is_timer_complete(void);

#endif