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
#include "mem_map.h"

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
	volatile uint32_t CDSR;      /*!< LTDC Current Display Status Register,                Address offset: 0x48 */
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

typedef enum {
	LAYER1 = 0,
	LAYER2 = 1
} LtdcLayer;

/**
 * Useful macro for getting the register map for a particular LTDC layer.
 *
 * “0” for the first layer, “1” for the second layer registers.
 *
 * Use the LtdcLayer enum definitions to keep the code clear.
 */
#define LTDC_LAYER_REG(x) ((LtdcLayerReg *)(LTDC_BASE + 0x84U + (x * 0x80U)))

/**
 * LTDC Synchronization Size Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_SSCR_VSH,    0, 0x000007FF);
BIT_FIELD(LTDC_SSCR_HSW,   16, 0x0FFF0000);

/**
 * LTDC Back Porch Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_BPCR_AVBP,   0, 0x000007FF);
BIT_FIELD(LTDC_BPCR_AHBP,  16, 0x0FFF0000);

/**
 * LTDC Active Width Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_AWCR_AAH,    0, 0x000007FF);
BIT_FIELD(LTDC_AWCR_AAW,   16, 0x0FFF0000);

/**
 * LTDC Total Width Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_TWCR_TOTALH,     0, 0x000007FF);
BIT_FIELD(LTDC_TWCR_TOTALW,    16, 0x0FFF0000);

/**
 * LTDC Global Control Register Register Bit Definitions.
 */
BIT_FIELD(LTDC_GCR_LTDCEN,  0, 0x00000001);
BIT_FIELD(LTDC_GCR_DBW,     4, 0x00000070);
BIT_FIELD(LTDC_GCR_DGW,     8, 0x00000700);
BIT_FIELD(LTDC_GCR_DRW,    12, 0x00007000);
BIT_FIELD(LTDC_GCR_DEN,    16, 0x00010000);
BIT_FIELD(LTDC_GCR_PCPOL,  28, 0x10000000);
BIT_FIELD(LTDC_GCR_DEPOL,  29, 0x20000000);
BIT_FIELD(LTDC_GCR_VSPOL,  30, 0x40000000);
BIT_FIELD(LTDC_GCR_HSPOL,  31, 0x80000000);

/**
 * LTDC Shadow Reload Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_SRCR_IMR,    0, 0x00000001);
BIT_FIELD(LTDC_SRCR_VBR,    1, 0x00000002);

/**
 * LTDC Background Color Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_BCCR_BCBLUE,     0, 0x000000FF);
BIT_FIELD(LTDC_BCCR_BCGREEN,    8, 0x0000FF00);
BIT_FIELD(LTDC_BCCR_BCRED,     16, 0x00FF0000);

/**
 * LTDC Interrupt Enable Register Bit Definitions.
 */
BIT_FIELD(LTDC_IER_LIE,     0, 0x00000001);
BIT_FIELD(LTDC_IER_FUIE,    1, 0x00000002);
BIT_FIELD(LTDC_IER_TERRIE,  2, 0x00000004);
BIT_FIELD(LTDC_IER_RRIE,    3, 0x00000008);

/**
 * LTDC Interrupt Status Register Bit Definitions.
 */
BIT_FIELD(LTDC_ISR_LIF,     0, 0x00000001);
BIT_FIELD(LTDC_ISR_FUIF,    1, 0x00000002);
BIT_FIELD(LTDC_ISR_TERRIF,  2, 0x00000004);
BIT_FIELD(LTDC_ISR_RRIF,    3, 0x00000008);

/**
 * LTDC Interrupt Clear Register Bit Definitions.
 */
BIT_FIELD(LTDC_ICR_CLIF,    0, 0x00000001);
BIT_FIELD(LTDC_ICR_CFUIF,   1, 0x00000002);
BIT_FIELD(LTDC_ICR_CTERRIF, 2, 0x00000004);
BIT_FIELD(LTDC_ICR_CRRIF,   3, 0x00000008);

/**
 * LTDC Line Interrupt Position Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_LIPCR_LIPOS, 0, 0x000007FF);

/**
 * LTDC Current Position Status Register Bit Definitions.
 */
