/**
 * @author Devon Andrade
 * @created 12/25/2016 (Merry Christmas!)
 *
 * Definitions and functions used to manipulate the GPIO module [6].
 */
#ifndef GPIO_H
#define GPIO_H

#include "registers/gpio_reg.h"
#include "status.h"

/**
 * Defines a digital state for a pin where a low == 0V, and high == VDD.
 */
typedef enum
{
    low = 0x0,
    high = 0x1
} DigitalState;

/**
 * GPIO Mode type defintions [6.4.1].
 */
typedef enum
{
    GPIO_INPUT = 0x0,
    GPIO_OUTPUT = 0x1,
    GPIO_ALT_FUNC = 0x2,
    GPIO_ANALOG = 0x3
} GpioMode;

/**
 * GPIO Output type definitions [6.4.2].
 */
typedef enum
{
    GPIO_PUSH_PULL = 0x0,
    GPIO_OPEN_DRAIN = 0x1
} GpioOType;

/**
 * GPIO Output speed definitions [6.4.3].
 */
typedef enum
{
    GPIO_OSPEED_4MHZ = 0x0,
    GPIO_OSPEED_25MHZ = 0x1,
    GPIO_OSPEED_50MHZ = 0x2,
    GPIO_OSPEED_100MHZ = 0x3
} GpioOSpeed;

/**
 * GPIO Pull state definitions [6.4.4].
 */
typedef enum
{
    GPIO_NO_PULL = 0x0,
    GPIO_PULL_UP = 0x1,
    GPIO_PULL_DOWN = 0x2
} GpioPull;

/**
 * Every available GPIO pin (all 168 of them).The 4 LSBs represent the pin
 * number. The 4 MSBs represent the port.
 *
 * So PI11 = 0x8B:
 *           0x8 = Port 8 (aka, Port I)
 *           0xB = Pin 11
 */
