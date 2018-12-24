/**
 * @author Devon Andrade
 * @created 12/6/2018
 *
 * Platform-specific definitions relating to the STM32F746 microcontroller
 * and the STM32F746 Discovery board.
 */
#ifndef STM32F746_DISCO_CONFIG_H
#define STM32F746_DISCO_CONFIG_H

/***** CLOCK SETTINGS *****/

/**
 * Clock settings. The user needs to ensure that init_clocks() in "system.c"
 * generates clocks with these frequencies.
 */
#define CPU_HZ 216000000
#define APB2_HZ 108000000
#define APB1_HZ 54000000
#define AHB_HZ CPU_HZ
#define FLASH_WAIT_STATES 7

/**
 * PLL and bus divider settings used to get the clock values above.
 *
 * CPU_HZ = (fInput * (PLLN / PLLM)) / PLLP
 * AHB_HZ = CPU_HZ
 * APB2_HZ = AHB_HZ / APB2_DIV
 * APB1_HZ = AHB_HZ / APB1_DIV
 */
#define CLK_PLLQ 4
#define CLK_PLLM 25
#define CLK_PLLN 432
#define CLK_PLLP 0        /* Divide by 2 */

#define CLK_APB1_DIV 5    /* Divide by 4 */
#define CLK_APB2_DIV 4    /* Divide by 2 */

/**
 * Clock settings used for the LCD pixel clock.
 *
 * PLLSAI = fInput * (PLLSAIN / PLLM)
 * PLLSAIR = PLLSAI / PLLSAIR
 * Pixel Clock = PLLSAIR / PLLSAIDIVR
 *
 * Pixel Clock = ((25MHz * (192 / 25)) / 5) / 4 = 9.6MHz
 */
#define CLK_PLLSAI_PLLN 192
#define CLK_PLLSAIR     5
#define CLK_PLLSAIDIVR  1 /* Divide by 4 */

/***** GPIO CONFIGURATION *****/

/**
 * Ensure that the GPIO_Pxy macros have been defined before the following
 * macros.
 */
#include "gpio.h"

/**
 * Aliases to the GPIO_Pxy macros for the I/O pins on the STM32F7
 * discovery board.
 */
#define GPIO_ARD_A0                 GPIO_PA0
#define GPIO_RMII_REF_CLK           GPIO_PA1
#define GPIO_RMII_MDIO              GPIO_PA2
#define GPIO_ULPI_D0                GPIO_PA3
#define GPIO_DCMI_HSYNC             GPIO_PA4
#define GPIO_ULPI_CK                GPIO_PA5
#define GPIO_DCMI_PIXCK             GPIO_PA6
#define GPIO_RMII_CRS_DV            GPIO_PA7
#define GPIO_ARD_D10                GPIO_PA8
#define GPIO_VCP_TX                 GPIO_PA9
#define GPIO_OTG_FS_ID              GPIO_PA10
#define GPIO_OTG_FS_N               GPIO_PA11
#define GPIO_OTG_FS_P               GPIO_PA12
#define GPIO_SWDIO                  GPIO_PA13
#define GPIO_SWCLK                  GPIO_PA14
#define GPIO_ARD_D9                 GPIO_PA15

#define GPIO_ULPI_D1                GPIO_PB0
#define GPIO_ULPI_D2                GPIO_PB1
#define GPIO_QSPI_CLK               GPIO_PB2
#define GPIO_SWO                    GPIO_PB3
#define GPIO_ARD_D3                 GPIO_PB4
#define GPIO_ULPI_D7                GPIO_PB5
#define GPIO_QSPI_NCS               GPIO_PB6
#define GPIO_VCP_RX                 GPIO_PB7
#define GPIO_DCMI_SCL               GPIO_PB8
#define GPIO_DCMI_SDA               GPIO_PB9
#define GPIO_ULPI_D3                GPIO_PB10
#define GPIO_ULPI_D4                GPIO_PB11
#define GPIO_ULPI_D5                GPIO_PB12
#define GPIO_ULIP_D6                GPIO_PB13
#define GPIO_ARD_D12                GPIO_PB14
#define GPIO_ARD_D11                GPIO_PB15

