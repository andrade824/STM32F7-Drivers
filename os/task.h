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
	 * code.
	 */
	uint32_t saved_sp;
} Task;

Task * current_task(void);
