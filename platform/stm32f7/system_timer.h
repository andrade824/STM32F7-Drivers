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

void init_system_timer(void);

void start_timer(uint32_t ticks);
void stop_timer(void);
void sleep(uint32_t ticks);

bool is_timer_complete(void);

#endif