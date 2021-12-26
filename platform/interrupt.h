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
 * STM32F7 only supports 16 interrupt priority levels. The "urgency" of the
 * interrupt is inversely correlated with its priority number (e.g., zero is the
 * highest priority). For a system with 4-bit priority, priority 15 would be the
 * lowest possible priority.
 */
#define LOWEST_INTR_PRIORITY ((1U << (INTR_PRIORITY_BITS)) - 1)

/**
 * Type for pointers to Interrupt Service Routines.
 */
typedef void (*isr_func_t) (void);

void intr_init(void);

void intr_enable_interrupts(void);
void intr_disable_interrupts(void);

void intr_register(irq_num_t irq, isr_func_t isr, uint8_t priority);
void intr_register_svcall(isr_func_t isr, uint8_t priority);
void intr_register_pendsv(isr_func_t isr, uint8_t priority);
void intr_register_systick(isr_func_t isr, uint8_t priority);

void intr_enable_irq(irq_num_t irq);
void intr_disable_irq(irq_num_t irq);

void intr_set_base_priority(uint8_t priority);
void intr_trigger_pendsv(void);

/* TODO: Add methods for setting priorities and enabling/disabling ISRs */
