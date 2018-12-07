/**
 * @author Devon Andrade
 * @created 4/9/2018
 *
 * Definitions and functions used to manipulate the SysTick timer.
 */
#ifndef SYSTICK_REG_H
#define SYSTICK_REG_H

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

/**
 * Type defining the SysTick module register map.
 */
typedef struct
{
    volatile uint32_t CTRL;   /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
    volatile uint32_t LOAD;   /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
    volatile uint32_t VAL;    /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
    volatile uint32_t CALIB;  /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTickReg;

/**
 * Define the SysTick register map accessor.
 */
#define SYSTICK_REG_BASE (SCS_BASE +  0x0010UL)
#define SYSTICK ((SysTickReg *) SYSTICK_REG_BASE)

/**
 * Control and Status Register [ARM Cortex-M7 Devices Generic User's
 * Guide section 4.4.2].
 */
BIT_FIELD(SYSTICK_CTRL_ENABLE,      0, 0x00000001);
BIT_FIELD(SYSTICK_CTRL_TICKINT,     1, 0x00000002);
BIT_FIELD(SYSTICK_CTRL_CLKSOURCE,   2, 0x00000004);
BIT_FIELD(SYSTICK_CTRL_COUNTFLAG,   16, 0x00010000);

/**
 * Reload Value Register [ARM Cortex-M7 Devices Generic User's
 * Guide section 4.4.1].
 */
BIT_FIELD(SYSTICK_LOAD_RELOAD, 0, 0x00FFFFFF);

/**
 * Current Value Register [ARM Cortex-M7 Devices Generic User's
 * Guide section 4.4.3].
 */
BIT_FIELD(SYSTICK_VAL_CURRENT, 0, 0x00FFFFFF);

#endif