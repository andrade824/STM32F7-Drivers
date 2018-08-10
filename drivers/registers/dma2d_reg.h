/**
 * @author Devon Andrade
 * @created 8/9/2018
 *
 * Chrom-ART Accelerator (Graphics DMA) Controller Register Map [18.7].
 */
#ifdef INCLUDE_DMA2D_DRIVER
#ifndef DAM2D_REG_H
#define DMA2D_REG_H

#include "bitfield.h"
#include "platform.h"

#include <stdint.h>

/**
 * Type defining the DMA2D Controller register map.
 */
typedef struct
{
    volatile uint32_t CR;            /*!< DMA2D Control Register,                         Address offset: 0x00 */
    volatile uint32_t ISR;           /*!< DMA2D Interrupt Status Register,                Address offset: 0x04 */
    volatile uint32_t IFCR;          /*!< DMA2D Interrupt Flag Clear Register,            Address offset: 0x08 */
    volatile uint32_t FGMAR;         /*!< DMA2D Foreground Memory Address Register,       Address offset: 0x0C */
    volatile uint32_t FGOR;          /*!< DMA2D Foreground Offset Register,               Address offset: 0x10 */
    volatile uint32_t BGMAR;         /*!< DMA2D Background Memory Address Register,       Address offset: 0x14 */
    volatile uint32_t BGOR;          /*!< DMA2D Background Offset Register,               Address offset: 0x18 */
    volatile uint32_t FGPFCCR;       /*!< DMA2D Foreground PFC Control Register,          Address offset: 0x1C */
    volatile uint32_t FGCOLR;        /*!< DMA2D Foreground Color Register,                Address offset: 0x20 */
    volatile uint32_t BGPFCCR;       /*!< DMA2D Background PFC Control Register,          Address offset: 0x24 */
    volatile uint32_t BGCOLR;        /*!< DMA2D Background Color Register,                Address offset: 0x28 */
    volatile uint32_t FGCMAR;        /*!< DMA2D Foreground CLUT Memory Address Register,  Address offset: 0x2C */
    volatile uint32_t BGCMAR;        /*!< DMA2D Background CLUT Memory Address Register,  Address offset: 0x30 */
    volatile uint32_t OPFCCR;        /*!< DMA2D Output PFC Control Register,              Address offset: 0x34 */
    volatile uint32_t OCOLR;         /*!< DMA2D Output Color Register,                    Address offset: 0x38 */
    volatile uint32_t OMAR;          /*!< DMA2D Output Memory Address Register,           Address offset: 0x3C */
    volatile uint32_t OOR;           /*!< DMA2D Output Offset Register,                   Address offset: 0x40 */
    volatile uint32_t NLR;           /*!< DMA2D Number of Line Register,                  Address offset: 0x44 */
    volatile uint32_t LWR;           /*!< DMA2D Line Watermark Register,                  Address offset: 0x48 */
    volatile uint32_t AMTCR;         /*!< DMA2D AHB Master Timer Configuration Register,  Address offset: 0x4C */
    uint32_t          RESERVED[236]; /*!< Reserved, 0x50-0x3FF */
    volatile uint32_t FGCLUT[256];   /*!< DMA2D Foreground CLUT,                          Address offset:400-7FF */
    volatile uint32_t BGCLUT[256];   /*!< DMA2D Background CLUT,                          Address offset:800-BFF */
} Dma2dReg;


/**
 * Define the DMA2D Controller register map accessor.
 */
#define DMA2D_BASE  (AHB1PERIPH_BASE + 0xB000U)
#define DMA2D       ((Dma2dReg *)DMA2D_BASE)

/**
 * DMA2D Control Register Bit Definitions.
 */
BIT_FIELD(DMA2D_CR_START,   0, 0x00000001);
BIT_FIELD(DMA2D_CR_SUSP,    1, 0x00000002);
BIT_FIELD(DMA2D_CR_ABORT,   2, 0x00000004);
BIT_FIELD(DMA2D_CR_TEIE,    8, 0x00000100);
BIT_FIELD(DMA2D_CR_TCIE,    9, 0x00000200);
BIT_FIELD(DMA2D_CR_TWIE,   10, 0x00000400);
BIT_FIELD(DMA2D_CR_CAEIE,  11, 0x00000800);
BIT_FIELD(DMA2D_CR_CTCIE,  12, 0x00001000);
BIT_FIELD(DMA2D_CR_CEIE,   13, 0x00002000);
BIT_FIELD(DMA2D_CR_MODE,   16, 0x00030000);

/**
 * DMA2D Interrupt Status Register Bit Definitions.
 */
BIT_FIELD(DMA2D_ISR_TEIF,   0, 0x00000001);
BIT_FIELD(DMA2D_ISR_TCIF,   1, 0x00000002);
BIT_FIELD(DMA2D_ISR_TWIF,   2, 0x00000004);
BIT_FIELD(DMA2D_ISR_CAEIF,  3, 0x00000008);
BIT_FIELD(DMA2D_ISR_CTCIF,  4, 0x00000010);
BIT_FIELD(DMA2D_ISR_CEIF,   5, 0x00000020);

/**
 * DMA2D Interrupt Flag Clear Register Bit Definitions.
 */
