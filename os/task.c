/**
 * @author Devon Andrade
 * @created 12/18/21
 *
 * Functions related to task management.
 */
#include "config.h"
#include "debug.h"
#include "os/task.h"

#include <stdint.h>

/* Pointer to the task structure for the currently running task. */
static Task *current_task_ptr = NULL;

/**
 * Return a poiner to the current running task's task structure.
 */
Task * current_task(void)
{
	return current_task_ptr;
}
