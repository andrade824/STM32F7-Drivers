/**
 * @author Devon Andrade
 * @created 4/7/2018
 *
 * Contains methods for controlling the system timer (the builtin SysTick
 * timer common to all ARM microcontrollers).
 */
#pragma once

#include "config.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * Converts between cycles and seconds.
 *
 * Example usage (delay for 25ms):
 * sleep(MSECS(25));
 */
#define SECS(x)  (x * (CPU_HZ))
#define MSECS(x) (x * (CPU_HZ / 1000))
#define USECS(x) (x * (CPU_HZ / 1000000))

void system_timer_init(void);

void sleep(uint64_t cycles);

uint64_t get_cycles(void);