BIT_FIELD(DMA2D_IFCR_CTEIF,     0, 0x00000001);
BIT_FIELD(DMA2D_IFCR_CTCIF,     1, 0x00000002);
BIT_FIELD(DMA2D_IFCR_CTWIF,     2, 0x00000004);
BIT_FIELD(DMA2D_IFCR_CCAEIF,    3, 0x00000008);
BIT_FIELD(DMA2D_IFCR_CCTCIF,    4, 0x00000010);
BIT_FIELD(DMA2D_IFCR_CCEIF,     5, 0x00000020);

/**
 * DMA2D Foreground Memory Address Register Bit Definitions.
 */
BIT_FIELD(DMA2D_FGMAR_MA, 0, 0xFFFFFFFF);

/**
 * DMA2D Foreground Offset Register Bit Definitions.
 */
BIT_FIELD(DMA2D_FGOR_LO, 0, 0x00003FFF);

/**
 * DMA2D Background Memory Address Register Bit Definitions.
 */
BIT_FIELD(DMA2D_BGMAR_MA, 0, 0xFFFFFFFF);

/**
 * DMA2D Background Offset Register Bit Definitions.
 */
BIT_FIELD(DMA2D_BGOR_LO, 0, 0x00003FFF);

/**
 * DMA2D Foreground PFC Control Register Bit Definitions.
 */
BIT_FIELD(DMA2D_FGPFCCR_CM,      0, 0x0000000F);
BIT_FIELD(DMA2D_FGPFCCR_CCM,     4, 0x00000010);
BIT_FIELD(DMA2D_FGPFCCR_START,   5, 0x00000020);
BIT_FIELD(DMA2D_FGPFCCR_CS,      8, 0x0000FF00);
BIT_FIELD(DMA2D_FGPFCCR_AM,     16, 0x00030000);
BIT_FIELD(DMA2D_FGPFCCR_ALPHA,  24, 0xFF000000);

/**
 * DMA2D Foreground Color Register Bit Definitions.
 */
BIT_FIELD(DMA2D_FGCOLR_BLUE,     0, 0x000000FF);
BIT_FIELD(DMA2D_FGCOLR_GREEN,    8, 0x0000FF00);
BIT_FIELD(DMA2D_FGCOLR_RED,     16, 0x00FF0000);

/**
 * DMA2D Background PFC Control Register Bit Definitions.
 */
BIT_FIELD(DMA2D_BGPFCCR_CM,      0, 0x0000000F);
BIT_FIELD(DMA2D_BGPFCCR_CCM,     4, 0x00000010);
BIT_FIELD(DMA2D_BGPFCCR_START,   5, 0x00000020);
BIT_FIELD(DMA2D_BGPFCCR_CS,      8, 0x0000FF00);
BIT_FIELD(DMA2D_BGPFCCR_AM,     16, 0x00030000);
BIT_FIELD(DMA2D_BGPFCCR_ALPHA,  24, 0xFF000000);

/**
 * DMA2D Background Color Register Bit Definitions.
 */
BIT_FIELD(DMA2D_BGCOLR_BLUE,     0, 0x000000FF);
BIT_FIELD(DMA2D_BGCOLR_GREEN,    8, 0x0000FF00);
BIT_FIELD(DMA2D_BGCOLR_RED,     16, 0x00FF0000);

/**
 * DMA2D Foreground CLUT Memory Address Register Bit Definitions.
 */
BIT_FIELD(DMA2D_FGCMAR_MA, 0, 0xFFFFFFFF);

/**
 * DMA2D Background CLUT Memory Address Register Bit Definitions.
 */
BIT_FIELD(DMA2D_BGCMAR_MA, 0, 0xFFFFFFFF);

/**
 * DMA2D Output PFC Control Register Bit Definitions.
 */
BIT_FIELD(DMA2D_OPFCCR_CM, 0, 0x00000007);

/**
 * DMA2D Output Color Register Bit Definitions.
 */
BIT_FIELD(DMA2D_OCOLR_BLUE,      0, 0x000000FF);
BIT_FIELD(DMA2D_OCOLR_GREEN,     8, 0x0000FF00);
BIT_FIELD(DMA2D_OCOLR_RED,      16, 0x00FF0000);
BIT_FIELD(DMA2D_OCOLR_ALPHA,    24, 0xFF000000);

/**
 * DMA2D Output Memory Address Register Bit Definitions.
 */
BIT_FIELD(DMA2D_OMAR_MA, 0, 0xFFFFFFFF);

/**
 * DMA2D Output Offset Register Bit Definitions.
 */
BIT_FIELD(DMA2D_OOR_LO, 0, 0x00003FFF);

/**
 * DMA2D Number of Line Register Bit Definitions.
 */
BIT_FIELD(DMA2D_NLR_NL,  0, 0x0000FFFF);
BIT_FIELD(DMA2D_NLR_PL, 16, 0x3FFF0000);

/**
 * DMA2D Line Watermark Register Bit Definitions.
 */
BIT_FIELD(DMA2D_LWR_LW,  0, 0x0000FFFF);

/**
 * DMA2D AHB Master Timer Configuration Register Bit Definitions.
 */
BIT_FIELD(DMA2D_AMTCR_EN, 0, 0x00000001);
BIT_FIELD(DMA2D_AMTCR_DT, 8, 0x0000FF00);

#endif
#endif
