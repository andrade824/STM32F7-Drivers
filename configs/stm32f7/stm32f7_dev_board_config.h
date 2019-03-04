/**
 * @author Devon Andrade
 * @created 1/27/2019
 *
 * Platform-specific definitions relating to the STM32F730R8 microcontroller
 * and my custom STM32F7 development board based around that micro.
 */
#ifndef STM32F7_DEV_BOARD_CONFIG_H
#define STM32F7_DEV_BOARD_CONFIG_H

#include <stdint.h>

/***** CLOCK SETTINGS *****/

/**
 * Bypass the HSE oscillator. Enable this bypass when an external oscillator is
 * used. Disable this when an external crystal is used and an oscillator is
 * still needed.
 */
#define HSE_BYPASS 0U

/***** GPIO CONFIGURATION *****/

/**
 * Ensure that the GPIO_Pxy macros have been defined before the following
 * macros.
 */
#include "gpio.h"

/**
 * Aliases to the GPIO_Pxy macros for the I/O pins on the STM32F7
 * development board.
 */
#define GPIO_SWDIO                  GPIO_PA13
#define GPIO_SWCLK                  GPIO_PA14
#define GPIO_SWO                    GPIO_PB3

#define GPIO_USD_D0                 GPIO_PC8
#define GPIO_USD_D1                 GPIO_PC9
#define GPIO_USD_D2                 GPIO_PC10
#define GPIO_USD_D3                 GPIO_PC11
#define GPIO_USD_CLK                GPIO_PC12
#define GPIO_USD_CMD                GPIO_PD2

#define GPIO_BTN_USER               GPIO_PA3
#define GPIO_LED_USER               GPIO_PA4

/***** INTERRUPT CONFIGURATION *****/

/**
 * The start address of the vector table. This is located in tightly coupled
 * instruction RAM.
 */
#define VECTOR_TABLE_ADDR 0x0

/**
 * Interrupt Request Numbers.
 *
 * The CMSIS interrupt methods assume that the exception values are negative
 * and that the STM32 specific interrupts are positive (starting at zero).
 *
 * To use the IRQ values as indices into the vector table, the vector table
 * will need to point to vector 0 (the vector for "WWDG_IRQn"). This way, the
 * negative indices will correctly be evaluated.
 */
