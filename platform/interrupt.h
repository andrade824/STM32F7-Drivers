/**
 * @author Devon Andrade
 * @created 6/8/2017
 *
 * Contains methods for initializaing and controlling interrupts and the
 * vector table.
 */
#pragma once

#include "config.h"

/**
 * Type for pointers to Interrupt Service Routines.
 */
typedef void (*ISR_Type) (void);

void interrupt_init(void);

void interrupt_request(IRQn_Type irq, ISR_Type isr);

/* TODO: Add methods for setting priorities and enabling/disabling ISRs */
