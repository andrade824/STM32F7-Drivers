/**
 * @author Devon Andrade
 * @created 12/22/2016
 *
 * Definitions useful in debugging firmware.
 */
#pragma once

#include "system_timer.h"

#include <stdio.h>

#ifdef DEBUG_ON
	#define dbputs(s)               puts(s)
	#define dbprintf(szFormat, ...) printf(szFormat,##__VA_ARGS__)
#else
	#define dbputs(s)
	#define dbprintf(szFormat, ...)
#endif

/**
 * Cause the system to enter an infinite loop. This is called when the system
 * encounters an unrecoverable error. This gives the user a chance to connect
 * a debugger and capture a backtrace.
 */
void die(void);

/**
 * Prints out a debug message and then enters the system into an "abort" state.
 *
 * @param msg The abort message to print before crashing.
 */
#define ABORT(msg,...)                                                  \
	do                                                                  \
	{                                                                   \
		dbprintf("[ABORT] %s:%s():%d -- " msg "\n",                     \
		         __FILE__, __func__, __LINE__, ##__VA_ARGS__);          \
		die();                                                          \
	} while(0)

/**
 * Aborts the current method early if the expression doesn't evaluate to true
 * within the given `timeout` period.
 *
 * On single-threaded systems, this would require the expression to be modified
 * either through hardware or an interrupt service routine.
 */
#define ABORT_TIMEOUT(expr,timeout)                                 \
	do                                                              \
	{                                                               \
		const uint64_t target_cycles = get_cycles() + timeout;      \
		while(!(expr) && (get_cycles() <= target_cycles));          \
		if(get_cycles() > target_cycles) {                          \
			dbprintf("[ABORT] %s:%s():%d -- %s\n",                  \
			         __FILE__, __func__, __LINE__, #expr);          \
			die();                                                  \
		}                                                           \
	} while(0)

/**
 * Aborts the current method early if the expression evaluates to true.
 *
 * @param expr The expression to die on.
 */
#define ABORT_IF(expr)                                              \
	do                                                              \
	{                                                               \
		if((expr))                                                  \
		{                                                           \
			dbprintf("[ABORT] %s:%s():%d -- %s\n",                  \
			         __FILE__, __func__, __LINE__, #expr);          \
			die();                                                  \
		}                                                           \
	} while(0)

/**
 * Aborts the current method early if the expression evaluates to false.
 *
 * @param expr The expression to break on if false.
 */
#define ABORT_IF_NOT(expr) ABORT_IF(!(expr))

/**
 * Asserts are similar to Aborts except they get compiled out on release builds.
 */
#ifdef DEBUG_ON
	#define ASSERT(expr) ABORT_IF_NOT(expr)
	#define ASSERT_NOT(expr) ABORT_IF(expr)
#else
	#define ASSERT(expr)
	#define ASSERT_NOT(expr)
#endif