typedef enum
{
	IRQ_START                   = -15,

	/******  Cortex-M7 Processor Exceptions Numbers ****************************************************************/
	NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                          */
	HardFault_IRQn              = -13,    /*!< 3 Cortex-M7 Hard Fault Interrupt                                  */
	MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M7 Memory Management Interrupt                           */
	BusFault_IRQn               = -11,    /*!< 5 Cortex-M7 Bus Fault Interrupt                                   */
	UsageFault_IRQn             = -10,    /*!< 6 Cortex-M7 Usage Fault Interrupt                                 */
	SVCall_IRQn                 = -5,     /*!< 11 Cortex-M7 SV Call Interrupt                                    */
	DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M7 Debug Monitor Interrupt                              */
	PendSV_IRQn                 = -2,     /*!< 14 Cortex-M7 Pend SV Interrupt                                    */
	SysTick_IRQn                = -1,     /*!< 15 Cortex-M7 System Tick Interrupt                                */
	/******  STM32 specific Interrupt Numbers **********************************************************************/
	WWDG_IRQn                   = 0,      /*!< Window WatchDog Interrupt                                         */
	PVD_IRQn                    = 1,      /*!< PVD through EXTI Line detection Interrupt                         */
	TAMP_STAMP_IRQn             = 2,      /*!< Tamper and TimeStamp interrupts through the EXTI line             */
	RTC_WKUP_IRQn               = 3,      /*!< RTC Wakeup interrupt through the EXTI line                        */
	FLASH_IRQn                  = 4,      /*!< FLASH global Interrupt                                            */
	RCC_IRQn                    = 5,      /*!< RCC global Interrupt                                              */
	EXTI0_IRQn                  = 6,      /*!< EXTI Line0 Interrupt                                              */
	EXTI1_IRQn                  = 7,      /*!< EXTI Line1 Interrupt                                              */
	EXTI2_IRQn                  = 8,      /*!< EXTI Line2 Interrupt                                              */
	EXTI3_IRQn                  = 9,      /*!< EXTI Line3 Interrupt                                              */
	EXTI4_IRQn                  = 10,     /*!< EXTI Line4 Interrupt                                              */
	DMA1_Stream0_IRQn           = 11,     /*!< DMA1 Stream 0 global Interrupt                                    */
	DMA1_Stream1_IRQn           = 12,     /*!< DMA1 Stream 1 global Interrupt                                    */
	DMA1_Stream2_IRQn           = 13,     /*!< DMA1 Stream 2 global Interrupt                                    */
	DMA1_Stream3_IRQn           = 14,     /*!< DMA1 Stream 3 global Interrupt                                    */
	DMA1_Stream4_IRQn           = 15,     /*!< DMA1 Stream 4 global Interrupt                                    */
	DMA1_Stream5_IRQn           = 16,     /*!< DMA1 Stream 5 global Interrupt                                    */
	DMA1_Stream6_IRQn           = 17,     /*!< DMA1 Stream 6 global Interrupt                                    */
	ADC_IRQn                    = 18,     /*!< ADC1, ADC2 and ADC3 global Interrupts                             */
	CAN1_TX_IRQn                = 19,     /*!< CAN1 TX Interrupt                                                 */
	CAN1_RX0_IRQn               = 20,     /*!< CAN1 RX0 Interrupt                                                */
	CAN1_RX1_IRQn               = 21,     /*!< CAN1 RX1 Interrupt                                                */
	CAN1_SCE_IRQn               = 22,     /*!< CAN1 SCE Interrupt                                                */
	EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                                     */
	TIM1_BRK_TIM9_IRQn          = 24,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
	TIM1_UP_TIM10_IRQn          = 25,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
	TIM1_TRG_COM_TIM11_IRQn     = 26,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
	TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
	TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                             */
	TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
	TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                             */
	I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                              */
	I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                              */
	I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                              */
	I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                              */
	SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                             */
	SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                             */
	USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                                           */
	USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                                           */
	USART3_IRQn                 = 39,     /*!< USART3 global Interrupt                                           */
	EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
	RTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
	OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
	TIM8_BRK_TIM12_IRQn         = 43,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
	TIM8_UP_TIM13_IRQn          = 44,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
	TIM8_TRG_COM_TIM14_IRQn     = 45,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
	TIM8_CC_IRQn                = 46,     /*!< TIM8 Capture Compare Interrupt                                    */
	DMA1_Stream7_IRQn           = 47,     /*!< DMA1 Stream7 Interrupt                                            */
	FMC_IRQn                    = 48,     /*!< FMC global Interrupt                                              */
	SDMMC1_IRQn                 = 49,     /*!< SDMMC1 global Interrupt                                           */
	TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                             */
	SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                             */
	UART4_IRQn                  = 52,     /*!< UART4 global Interrupt                                            */
	UART5_IRQn                  = 53,     /*!< UART5 global Interrupt                                            */
	TIM6_DAC_IRQn               = 54,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
	TIM7_IRQn                   = 55,     /*!< TIM7 global interrupt                                             */
	DMA2_Stream0_IRQn           = 56,     /*!< DMA2 Stream 0 global Interrupt                                    */
	DMA2_Stream1_IRQn           = 57,     /*!< DMA2 Stream 1 global Interrupt                                    */
	DMA2_Stream2_IRQn           = 58,     /*!< DMA2 Stream 2 global Interrupt                                    */
	DMA2_Stream3_IRQn           = 59,     /*!< DMA2 Stream 3 global Interrupt                                    */
	DMA2_Stream4_IRQn           = 60,     /*!< DMA2 Stream 4 global Interrupt                                    */
	ETH_IRQn                    = 61,     /*!< Ethernet global Interrupt                                         */
	ETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
	OTG_FS_IRQn                 = 67,     /*!< USB OTG FS global Interrupt                                       */
	DMA2_Stream5_IRQn           = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
	DMA2_Stream6_IRQn           = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
	DMA2_Stream7_IRQn           = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
	USART6_IRQn                 = 71,     /*!< USART6 global interrupt                                           */
	I2C3_EV_IRQn                = 72,     /*!< I2C3 event interrupt                                              */
	I2C3_ER_IRQn                = 73,     /*!< I2C3 error interrupt                                              */
	OTG_HS_EP1_OUT_IRQn         = 74,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
	OTG_HS_EP1_IN_IRQn          = 75,     /*!< USB OTG HS End Point 1 In global interrupt                        */
	OTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
	OTG_HS_IRQn                 = 77,     /*!< USB OTG HS global interrupt                                       */
	AES_IRQn                    = 79,     /*!< AES global interrupt                                              */
	RNG_IRQn                    = 80,     /*!< RNG global interrupt                                              */
	FPU_IRQn                    = 81,     /*!< FPU global interrupt                                              */
	UART7_IRQn                  = 82,     /*!< UART7 global interrupt                                            */
	UART8_IRQn                  = 83,     /*!< UART8 global interrupt                                            */
	SPI4_IRQn                   = 84,     /*!< SPI4 global Interrupt                                             */
	SPI5_IRQn                   = 85,     /*!< SPI5 global Interrupt                                             */
	SAI1_IRQn                   = 87,     /*!< SAI1 global Interrupt                                             */
	SAI2_IRQn                   = 91,     /*!< SAI2 global Interrupt                                             */
	QUADSPI_IRQn                = 92,     /*!< Quad SPI global interrupt                                         */
	LPTIM1_IRQn                 = 93,     /*!< LP TIM1 interrupt                                                 */
	SDMMC2_IRQn                 = 103,    /*!< SDMMC2 global Interrupt                                           */

	IRQ_END
} IRQn_Type;

/***** INCLUDE GENERIC STM32F7 CONFIGURATION *****/

/**
 * This needs to be included after IRQn_Type is defined as that is needed by
 * the included CMSIS header file.
 */
#include "stm32f7_generic.h"

#endif /* STM32F7_DEV_BOARD_CONFIG_H */