#define GPIO_ULPI_STP               GPIO_PC0
#define GPIO_RMII_MDC               GPIO_PC1
#define GPIO_ULPI_DIR               GPIO_PC2
#define GPIO_FMC_SDCKE0             GPIO_PC3
#define GPIO_RMII_RXD0              GPIO_PC4
#define GPIO_RMII_RXD1              GPIO_PC5
#define GPIO_ARD_D1                 GPIO_PC6
#define GPIO_ARD_D0                 GPIO_PC7
#define GPIO_USD_D0                 GPIO_PC8
#define GPIO_USD_D1                 GPIO_PC9
#define GPIO_USD_D2                 GPIO_PC10
#define GPIO_USD_D3                 GPIO_PC11
#define GPIO_USD_CLK                GPIO_PC12
#define GPIO_USD_DETECT             GPIO_PC13
#define GPIO_OSC32_IN               GPIO_PC14
#define GPIO_OSC32_OUT              GPIO_PC15

#define GPIO_FMC_D2                 GPIO_PD0
#define GPIO_FMC_D3                 GPIO_PD1
#define GPIO_USD_CMD                GPIO_PD2
#define GPIO_DCMI_D5                GPIO_PD3
#define GPIO_OTG_FS_OVERCURRENT     GPIO_PD4
#define GPIO_OTG_FS_POWERSWITCHON   GPIO_PD5
#define GPIO_AUDIO_INT              GPIO_PD6
#define GPIO_SPDIF_RX0              GPIO_PD7
#define GPIO_FMC_D13                GPIO_PD8
#define GPIO_FMC_D14                GPIO_PD9
#define GPIO_FMC_D15                GPIO_PD10
#define GPIO_QSPI_D0                GPIO_PD11
#define GPIO_QSPI_D1                GPIO_PD12
#define GPIO_QSPI_D3                GPIO_PD13
#define GPIO_FMC_D0                 GPIO_PD14
#define GPIO_FMC_D1                 GPIO_PD15

#define GPIO_FMC_NBL0               GPIO_PE0
#define GPIO_FMC_NBL1               GPIO_PE1
#define GPIO_QSPI_D2                GPIO_PE2
#define GPIO_OTG_HS_OVERCURRENT     GPIO_PE3
#define GPIO_LCD_B0                 GPIO_PE4
#define GPIO_DCMI_D6                GPIO_PE5
#define GPIO_DCMI_D7                GPIO_PE6
#define GPIO_FMC_D4                 GPIO_PE7
#define GPIO_FMC_D5                 GPIO_PE8
#define GPIO_FMC_D6                 GPIO_PE9
#define GPIO_FMC_D7                 GPIO_PE10
#define GPIO_FMC_D8                 GPIO_PE11
#define GPIO_FMC_D9                 GPIO_PE12
#define GPIO_FMC_D10                GPIO_PE13
#define GPIO_FMC_D11                GPIO_PE14
#define GPIO_FMC_D12                GPIO_PE15

#define GPIO_FMC_A0                 GPIO_PF0
#define GPIO_FMC_A1                 GPIO_PF1
#define GPIO_FMC_A2                 GPIO_PF2
#define GPIO_FMC_A3                 GPIO_PF3
#define GPIO_FMC_A4                 GPIO_PF4
#define GPIO_FMC_A5                 GPIO_PF5
#define GPIO_ARD_A5                 GPIO_PF6
#define GPIO_ARD_A4                 GPIO_PF7
#define GPIO_ARD_A3                 GPIO_PF8
#define GPIO_ARD_A2                 GPIO_PF9
#define GPIO_ARD_A1                 GPIO_PF10
#define GPIO_FMC_SDNRAS             GPIO_PF11
#define GPIO_FMC_A6                 GPIO_PF12
#define GPIO_FMC_A7                 GPIO_PF13
#define GPIO_FMC_A8                 GPIO_PF14
#define GPIO_FMC_A9                 GPIO_PF15

