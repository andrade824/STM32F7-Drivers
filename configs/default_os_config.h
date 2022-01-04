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

/* Whether the RTOS features are enabled. */
#define OS_ENABLED 1

/* Enable the FPU and saving/restoring of FPU context when switching tasks. */
#define FPU_ENABLED 1

/**
 * The smallest possible stack size contains just enough space to store the
 * registers needed for context switching and nothing else.
 */
#define MIN_STACK_SIZE 72U

/**
 * Init/Idle thread stack size. The initial thread that runs main() will turn
 * into the idle thread once the scheduler begins. The bottom (zeroth) byte will
 * be used as a stack guard byte.
 */
#define INIT_THREAD_STACK_SIZE 512U

/**
 * Minimum interrupt stack size is 512 bytes. This value is used to ensure the
 * heap doesn't grow into the interrupt stack. This check isn't entirely
 * accurate since the interrupt stack could be larger than this value, but it
 * does provide at least a minimum size the interupt stack can be without
 * needing to worry that the heap will grow into it.
 */
#define MIN_INTR_STACK_SIZE 512U

/**
 * Set to 1 to enable inserting and checking the stack guard byte value. One
 * byte at the bottom of every stack will be used as a guard value.
 */
#define ENABLE_STACK_GUARD 1

#if ENABLE_STACK_GUARD
/**
 * The magic value used as a stack guard. If the stack guard value (located at
 * the end of the stack) is ever changed from this value, then the stack will
 * have overflowed.
 */
#define STACK_GUARD_MAGIC 0xD5
#endif /* ENABLE_STACK_GUARD */

/**
 * System timer tick granularity (in CPU_HZ cycles).
 *
 * This has to be less than 2^24 cycles (the width of the systick timer). With
 * a 216MHz CPU clock, this gives a maximum tick of 77.6ms.
 */
#define SYSTIMER_TICK (CPU_HZ / 1000U) /* 1ms tick */
