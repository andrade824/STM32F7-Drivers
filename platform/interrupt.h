/**
 * @author Devon Andrade
 * @created 6/8/2017
 *
 * Contains methods for initializaing and controlling interrupts and the
 * vector table.
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "config.h"
#include "status.h"

/**
 * Type for pointers to Interrupt Service Routines.
 */
typedef void (*ISR_Type) (void);

status_t init_interrupts(void);

status_t request_interrupt(IRQn_Type irq, ISR_Type isr);

/* TODO: Add methods for setting priorities and enabling/disabling ISRs */

#endif