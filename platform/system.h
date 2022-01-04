/**
 * @author Devon Andrade
 * @created 12/22/2016
 *
 * Contains initialization functions used to get the system into a usable
 * state after startup.
 */
#pragma once

/* Helper macros for issuing barriers. */
#define DMB() asm volatile("dmb SY" ::: "memory")
#define DSB() asm volatile("dsb SY" ::: "memory")
#define ISB() asm volatile("isb SY" ::: "memory")

void system_init(void);