#define GPIO_FMC_A10                GPIO_PG0
#define GPIO_FMC_A11                GPIO_PG1
#define GPIO_RMII_RXER              GPIO_PG2
#define GPIO_EXT_RST                GPIO_PG3
#define GPIO_FMC_BA0                GPIO_PG4
#define GPIO_FMC_BA1                GPIO_PG5
#define GPIO_ARD_D2                 GPIO_PG6
#define GPIO_ARD_D4                 GPIO_PG7
#define GPIO_FMC_SDCLK              GPIO_PG8
#define GPIO_DCMI_VSYNC             GPIO_PG9
#define GPIO_SAI2_SDB               GPIO_PG10
#define GPIO_RMII_TX_EN             GPIO_PG11
#define GPIO_LCD_B4                 GPIO_PG12
#define GPIO_RMII_TXD0              GPIO_PG13
#define GPIO_RMII_TXD1              GPIO_PG14
#define GPIO_FMC_SDNCAS             GPIO_PG15

#define GPIO_OSC_IN                 GPIO_PH0
#define GPIO_OSC_OUT                GPIO_PH1
#define GPIO_NC2                    GPIO_PH2
#define GPIO_FMC_SDNE0              GPIO_PH3
#define GPIO_ULPI_NXT               GPIO_PH4
#define GPIO_FMC_SDNWE              GPIO_PH5
#define GPIO_ARD_D6                 GPIO_PH6
#define GPIO_LCD_SCL                GPIO_PH7
#define GPIO_LCD_SDA                GPIO_PH8
#define GPIO_DCMI_D0                GPIO_PH9
#define GPIO_DCMI_D1                GPIO_PH10
#define GPIO_DCMI_D2                GPIO_PH11
#define GPIO_DCMI_D3                GPIO_PH12
#define GPIO_DCMI_PWR_EN            GPIO_PH13
#define GPIO_DCMI_D4                GPIO_PH14
#define GPIO_TP_PH15                GPIO_PH15

#define GPIO_ARD_D5                 GPIO_PI0
#define GPIO_ARD_D13                GPIO_PI1
#define GPIO_ARD_D8                 GPIO_PI2
#define GPIO_ARD_D7                 GPIO_PI3
#define GPIO_SAI2_MCLKA             GPIO_PI4
#define GPIO_SAI2_SCKA              GPIO_PI5
#define GPIO_SAI2_SDA               GPIO_PI6
#define GPIO_SAI2_FSA               GPIO_PI7
#define GPIO_NC1                    GPIO_PI8
#define GPIO_LCD_VSYNC              GPIO_PI9
#define GPIO_LCD_HSYNC              GPIO_PI10
#define GPIO_B_USER                 GPIO_PI11
#define GPIO_LCD_DISP               GPIO_PI12
#define GPIO_LCD_INT                GPIO_PI13
#define GPIO_LCD_CLK                GPIO_PI14
#define GPIO_LCD_R0                 GPIO_PI15

#define GPIO_LCD_R1                 GPIO_PJ0
#define GPIO_LCD_R2                 GPIO_PJ1
#define GPIO_LCD_R3                 GPIO_PJ2
#define GPIO_LCD_R4                 GPIO_PJ3
#define GPIO_LCD_R5                 GPIO_PJ4
#define GPIO_LCD_R6                 GPIO_PJ5
#define GPIO_LCD_R7                 GPIO_PJ6
#define GPIO_LCD_G0                 GPIO_PJ7
#define GPIO_LCD_G1                 GPIO_PJ8
#define GPIO_LCD_G2                 GPIO_PJ9
#define GPIO_LCD_G3                 GPIO_PJ10
#define GPIO_LCD_G4                 GPIO_PJ11
#define GPIO_OTG_FS_VBUS            GPIO_PJ12
#define GPIO_LCD_B1                 GPIO_PJ13
#define GPIO_LCD_B2                 GPIO_PJ14
#define GPIO_LCD_B3                 GPIO_PJ15

