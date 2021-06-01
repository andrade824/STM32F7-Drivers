/**
 * @author Devon Andrade
 * @created 3/28/21
 *
 * Configuration values for the operating system features provided by this
 * project.
 *
 * This file is meant to be used as a template and should be modified to fit the
 * application and platform configuration's need.
 */
#pragma once

/**
 * Minimum kernel stack size is 512 bytes. This value is used to ensure the heap
 * doesn't grow into the kernel stack. This check isn't entirely accurate since
 * the kernel stack could be larger than this value, but it does provide at
 * least a minimum size the kernel stack can be without needing to worry that
 * the heap will grow into it.
 */
#define MIN_KERN_STACK_SIZE 512U

/**
 * System timer tick granularity (in CPU_HZ cycles).
 *
 * This has to be less than 2^24 cycles (the width of the systick timer). With
 * a 216MHz CPU clock, this gives a maximum tick of 77.6ms.
 */
#define SYSTIMER_TICK (CPU_HZ / 1000U) /* 1ms tick */