typedef enum
{
    PA0  = 0x00,
    PA1  = 0x01,
    PA2  = 0x02,
    PA3  = 0x03,
    PA4  = 0x04,
    PA5  = 0x05,
    PA6  = 0x06,
    PA7  = 0x07,
    PA8  = 0x08,
    PA9  = 0x09,
    PA10 = 0x0A,
    PA11 = 0x0B,
    PA12 = 0x0C,
    PA13 = 0x0D,
    PA14 = 0x0E,
    PA15 = 0x0F,

    PB0  = 0x10,
    PB1  = 0x11,
    PB2  = 0x12,
    PB3  = 0x13,
    PB4  = 0x14,
    PB5  = 0x15,
    PB6  = 0x16,
    PB7  = 0x17,
    PB8  = 0x18,
    PB9  = 0x19,
    PB10 = 0x1A,
    PB11 = 0x1B,
    PB12 = 0x1C,
    PB13 = 0x1D,
    PB14 = 0x1E,
    PB15 = 0x1F,

    PC0  = 0x20,
    PC1  = 0x21,
    PC2  = 0x22,
    PC3  = 0x23,
    PC4  = 0x24,
    PC5  = 0x25,
    PC6  = 0x26,
    PC7  = 0x27,
    PC8  = 0x28,
    PC9  = 0x29,
    PC10 = 0x2A,
    PC11 = 0x2B,
    PC12 = 0x2C,
    PC13 = 0x2D,
    PC14 = 0x2E,
    PC15 = 0x2F,

    PD0  = 0x30,
    PD1  = 0x31,
    PD2  = 0x32,
    PD3  = 0x33,
    PD4  = 0x34,
    PD5  = 0x35,
    PD6  = 0x36,
    PD7  = 0x37,
    PD8  = 0x38,
    PD9  = 0x39,
    PD10 = 0x3A,
    PD11 = 0x3B,
    PD12 = 0x3C,
    PD13 = 0x3D,
    PD14 = 0x3E,
    PD15 = 0x3F,

    PE0  = 0x40,
    PE1  = 0x41,
    PE2  = 0x42,
    PE3  = 0x43,
    PE4  = 0x44,
    PE5  = 0x45,
    PE6  = 0x46,
    PE7  = 0x47,
    PE8  = 0x48,
    PE9  = 0x49,
    PE10 = 0x4A,
    PE11 = 0x4B,
    PE12 = 0x4C,
    PE13 = 0x4D,
    PE14 = 0x4E,
    PE15 = 0x4F,

    PF0  = 0x50,
    PF1  = 0x51,
    PF2  = 0x52,
    PF3  = 0x53,
    PF4  = 0x54,
    PF5  = 0x55,
    PF6  = 0x56,
    PF7  = 0x57,
    PF8  = 0x58,
    PF9  = 0x59,
    PF10 = 0x5A,
    PF11 = 0x5B,
    PF12 = 0x5C,
    PF13 = 0x5D,
    PF14 = 0x5E,
    PF15 = 0x5F,

    PG0  = 0x60,
    PG1  = 0x61,
    PG2  = 0x62,
    PG3  = 0x63,
    PG4  = 0x64,
    PG5  = 0x65,
    PG6  = 0x66,
    PG7  = 0x67,
    PG8  = 0x68,
    PG9  = 0x69,
    PG10 = 0x6A,
    PG11 = 0x6B,
    PG12 = 0x6C,
    PG13 = 0x6D,
    PG14 = 0x6E,
    PG15 = 0x6F,

    PH0  = 0x70,
    PH1  = 0x71,
    PH2  = 0x72,
    PH3  = 0x73,
    PH4  = 0x74,
    PH5  = 0x75,
    PH6  = 0x76,
    PH7  = 0x77,
    PH8  = 0x78,
    PH9  = 0x79,
    PH10 = 0x7A,
    PH11 = 0x7B,
    PH12 = 0x7C,
    PH13 = 0x7D,
    PH14 = 0x7E,
    PH15 = 0x7F,

    PI0  = 0x80,
    PI1  = 0x81,
    PI2  = 0x82,
    PI3  = 0x83,
    PI4  = 0x84,
    PI5  = 0x85,
    PI6  = 0x86,
    PI7  = 0x87,
    PI8  = 0x88,
    PI9  = 0x89,
    PI10 = 0x8A,
    PI11 = 0x8B,
    PI12 = 0x8C,
    PI13 = 0x8D,
    PI14 = 0x8E,
    PI15 = 0x8F,

    PJ0  = 0x90,
    PJ1  = 0x91,
    PJ2  = 0x92,
    PJ3  = 0x93,
    PJ4  = 0x94,
    PJ5  = 0x95,
    PJ6  = 0x96,
    PJ7  = 0x97,
    PJ8  = 0x98,
    PJ9  = 0x99,
    PJ10 = 0x9A,
    PJ11 = 0x9B,
    PJ12 = 0x9C,
    PJ13 = 0x9D,
    PJ14 = 0x9E,
    PJ15 = 0x9F,

    PK0  = 0xA0,
    PK1  = 0xA1,
    PK2  = 0xA2,
    PK3  = 0xA3,
    PK4  = 0xA4,
    PK5  = 0xA5,
    PK6  = 0xA6,
    PK7  = 0xA7,
    NUM_GPIO_PINS
} GpioPin;

/**
 * Macros for retrieving the port and pin numbers from the above enumerations.
 */
#define GPIO_GET_PORT(x) (((uint8_t)x) >> 4)
#define GPIO_GET_PIN(x) (((uint8_t)x) & 0x0F)

/**
 * These macros are to be used as the first parameter to the GPIO functions.
 * This will pass the correct GPIO port register and pin number to the function
 * without the user having to specify the port twice (e.g., PA12 implies Port A
 * already). So, the user can do either of the following:
 *
 * gpio_set_output(GPIO_PA12, high)
 *
 * OR
 *
 * gpio_set_output(GPIOA, PA12, high)
 */
#define GPIO_PA0  GPIOA, PA0
#define GPIO_PA1  GPIOA, PA1
#define GPIO_PA2  GPIOA, PA2
#define GPIO_PA3  GPIOA, PA3
#define GPIO_PA4  GPIOA, PA4
#define GPIO_PA5  GPIOA, PA5
#define GPIO_PA6  GPIOA, PA6
#define GPIO_PA7  GPIOA, PA7
#define GPIO_PA8  GPIOA, PA8
#define GPIO_PA9  GPIOA, PA9
#define GPIO_PA10 GPIOA, PA10
#define GPIO_PA11 GPIOA, PA11
#define GPIO_PA12 GPIOA, PA12
#define GPIO_PA13 GPIOA, PA13
#define GPIO_PA14 GPIOA, PA14
#define GPIO_PA15 GPIOA, PA15