#define GPIO_LCD_G5                 GPIO_PK0
#define GPIO_LCD_G6                 GPIO_PK1
#define GPIO_LCD_G7                 GPIO_PK2
#define GPIO_LCD_BL_CTRL            GPIO_PK3
#define GPIO_LCD_B5                 GPIO_PK4
#define GPIO_LCD_B6                 GPIO_PK5
#define GPIO_LCD_B7                 GPIO_PK6
#define GPIO_LCD_DE                 GPIO_PK7

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

	/******  Cortex-M7 Processor Exceptions Numbers **************************************************************/
	NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                          */
	HardFault_IRQn              = -13,    /*!< 3 Cortex-M7 Hard Fault Interrupt                                  */
	MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M7 Memory Management Interrupt                           */
	BusFault_IRQn               = -11,    /*!< 5 Cortex-M7 Bus Fault Interrupt                                   */
	UsageFault_IRQn             = -10,    /*!< 6 Cortex-M7 Usage Fault Interrupt                                 */
	SVCall_IRQn                 = -5,     /*!< 11 Cortex-M7 SV Call Interrupt                                    */
	DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M7 Debug Monitor Interrupt                              */
	PendSV_IRQn                 = -2,     /*!< 14 Cortex-M7 Pend SV Interrupt                                    */
	SysTick_IRQn                = -1,     /*!< 15 Cortex-M7 System Tick Interrupt                                */
	/******  STM32 specific Interrupt Numbers ********************************************************************/
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
	CAN2_TX_IRQn                = 63,     /*!< CAN2 TX Interrupt                                                 */
	CAN2_RX0_IRQn               = 64,     /*!< CAN2 RX0 Interrupt                                                */
	CAN2_RX1_IRQn               = 65,     /*!< CAN2 RX1 Interrupt                                                */
	CAN2_SCE_IRQn               = 66,     /*!< CAN2 SCE Interrupt                                                */
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
	DCMI_IRQn                   = 78,     /*!< DCMI global interrupt                                             */
	RNG_IRQn                    = 80,     /*!< RNG global interrupt                                              */
	FPU_IRQn                    = 81,     /*!< FPU global interrupt                                              */
	UART7_IRQn                  = 82,     /*!< UART7 global interrupt                                            */
	UART8_IRQn                  = 83,     /*!< UART8 global interrupt                                            */
	SPI4_IRQn                   = 84,     /*!< SPI4 global Interrupt                                             */
	SPI5_IRQn                   = 85,     /*!< SPI5 global Interrupt                                             */
	SPI6_IRQn                   = 86,     /*!< SPI6 global Interrupt                                             */
	SAI1_IRQn                   = 87,     /*!< SAI1 global Interrupt                                             */
	LTDC_IRQn                   = 88,     /*!< LTDC global Interrupt                                             */
	LTDC_ER_IRQn                = 89,     /*!< LTDC Error global Interrupt                                       */
	DMA2D_IRQn                  = 90,     /*!< DMA2D global Interrupt                                            */
	SAI2_IRQn                   = 91,     /*!< SAI2 global Interrupt                                             */
	QUADSPI_IRQn                = 92,     /*!< Quad SPI global interrupt                                         */
	LPTIM1_IRQn                 = 93,     /*!< LP TIM1 interrupt                                                 */
	CEC_IRQn                    = 94,     /*!< HDMI-CEC global Interrupt                                         */
	I2C4_EV_IRQn                = 95,     /*!< I2C4 Event Interrupt                                              */
	I2C4_ER_IRQn                = 96,     /*!< I2C4 Error Interrupt                                              */
	SPDIF_RX_IRQn               = 97,     /*!< SPDIF-RX global Interrupt                                         */

	IRQ_END                     = 98
} IRQn_Type;

/***** SDRAM CONFIGURATION *****/

/**
 * Timings and settings used for the SDRAM chip (MT48LC4M32B2) on the STM32F7
 * Discovery board.
 */
#ifdef INCLUDE_SDRAM_DRIVER

#include "system_timer.h"

/**
 *  The SDRAM chip on the STM32F7 Discovery board is connected to the first
 *  SDRAM bank (the FMC's bank 5).
 */
