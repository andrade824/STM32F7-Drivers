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
 * Function that will get called when a task attempts to return from its entry
 * point. Typically, tasks are not supposed to return, so this will kill the
 * system.
 */
static void task_returned(void)
{
	dbprintf("Task \"%s\" returned. That's weird ._.\n", get_current_task()->name);
	die();
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

	/* TODO: GET RID OF THIS. */
	next_task_to_sched = task;

	task->name = task_name;
	task->stack_size = stack_size;

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
	 * grows downward). The initial state
	 */
	stack_mem += stack_size - sizeof(initial_task_state_t);
	task->saved_sp = stack_mem;

	initial_task_state_t *state = (initial_task_state_t*)stack_mem;
	memset((void*)state, 0, sizeof(initial_task_state_t));

	/**
	 * This is the exception return code to set LR to that tells the exception
	 * return hardware what mode and stack to switch to when returning. This
	 * code specifies Thread Mode, Process Stack, and "Basic" frame (meaning
	 * that no floating point registers were pushed in the initial state).
	 */
	#define EXC_RETURN_PROCESS 0xFFFFFFFDUL
	state->exc_return = EXC_RETURN_PROCESS;

	/* Setup the entry point and the parameter to that function. */
	state->r0_to_r3[0] = (uint32_t)param;
	state->pc = (uint32_t)entry_point;
	state->lr = (uint32_t)&task_returned;

	/* The "Thumb" bit HAS to be set in xPSR for all armv7-M code. */
	state->psr = 0x1000000;
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

	/* Switch to the highest priority task. */
	intr_trigger_pendsv();

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

void set_next_task(task_t *task)
{
	ASSERT(task != NULL);
	next_task_to_sched = task;
}
