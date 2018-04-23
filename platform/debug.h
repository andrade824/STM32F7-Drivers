/**
 * @author Devon Andrade
 * @created 12/22/2016
 *
 * Definitions useful in debugging firmware.
 */
#ifndef DEBUG_H
#define DEBUG_H

#include "system_timer.h"

#include <stdio.h>

#ifdef DEBUG_ON
    #define dbputs(s)               puts(s)
    #define dbprintf(szFormat, ...) printf(szFormat,##__VA_ARGS__)
#else
    #define dbout(s)
    #define dbprintf(szFormat, ...)
#endif

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
        ABORT_IF_NOT(start_timer(timeout));                         \
        while(!(expr) && !is_timer_complete());                     \
        if(is_timer_complete()) {                                   \
            return fail;                                            \
        }                                                           \
    } while(0)

/**
 * Aborts the current method early if the expression evaluates to true and
 * returns a custom value.
 *
 * @param expr The expression to break on.
 * @param retval The value to return if expr is true.
 */
#define ABORT_IF_VAL(expr,retval)                                   \
    do                                                              \
    {                                                               \
        if((expr))                                                  \
        {                                                           \
            dbprintf("[ABORT] %s:%s():%d -- %s,\n",                 \
                     __FILE__, __func__, __LINE__, #expr);          \
            return retval;                                          \
        }                                                           \
    } while(0)

/**
 * Aborts the current method early if the expression evaluates to true.
 *
 * @param expr The expression to break on.
 *
 * @return fail if the expression is true.
 */
#define ABORT_IF(expr) ABORT_IF_VAL(expr,fail)

/**
 * Aborts the current method early if the expression evaluates to false and
 * return a custom value.
 *
 * @param expr The expression to break on.
 * @param retval The value to return if expr is false.
 */
#define ABORT_IF_NOT_VAL(expr,retval) ABORT_IF_VAL(!(expr),retval)

/**
 * Aborts the current method early if the expression evaluates to false.
 *
 * @param expr The expression to break on.
 *
 * @return fail if the expression is false.
 */
#define ABORT_IF_NOT(expr) ABORT_IF(!(expr))

#endif