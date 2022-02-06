/**
 * @author Devon Andrade
 * @created 12/18/21
 *
 * Functions related to task management, including the scheduler logic.
 */
#include "config.h"
#include "debug.h"
#include "interrupt.h"
#include "os/task.h"

#include <stdint.h>
#include <string.h>

/* Pointer to the task structure for the currently running task. */
static task_t *current_task = NULL;

/* Task structure used by the initial/idle task. */
static task_t idle_task = {
	/* The current SP will be written by the context switch logic. */
	.saved_sp = 0,
	.name = "idle task",
	.stack_size = INIT_THREAD_STACK_SIZE
};

/* The next task to run. Typically this will be determined by a scheduling algorithm. */
static task_t *next_task_to_sched = NULL;

/**
 * Return a poiner to the current running task's task structure.
 */
task_t * get_current_task(void)
{
	return current_task;
}

/**
 * Wrapper function set as the entry point for every task. This is done to
 * properly capture when a task returns (which it's not supposed to do).
 *
 * @param entry_point Entry point function for the task being called.
 * @param param Parameter to pass to the entry point.
 */
static void task_wrapper(void (*entry_point)(void*), void *param)
{
	ASSERT(entry_point != NULL);

	/* Call the actual task entry point. */
	entry_point(param);

	ABORT("Task \"%s\" returned. That's weird ._.\n", get_current_task()->name);
}

/**
 * Initialize a task structure and register it with the scheduler. Until this
 * occurs, the task will not be runnable.
 *
 * @param task Pointer to the task structure to initialize.
 * @param task_name Name of the task. Strictly used for debugging purposes.
 * @param stack_mem Memory address of the bottom of the stack. The stack pointer
 *                  for this task will be set to the top of the region of memory
 *                  allocated for the stack
 * @param stack_size Size of the stack pointed to by [stack_mem].
 * @param entry_point Pointer to the function to jump to the first time this
 *                    task is scheduled.
 * @param param Parameter to pass to [entry_point]. Allowed to be NULL if no
 *              parameter is needed.
 */
void task_create(
	task_t *task,
	char *task_name,
	uintptr_t stack_mem,
	size_t stack_size,
	void *entry_point,
	void *param)
{
	ASSERT((task != NULL) && (task_name != NULL) && (stack_mem != 0));
	ASSERT(stack_size > MIN_STACK_SIZE);

	/* Ensure the stack has 8-byte alignment. This is an ARM architectural requirement. */
	ASSERT((stack_mem & 0x7) == 0);

	/* TODO: GET RID OF THIS AFTER REAL SCHED ALGORITHM IS WRITTEN. */
	next_task_to_sched = task;

	task->name = task_name;
	task->stack_size = stack_size;

	/**
	 * This structure must match the exact order and alignment that the context
	 * switching and exception return hardware expect the state to be in.
	 */
	typedef struct {
		/* Registers r4-r11. */
		uint32_t r4_to_r11[8];

		/* The exception return code used when returning from the PendSV handler. */
		uint32_t exc_return;

		/**
		 * The following are the values typically pushed onto the stack during
		 * exception entry. The context switch code uses the automatic saving
		 * and restoring of these registers so these also need to be setup.
		 */
		uint32_t r0_to_r3[4];
		uint32_t r12;
		uint32_t lr;
		uint32_t pc;
		uint32_t psr;
	} initial_task_state_t;

	/**
	 * Setup the initial register state. This needs to match what the context
	 * switching code (and the exception return hardware) expects. Reserve
	 * enough space on the stack to store the initial register state (the stack
	 * grows downward).
	 *
	 * The first context switch to this thread will pop off this initial state,
	 * load it up, and move the stack pointer back up to the top.
	 */
	stack_mem += stack_size - sizeof(initial_task_state_t);
	task->saved_sp = stack_mem;

	initial_task_state_t *state = (initial_task_state_t*)stack_mem;
	memset((void*)state, 0, sizeof(initial_task_state_t));

	/* TODO: Set up stack guard value. */

	/**
	 * This is the exception return code to set LR to that tells the exception
	 * return hardware what mode and stack to switch to when returning. This
	 * code specifies Thread Mode, Process Stack, and "Basic" frame (meaning
	 * that no floating point registers were pushed in the initial state).
	 */
	#define EXC_RETURN_PROCESS 0xFFFFFFFDUL
	state->exc_return = EXC_RETURN_PROCESS;

	/* Setup the entry point and the parameter to that function. */
	state->r0_to_r3[0] = (uint32_t)entry_point;
	state->r0_to_r3[1] = (uint32_t)param;
	state->pc = (uint32_t)task_wrapper;

	/* The "Thumb" bit HAS to be set in xPSR for all armv7-M code. */
	state->psr = 0x1000000;
}

