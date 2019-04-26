/**
 * @author Devon Andrade
 * @created 4/7/2018
 *
 * Contains methods for controlling the system timer (the builtin SysTick
 * timer common to all ARM microcontrollers).
 */
/**
 * Design Proposal: Blocking Timer Events in a Multitasking Environment
 *
 * This timer module can be modified in the future to take tasks and put them
 * to sleep until a specific number of cycles has elapsed.
 *
 * This would involve having a static array of timer_event_t structures, each
 * of which contains a pointer to the task_t structure and the cycle count at
 * which to wake the process up.
 *
 * The sleep(task, timeout_in_cycles) function will take the task off of the
 * running queue, enter it into the timer_event_t queue (the final cycle
 * count would be timeout_in_cycles + current_cycles), and yield to the
 * scheduler.
 *
 * In the systick_interrupt (which gets triggered on the tick granularity) every
 * timer event that's queued will be checked to see if that event elapsed. If it
 * did, it will take the item off of the queue and put the task back onto the
 * running queue.
 */
#include "config.h"
#include "debug.h"
#include "interrupt.h"
#include "status.h"
#include "system_timer.h"

#include "registers/systick_reg.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * The maximum number of CPU cycles the SysTick timer can count to.
 */
#define SYSTICK_MAX_TICKS 0x00FFFFFF

/* Ensure the tick granularity is supported by the systick timer (< 2^24). */
_Static_assert(SYSTIMER_TICK <= SYSTICK_MAX_TICKS, "System timer tick is larger than 2^24");

void systick_interrupt(void);

static volatile uint64_t total_cycles = 0;

/**
 * Initialize the system timer to update the `total_cycles` count during each
 * SYSTIMER_TICK granularity defined in the config file.
 */
void init_system_timer(void)
{
	request_interrupt(SysTick_IRQn, systick_interrupt);

	/**
	 * Disable the timer, use processor clock, and enable SysTick interrupt.
	 */
	CLEAR_FIELD(SYSTICK->CTRL, SYSTICK_CTRL_ENABLE());
	SET_FIELD(SYSTICK->CTRL, SYSTICK_CTRL_TICKINT() |
							 SYSTICK_CTRL_CLKSOURCE());

	SYSTICK->LOAD = SET_SYSTICK_LOAD_RELOAD(SYSTIMER_TICK);
	SYSTICK->VAL = 0;

	SET_FIELD(SYSTICK->CTRL, SYSTICK_CTRL_ENABLE());
}

/**
 * Sleep for at least (but maybe more) `cycles` number of CPU cycles.
 *
 * This currently just spins, but in a multitasking environment this function
 * can be modified to take the task off of the running queue until the number
 * of cycles has elapsed. See the design proposal at the top of this file.
 *
 * @note This is not meant for cycle-accurate timing. There will be overhead
 *       associated with setup and interrupt processing that isn't accounted
 *       for when setting up the timer. If you need cycle-accurate timing then
 *       use a dedicated hardware timer.
 *
 * @note On a 216MHz system, there seems to around 0.5us (108 cycles) of
 *       overhead on average.
 *
 * @param cycles The number of CPU cycles to sleep for.
 */
void sleep(uint64_t cycles)
{
	const uint64_t target_cycles = get_cycles() + cycles;

	while(get_cycles() <= target_cycles);
}

/**
 * @return the number of CPU cycles that have elapsed since system start. This
 *         is not restricted to the tick granularity (the cycle delta since the
 *         last tick will be considered).
 */
uint64_t get_cycles(void)
{
	uint64_t initial_cycles = 0;
	uint64_t cycles = 0;

	/**
	 * It's possible for a SysTick interrupt to get triggered after the
	 * total_cycles is read from memory, but before the current counter value
	 * is added. To ensure we get a consistent view of time, two checks of the
	 * total_cycles is needed. If an interrupt did just occur, we'll surely be
	 * able to do the addition before the next interrupt gets triggered, so at
	 * most there will be two iterations of this loop.
	 */
	do {
		initial_cycles = total_cycles;

		/**
		 * The Systick timer is a count-down timer, so to get the delta from the
		 * last interrupt you need to subtract the current value from the reload
		 * value.
		 */
		cycles = initial_cycles + (SYSTIMER_TICK - GET_SYSTICK_VAL_CURRENT(SYSTICK->VAL));
	} while (initial_cycles != total_cycles);

	return cycles;
}

/**
 * SysTick interrupt.
 *
 * @note Processing this ISR should take less time than one tick granularity.
 */
void systick_interrupt(void)
{
	total_cycles += SYSTIMER_TICK;
}