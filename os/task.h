/**
 * @author Devon Andrade
 * @created 6/28/21
 *
 * Type and functions related to task management.
 */
#pragma once

#include <stdint.h>

/**
 * Structure representing a task. Tasks should not access this structure
 * directly but instead go through the API exposed in this header.
 */
typedef struct {
	/**
	 * During context switches, this is where the task's latest stack pointer
	 * will be saved off. Every other register will be saved onto the stack
	 * itself. This value should not be modified outside of the context swiching
	 * code. The context switching code assumes this field is the first one in
	 * the struct.
	 */
	uintptr_t saved_sp;

	/* Name of the task. Only stored for debugging purposes. */
	char *name;

	/* Size of the stack in bytes. */
	size_t stack_size;
} task_t;

task_t * get_current_task(void);

void task_create(
	task_t *task,
	char *task_name,
	uintptr_t stack_mem,
	size_t stack_size,
	void *entry_point,
	void *param);

/**
 * Helper macro for statically allocating a task structure and its stack. It's
 * anticipated that this macro will be used at the global scope so the task and
 * stack is allocated out of the DATA section.
 *
 * @note This macro also forces the stack to 8-byte alignment which is required
 *       according to the ARM procedure call standard.
 */
#define STATIC_TASK_ALLOC(task_name, stack_size) \
	task_t task_name ##_task; \
	uint8_t task_name ## _stack[(stack_size)] __attribute__ ((aligned (8))) = { 0 };

/**
 * Wrapper around task_create() that is meant to be used with an associated call
 * to the STATIC_STASK_ALLOC macro. This macro calls task_create() while using
 * the task structure and stack allocated by STATIC_TASK_ALLOC.
 */
#define STATIC_TASK_CREATE(task_name, stack_size, entry_point, param) \
	(task_create( \
		&task_name ## _task, \
		#task_name, \
		(uintptr_t)task_name ## _stack, \
		(stack_size), \
		(entry_point), \
		(param)))

void sched_begin(void);
task_t * sched_get_next_task(void);
void set_next_task(task_t *task);