#define GPIO_PB0  GPIOB, PB0
#define GPIO_PB1  GPIOB, PB1
#define GPIO_PB2  GPIOB, PB2
#define GPIO_PB3  GPIOB, PB3
#define GPIO_PB4  GPIOB, PB4
#define GPIO_PB5  GPIOB, PB5
#define GPIO_PB6  GPIOB, PB6
#define GPIO_PB7  GPIOB, PB7
#define GPIO_PB8  GPIOB, PB8
#define GPIO_PB9  GPIOB, PB9
#define GPIO_PB10 GPIOB, PB10
#define GPIO_PB11 GPIOB, PB11
#define GPIO_PB12 GPIOB, PB12
#define GPIO_PB13 GPIOB, PB13
#define GPIO_PB14 GPIOB, PB14
#define GPIO_PB15 GPIOB, PB15

#define GPIO_PC0  GPIOC, PC0
#define GPIO_PC1  GPIOC, PC1
#define GPIO_PC2  GPIOC, PC2
#define GPIO_PC3  GPIOC, PC3
#define GPIO_PC4  GPIOC, PC4
#define GPIO_PC5  GPIOC, PC5
#define GPIO_PC6  GPIOC, PC6
#define GPIO_PC7  GPIOC, PC7
#define GPIO_PC8  GPIOC, PC8
#define GPIO_PC9  GPIOC, PC9
#define GPIO_PC10 GPIOC, PC10
#define GPIO_PC11 GPIOC, PC11
#define GPIO_PC12 GPIOC, PC12
#define GPIO_PC13 GPIOC, PC13
#define GPIO_PC14 GPIOC, PC14
#define GPIO_PC15 GPIOC, PC15

#define GPIO_PD0  GPIOD, PD0
#define GPIO_PD1  GPIOD, PD1
#define GPIO_PD2  GPIOD, PD2
#define GPIO_PD3  GPIOD, PD3
#define GPIO_PD4  GPIOD, PD4
#define GPIO_PD5  GPIOD, PD5
#define GPIO_PD6  GPIOD, PD6
#define GPIO_PD7  GPIOD, PD7
#define GPIO_PD8  GPIOD, PD8
#define GPIO_PD9  GPIOD, PD9
#define GPIO_PD10 GPIOD, PD10
#define GPIO_PD11 GPIOD, PD11
#define GPIO_PD12 GPIOD, PD12
#define GPIO_PD13 GPIOD, PD13
#define GPIO_PD14 GPIOD, PD14
#define GPIO_PD15 GPIOD, PD15

#define GPIO_PE0  GPIOE, PE0
#define GPIO_PE1  GPIOE, PE1
#define GPIO_PE2  GPIOE, PE2
#define GPIO_PE3  GPIOE, PE3
#define GPIO_PE4  GPIOE, PE4
#define GPIO_PE5  GPIOE, PE5
#define GPIO_PE6  GPIOE, PE6
#define GPIO_PE7  GPIOE, PE7
#define GPIO_PE8  GPIOE, PE8
#define GPIO_PE9  GPIOE, PE9
#define GPIO_PE10 GPIOE, PE10
#define GPIO_PE11 GPIOE, PE11
#define GPIO_PE12 GPIOE, PE12
#define GPIO_PE13 GPIOE, PE13
#define GPIO_PE14 GPIOE, PE14
#define GPIO_PE15 GPIOE, PE15

#define GPIO_PF0  GPIOF, PF0
#define GPIO_PF1  GPIOF, PF1
#define GPIO_PF2  GPIOF, PF2
#define GPIO_PF3  GPIOF, PF3
#define GPIO_PF4  GPIOF, PF4
#define GPIO_PF5  GPIOF, PF5
#define GPIO_PF6  GPIOF, PF6
#define GPIO_PF7  GPIOF, PF7
#define GPIO_PF8  GPIOF, PF8
#define GPIO_PF9  GPIOF, PF9
#define GPIO_PF10 GPIOF, PF10
#define GPIO_PF11 GPIOF, PF11
#define GPIO_PF12 GPIOF, PF12
#define GPIO_PF13 GPIOF, PF13
#define GPIO_PF14 GPIOF, PF14
#define GPIO_PF15 GPIOF, PF15

