/**
 * @author Devon Andrade
 * @created 6/21/2018
 *
 * LCD Controller Register Map [18.7].
 */
#ifdef INCLUDE_LCD_CTRL_DRIVER
#ifndef LCD_CTRL_REG_H
#define LCD_CTRL_REG_H

#include "bitfield.h"
#include "platform.h"

#include <stdint.h>

/**
 * Type defining the LCD-TFT Display Controller module register map.
 */
typedef struct
{
  uint32_t          RES0[2];   /*!< Reserved, 0x00-0x04 */
  volatile uint32_t SSCR;      /*!< LTDC Synchronization Size Configuration Register,    Address offset: 0x08 */
  volatile uint32_t BPCR;      /*!< LTDC Back Porch Configuration Register,              Address offset: 0x0C */
  volatile uint32_t AWCR;      /*!< LTDC Active Width Configuration Register,            Address offset: 0x10 */
  volatile uint32_t TWCR;      /*!< LTDC Total Width Configuration Register,             Address offset: 0x14 */
  volatile uint32_t GCR;       /*!< LTDC Global Control Register,                        Address offset: 0x18 */
  uint32_t          RES1[2];   /*!< Reserved, 0x1C-0x20 */
  volatile uint32_t SRCR;      /*!< LTDC Shadow Reload Configuration Register,           Address offset: 0x24 */
  uint32_t          RES2[1];   /*!< Reserved, 0x28 */
  volatile uint32_t BCCR;      /*!< LTDC Background Color Configuration Register,        Address offset: 0x2C */
  uint32_t          RES3[1];   /*!< Reserved, 0x30 */
  volatile uint32_t IER;       /*!< LTDC Interrupt Enable Register,                      Address offset: 0x34 */
  volatile uint32_t ISR;       /*!< LTDC Interrupt Status Register,                      Address offset: 0x38 */
  volatile uint32_t ICR;       /*!< LTDC Interrupt Clear Register,                       Address offset: 0x3C */
  volatile uint32_t LIPCR;     /*!< LTDC Line Interrupt Position Configuration Register, Address offset: 0x40 */
  volatile uint32_t CPSR;      /*!< LTDC Current Position Status Register,               Address offset: 0x44 */
  volatile uint32_t CDSR;      /*!< LTDC Current Display Status Register,                 Address offset: 0x48 */
} LtdcReg;

/**
 * LCD-TFT Display registers for each layer.
 */
typedef struct
{
  volatile uint32_t CR;        /*!< LTDC Layerx Control Register                                  Address offset: 0x84 */
  volatile uint32_t WHPCR;     /*!< LTDC Layerx Window Horizontal Position Configuration Register Address offset: 0x88 */
  volatile uint32_t WVPCR;     /*!< LTDC Layerx Window Vertical Position Configuration Register   Address offset: 0x8C */
  volatile uint32_t CKCR;      /*!< LTDC Layerx Color Keying Configuration Register               Address offset: 0x90 */
  volatile uint32_t PFCR;      /*!< LTDC Layerx Pixel Format Configuration Register               Address offset: 0x94 */
  volatile uint32_t CACR;      /*!< LTDC Layerx Constant Alpha Configuration Register             Address offset: 0x98 */
  volatile uint32_t DCCR;      /*!< LTDC Layerx Default Color Configuration Register              Address offset: 0x9C */
  volatile uint32_t BFCR;      /*!< LTDC Layerx Blending Factors Configuration Register           Address offset: 0xA0 */
  uint32_t          RES0[2];   /*!< Reserved */
  volatile uint32_t CFBAR;     /*!< LTDC Layerx Color Frame Buffer Address Register               Address offset: 0xAC */
  volatile uint32_t CFBLR;     /*!< LTDC Layerx Color Frame Buffer Length Register                Address offset: 0xB0 */
  volatile uint32_t CFBLNR;    /*!< LTDC Layerx ColorFrame Buffer Line Number Register            Address offset: 0xB4 */
  uint32_t          RES1[3];   /*!< Reserved */
  volatile uint32_t CLUTWR;    /*!< LTDC Layerx CLUT Write Register                               Address offset: 0x144 */

} LtdcLayerReg;

/**
 * Define the LTDC Controller register map accessor.
 */
#define LTDC_BASE           (APB2PERIPH_BASE + 0x6800U)
#define LTDC_LAYER1_BASE    (LTDC_BASE + 0x84U)
#define LTDC_LAYER2_BASE    (LTDC_BASE + 0x104U)

#define LTDC                ((LtdcReg *) LTDC_BASE)
#define LTDC_LAYER1         ((LtdcLayerReg *) LTDC_LAYER1_BASE)
#define LTDC_LAYER2         ((LtdcLayerReg *) LTDC_LAYER2_BASE)

/**
 * SDRAM Control Register Bit Definitions.
 *
 * @{
 */
BIT_FIELD(SDRAM_SDCR_NC,         0, 0x00000003);
BIT_FIELD(SDRAM_SDCR_NR,         2, 0x0000000C);
BIT_FIELD(SDRAM_SDCR_MWID,       4, 0x00000030);
BIT_FIELD(SDRAM_SDCR_NB,         6, 0x00000040);
BIT_FIELD(SDRAM_SDCR_CAS,        7, 0x00000180);
BIT_FIELD(SDRAM_SDCR_WP,         9, 0x00000200);
BIT_FIELD(SDRAM_SDCR_SDCLK,     10, 0x00000C00);
BIT_FIELD(SDRAM_SDCR_RBURST,    12, 0x00001000);
BIT_FIELD(SDRAM_SDCR_RPIPE,     13, 0x00006000);
/**
 * @}
 */

#endif
#endif