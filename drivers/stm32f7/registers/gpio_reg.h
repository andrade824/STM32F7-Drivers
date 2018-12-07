/**
 * @author Devon Andrade
 * @created 12/25/2016 (Merry Christmas!)
 *
 * GPIO Register map [6].
 */
#ifndef GPIO_REG_H
#define GPIO_REG_H

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

/**
 * Type defining the GPIO module register map.
 */
typedef struct
{
    volatile uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
    volatile uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
    volatile uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
    volatile uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    volatile uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
    volatile uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
    volatile uint32_t BSRR;     /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
    volatile uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
    volatile uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
} GpioReg;

/**
 * Define the GPIO register map accessors.
 */
#define GPIOA_BASE (AHB1PERIPH_BASE + 0x0000U)
#define GPIOB_BASE (AHB1PERIPH_BASE + 0x0400U)
#define GPIOC_BASE (AHB1PERIPH_BASE + 0x0800U)
#define GPIOD_BASE (AHB1PERIPH_BASE + 0x0C00U)
#define GPIOE_BASE (AHB1PERIPH_BASE + 0x1000U)
#define GPIOF_BASE (AHB1PERIPH_BASE + 0x1400U)
#define GPIOG_BASE (AHB1PERIPH_BASE + 0x1800U)
#define GPIOH_BASE (AHB1PERIPH_BASE + 0x1C00U)
#define GPIOI_BASE (AHB1PERIPH_BASE + 0x2000U)
#define GPIOJ_BASE (AHB1PERIPH_BASE + 0x2400U)
#define GPIOK_BASE (AHB1PERIPH_BASE + 0x2800U)
#define GPIOA ((GpioReg *) GPIOA_BASE)
#define GPIOB ((GpioReg *) GPIOB_BASE)
#define GPIOC ((GpioReg *) GPIOC_BASE)
#define GPIOD ((GpioReg *) GPIOD_BASE)
#define GPIOE ((GpioReg *) GPIOE_BASE)
#define GPIOF ((GpioReg *) GPIOF_BASE)
#define GPIOG ((GpioReg *) GPIOG_BASE)
#define GPIOH ((GpioReg *) GPIOH_BASE)
#define GPIOI ((GpioReg *) GPIOI_BASE)
#define GPIOJ ((GpioReg *) GPIOJ_BASE)
#define GPIOK ((GpioReg *) GPIOK_BASE)

#endif