/**
 * Context switch logic that can be registered as the PendSV handler. From that
 * point on, triggering a PendSV exception will switch the currently running
 * task to the next task to run according to the scheduler.
 *
 * @note It's perfectly possible for the task being switched to and the task
 *       being switched from to be the same task.
 *
 * @note It's up to the scheduler as to which task will get switched to next.
 *
 * @note Typically this cswitch handler will be registered as the PendSV handler
 *       which would allow for causing a context switch whenever the PendSV
 *       exception is triggered. This supports both cooperative and preemptive
 *       scheduler approaches. The PendSV handler should be configured with the
 *       lowest possible priority to ensure that other exceptions can't get
 *       nested by this logic. This allows the PendSV exception to be safely
 *       triggered from another exception (like a timer ISR).
 */
__attribute ((__naked__))
static void cswitch_handler(void)
{
	asm volatile (
		/**
		 * Exception entry automatically pushed r0-r3, r12, LR, PC (Return
		 * Address), and xPSR. Need to push the rest of the registers onto the
		 * task's Process Stack. We are currently operating on the Main Stack.
		 */
		"mrs	r0, PSP \n"

		/**
		 * Push the remaining registers that need to be saved onto the stack,
		 * including the LR value that was generated by the exception entry
		 * logic (which determines which mode/stack to return to).
		 */
		"stmdb	r0!, {r4-r11, r14} \n"

		/* Get the current task structure pointer. */
		"ldr	r1, current_task_ptr \n"
		"ldr	r1, [r1] \n"

		/* Store the current process stack pointer in the current task structure. */
		"str	r0, [r1] \n"

		/* TODO: Check the stack guard value and die() if it's been clobbered. */

		/* Determine the next task to run. Task pointer returned through r0. */
		"bl		sched_get_next_task \n"

		/* Load up the new task's state that was previously pushed to the stack. */
		"ldr	r0, [r0] \n"
		"ldm	r0!, {r4-r11, r14} \n"
		"msr	PSP, r0 \n"
		"isb	sy \n"

		/* Return */
		"bx		lr \n"

		/* C variables accessed in ASM. */
		"current_task_ptr: .word current_task"
	);
}

/**
 * Meant to be called on the initial thread after all tasks have been created.
 * This will setup the current thread to act as the idle thread and switch to
 * the task with the highest priority.
 */
void sched_begin(void)
{
	/**
	 * Triggering the PendSV handler is what will trigger a context switch. The
	 * handler MUST be set to the lowest interrupt priority possible so that it
	 * never nests another exception (it assumes it's preempting a task, not an
	 * exception).
	 */
	extern void cswitch_handler(void);
	intr_register_pendsv(&cswitch_handler, LOWEST_INTR_PRIORITY);

	/* Use the current thread as the Idle thread going forwards. */
	current_task = &idle_task;

	/* Switch to the highest priority runnable task. */
	sched_yield();

	/**
	 * Once the idle task is next scheduled to return, this function will return
	 * and the code after the call to this function will be used as the idle
	 * task.
	 */
}

/**
 * Return back the next task that should be run on the CPU and set the current
 * task to that task.
 */
task_t * sched_get_next_task(void)
{
	static unsigned int num_switches = 0;

	if(++num_switches == 5) {
		num_switches = 0;
		next_task_to_sched = &idle_task;
	}

	current_task = next_task_to_sched;

	return next_task_to_sched;
}

/**
 * Dummy function used until a real scheduler is built.
 *
 * TODO: Get rid of this.
 */
void set_next_task(task_t *task)
{
	ASSERT(task != NULL);
	next_task_to_sched = task;
}

/**
 * Yield the current thread to the next highest priority runnable thread.
 */
void sched_yield(void)
{
	intr_trigger_pendsv();
}
