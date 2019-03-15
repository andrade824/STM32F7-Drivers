/**
 * @author Devon Andrade
 * @created 3/10/19
 *
 * Definitions useful in debugging firmware.
 */
#include "debug.h"

/**
 * Cause the system to enter an infinite loop. This is called when the system
 * encounters an unrecoverable error. This gives the user a chance to connect
 * a debugger and capture a backtrace.
 */
void die(void)
{
	dbprintf("[ABORT] Connect with a debugger...\n");
	while(1) { }
}