BIT_FIELD(LTDC_CPSR_CYPOS,  0, 0x0000FFFF);
BIT_FIELD(LTDC_CPSR_CXPOS,  0, 0xFFFF0000);

/**
 * LTDC Current Display Status Register Bit Definitions.
 */
BIT_FIELD(LTDC_CDSR_VDES,   0, 0x00000001);
BIT_FIELD(LTDC_CDSR_HDES,   1, 0x00000002);
BIT_FIELD(LTDC_CDSR_VSYNCS, 2, 0x00000004);
BIT_FIELD(LTDC_CDSR_HSYNCS, 3, 0x00000008);

/**** LAYER REGISTERS ****/

/**
 * LTDC Layer Control Register Bit Definitions.
 */
BIT_FIELD(LTDC_LCR_LEN,     0, 0x00000001);
BIT_FIELD(LTDC_LCR_COLKEN,  1, 0x00000002);
BIT_FIELD(LTDC_LCR_CLUTEN,  4, 0x00000010);

/**
 * LTDC Layer Window Horizontal Position Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_LWHPCR_WHSTPOS,  0, 0x00000FFF);
BIT_FIELD(LTDC_LWHPCR_WHSPPOS, 16, 0x0FFF0000);

/**
 * LTDC Layer Window Vertical Position Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_LWVPCR_WVSTPOS,  0, 0x000007FF);
BIT_FIELD(LTDC_LWVPCR_WVSPPOS, 16, 0x07FF0000);

/**
 * LTDC Layer Color Keying Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_LCKCR_CKBLUE,   0, 0x000000FF);
BIT_FIELD(LTDC_LCKCR_CKGREEN,  8, 0x0000FF00);
BIT_FIELD(LTDC_LCKCR_CKRED,   16, 0x00FF0000);

/**
 * LTDC Layer Pixel Format Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_LPFCR_PF, 0, 0x00000007);

/**
 * Definitions for the different supported pixel formats.
 */
typedef enum {
	PF_ARGB8888 = 0x0,
	PF_RGB888 = 0x1,
	PF_RGB565 = 0x2,
	PF_ARGB1555 = 0x3,
	PF_ARGB4444 = 0x4,
	PF_L8 = 0x5,
	PF_AL44 = 0x6,
	PF_AL88 = 0x7
} PFCR_PIXEL_FORMAT;

/**
 * LTDC Layer Constant Alpha Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_LCACR_CONSTA, 0, 0x000000FF);

/**
 * LTDC Layer Default Color Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_LDCCR_DCBLUE,   0, 0x000000FF);
BIT_FIELD(LTDC_LDCCR_DCGREEN,  8, 0x0000FF00);
BIT_FIELD(LTDC_LDCCR_DCRED,   16, 0x00FF0000);
BIT_FIELD(LTDC_LDCCR_DCALPHA, 24, 0xFF000000);

/**
 * LTDC Layer Blending Factors Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_LBFCR_BF2, 0, 0x00000007);
BIT_FIELD(LTDC_LBFCR_BF1, 8, 0x00000700);

/**
 * LTDC Layer Color Frame Buffer Address Register Bit Definitions.
 */
BIT_FIELD(LTDC_LCFBAR_CFBADD, 0, 0xFFFFFFFF);

/**
 * LTDC Layer Color Frame Buffer Length Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_LCFBLR_CFBLL,  0, 0x00001FFF);
BIT_FIELD(LTDC_LCFBLR_CFBP,  16, 0x1FFF0000);

/**
 * LTDC Layer Color Frame Buffer Line number Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_LCFBLNR_CFBLNBR, 0, 0x000007FF);

/**
 * LTDC Layer CLUT Write Configuration Register Bit Definitions.
 */
BIT_FIELD(LTDC_CLUTWR_BLUE,      0, 0x000000FF);
BIT_FIELD(LTDC_CLUTWR_GREEN,     8, 0x0000FF00);
BIT_FIELD(LTDC_CLUTWR_RED,      16, 0x00FF0000);
BIT_FIELD(LTDC_CLUTWR_CLUTADDR, 24, 0xFF000000);

#endif
#endif