#define GPIO_PG0  GPIOG, PG0
#define GPIO_PG1  GPIOG, PG1
#define GPIO_PG2  GPIOG, PG2
#define GPIO_PG3  GPIOG, PG3
#define GPIO_PG4  GPIOG, PG4
#define GPIO_PG5  GPIOG, PG5
#define GPIO_PG6  GPIOG, PG6
#define GPIO_PG7  GPIOG, PG7
#define GPIO_PG8  GPIOG, PG8
#define GPIO_PG9  GPIOG, PG9
#define GPIO_PG10 GPIOG, PG10
#define GPIO_PG11 GPIOG, PG11
#define GPIO_PG12 GPIOG, PG12
#define GPIO_PG13 GPIOG, PG13
#define GPIO_PG14 GPIOG, PG14
#define GPIO_PG15 GPIOG, PG15

#define GPIO_PH0  GPIOH, PH0
#define GPIO_PH1  GPIOH, PH1
#define GPIO_PH2  GPIOH, PH2
#define GPIO_PH3  GPIOH, PH3
#define GPIO_PH4  GPIOH, PH4
#define GPIO_PH5  GPIOH, PH5
#define GPIO_PH6  GPIOH, PH6
#define GPIO_PH7  GPIOH, PH7
#define GPIO_PH8  GPIOH, PH8
#define GPIO_PH9  GPIOH, PH9
#define GPIO_PH10 GPIOH, PH10
#define GPIO_PH11 GPIOH, PH11
#define GPIO_PH12 GPIOH, PH12
#define GPIO_PH13 GPIOH, PH13
#define GPIO_PH14 GPIOH, PH14
#define GPIO_PH15 GPIOH, PH15

#define GPIO_PI0  GPIOI, PI0
#define GPIO_PI1  GPIOI, PI1
#define GPIO_PI2  GPIOI, PI2
#define GPIO_PI3  GPIOI, PI3
#define GPIO_PI4  GPIOI, PI4
#define GPIO_PI5  GPIOI, PI5
#define GPIO_PI6  GPIOI, PI6
#define GPIO_PI7  GPIOI, PI7
#define GPIO_PI8  GPIOI, PI8
#define GPIO_PI9  GPIOI, PI9
#define GPIO_PI10 GPIOI, PI10
#define GPIO_PI11 GPIOI, PI11
#define GPIO_PI12 GPIOI, PI12
#define GPIO_PI13 GPIOI, PI13
#define GPIO_PI14 GPIOI, PI14
#define GPIO_PI15 GPIOI, PI15

#define GPIO_PJ0  GPIOJ, PJ0
#define GPIO_PJ1  GPIOJ, PJ1
#define GPIO_PJ2  GPIOJ, PJ2
#define GPIO_PJ3  GPIOJ, PJ3
#define GPIO_PJ4  GPIOJ, PJ4
#define GPIO_PJ5  GPIOJ, PJ5
#define GPIO_PJ6  GPIOJ, PJ6
#define GPIO_PJ7  GPIOJ, PJ7
#define GPIO_PJ8  GPIOJ, PJ8
#define GPIO_PJ9  GPIOJ, PJ9
#define GPIO_PJ10 GPIOJ, PJ10
#define GPIO_PJ11 GPIOJ, PJ11
#define GPIO_PJ12 GPIOJ, PJ12
#define GPIO_PJ13 GPIOJ, PJ13
#define GPIO_PJ14 GPIOJ, PJ14
#define GPIO_PJ15 GPIOJ, PJ15

#define GPIO_PK0  GPIOK, PK0
#define GPIO_PK1  GPIOK, PK1
#define GPIO_PK2  GPIOK, PK2
#define GPIO_PK3  GPIOK, PK3
#define GPIO_PK4  GPIOK, PK4
#define GPIO_PK5  GPIOK, PK5
#define GPIO_PK6  GPIOK, PK6
#define GPIO_PK7  GPIOK, PK7

/**
 * Aliases to the above GPIO_Pxy macros for the I/O pins on the STM32F7
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

status_t gpio_request_input(GpioReg *reg, GpioPin pin, GpioPull pull);
status_t gpio_request_output(GpioReg *reg, GpioPin pin,
                             DigitalState default_state);
status_t gpio_request_alt(GpioReg *reg, GpioPin pin);

void gpio_set_otype(GpioReg *reg, GpioPin pin, GpioOType type);
void gpio_set_ospeed(GpioReg *reg, GpioPin pin, GpioOSpeed speed);
void gpio_set_pullstate(GpioReg *reg, GpioPin pin, GpioPull pull);

void gpio_set_output(GpioReg *reg, GpioPin pin, DigitalState state);
DigitalState gpio_get_input(GpioReg *reg, GpioPin pin);

#endif