#define SDRAM_FMC_BANK 0

/**
 * The standard SDRAM delay used during initialization.
 */
#define SDRAM_INIT_DELAY USECS(100)

/* SDRAM Control Register settings */
#define SDCR_SDCLK  0x3 /* The SDRAM clock is 1/3rd of the CPU clock (72MHz) */
#define SDCR_CAS    0x2 /* Two clock cycle latency, must match SDCMR_MRD bits */
#define SDCR_NB     0x1 /* Four internal memory banks */
#define SDCR_MWID   0x1 /* 16-bit memory width */
#define SDCR_NR     0x1 /* 12-bit Row Address */
#define SDCR_NC     0x0 /* 8-bit Column Address */

/**
 * These SDRAM timings were calculated using a CPU clock of 216MHz and a memory
 * clock of 108MHz.
 *
 * The number of cycles each parameter uses is one more than its value.
 *
 * For Example: SDTR_TRCD is "0x1" or 2 cycles.
 */
#define SDTR_TRCD   0x1 /* Row to column delay */
#define SDTR_TRP    0x1 /* Row precharge delay */
#define SDTR_TWR    0x1 /* Write recovery delay */
#define SDTR_TRC    0x4 /* Row cycle delay */
#define SDTR_TRAS   0x3 /* Self refresh time */
#define SDTR_TXSR   0x5 /* Exit Self-refresh delay */
#define SDTR_TMRD   0x1 /* Load Mode Register to Active */

/* SDRAM Command Mode Register settings */
#define SDCMR_CTB1 0x1  /* Only perform commands on the first bank */
#define SDCMR_CTB2 0x0
#define SDCMR_NRFS 0x7  /* Send 8 Auto-Refresh commands */
#define SDCMR_MRD 0x220 /* CAS Latency = 2 cycles, Write Burst = Single */

/**
 * Refresh Timer Register [13.7.5]
 *
 * The datasheet is a little confusing as to how to compute this value. By
 * reverse engineering their example, this is the equation I've come up with:
 *
 * SDRTR = (((Refresh Rate) / (Number of Rows)) * (SDRAM Clock Frequency)) - 20
 *
 * The refresh rate must be increased by 20 SDRAM clock cycles to obtain a safe
 * margin if an internal refresh occurs when a read request has been accepted.
 *
 * For this device:
 * SDRTR = ((64ms / 4096 rows) * 108MHz) - 20 = TRUNCATE(1667.5) = 1667
 */
#define SDRTR_COUNT 1667

#endif /* INCLUDE_SDRAM_DRIVER */

/***** LCD TIMING CONFIGURATION *****/

#ifdef INCLUDE_LCD_CTRL_DRIVER

/* Required for polarity declarations. */
#include "lcd_ctrl.h"

/* Timings and settings used for the LCD on the STM32F7 Discovery board. */
#define LCD_CONFIG_HSYNC      1U
#define LCD_CONFIG_VSYNC      1U
#define LCD_CONFIG_HORIZ_BP   43U
#define LCD_CONFIG_VERT_BP    12U
#define LCD_CONFIG_WIDTH      480U
#define LCD_CONFIG_HEIGHT     272U
#define LCD_CONFIG_HORIZ_FP   8U
#define LCD_CONFIG_VERT_FP    4U
#define LCD_CONFIG_HSYNC_POL  ACTIVE_LOW
#define LCD_CONFIG_VSYNC_POL  ACTIVE_LOW
#define LCD_CONFIG_DE_POL     ACTIVE_LOW
#define LCD_CONFIG_CLK_POL    ACTIVE_LOW
#define LCD_CONFIG_PIXEL_SIZE 4U
#endif /* INCLUDE_LCD_CTRL_DRIVER */

/***** INCLUDE GENERIC STM32F7 CONFIGURATION *****/

/**
 * This needs to be included after IRQn_Type is defined as that is needed by
 * the included CMSIS header file.
 */
#include "stm32f7_generic.h"

#endif /* STM32F746_DISCO_CONFIG_H */
