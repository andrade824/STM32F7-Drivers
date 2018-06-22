/**
 * @author Devon Andrade
 * @created 6/14/2018
 *
 * SDRAM Controller Register Map [13.7].
 */
#ifdef INCLUDE_SDRAM_DRIVER
#ifndef FMC_SDRAM_REG_H
#define FMC_SDRAM_REG_H

#include "bitfield.h"
#include "platform.h"

#include <stdint.h>

/**
 * Type defining the FMC SDRAM Controller module register map.
 */
typedef struct
{
    volatile uint32_t SDCR[2];  /*!< SDRAM Control registers ,      Address offset: 0x140-0x144  */
    volatile uint32_t SDTR[2];  /*!< SDRAM Timing registers ,       Address offset: 0x148-0x14C  */
    volatile uint32_t SDCMR;    /*!< SDRAM Command Mode register,   Address offset: 0x150  */
    volatile uint32_t SDRTR;    /*!< SDRAM Refresh Timer register,  Address offset: 0x154  */
    volatile uint32_t SDSR;     /*!< SDRAM Status register,         Address offset: 0x158  */
} FmcSdramReg;

/**
 * Define the FMC SDRAM Controller register map accessor.
 */
#define SDRAM_REG_BASE (FMC_R_BASE + 0x0140U)
#define SDRAM ((FmcSdramReg *) SDRAM_REG_BASE)

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

/**
 * SDRAM Timing Register Bit Definitions.
 *
 * @{
 */
BIT_FIELD(SDRAM_SDTR_TMRD,   0, 0x0000000F);
BIT_FIELD(SDRAM_SDTR_TXSR,   4, 0x000000F0);
BIT_FIELD(SDRAM_SDTR_TRAS,   8, 0x00000F00);
BIT_FIELD(SDRAM_SDTR_TRC,   12, 0x0000F000);
BIT_FIELD(SDRAM_SDTR_TWR,   16, 0x000F0000);
BIT_FIELD(SDRAM_SDTR_TRP,   20, 0x00F00000);
BIT_FIELD(SDRAM_SDTR_TRCD,  24, 0x0F000000);
/**
 * @}
 */

/**
 * SDRAM Command Mode Register Bit Definitions.
 *
 * @{
 */
BIT_FIELD(SDRAM_SDCMR_MODE, 0, 0x00000007);
BIT_FIELD(SDRAM_SDCMR_CTB2, 3, 0x00000008);
BIT_FIELD(SDRAM_SDCMR_CTB1, 4, 0x00000010);
BIT_FIELD(SDRAM_SDCMR_NRFS, 5, 0x000001E0);
BIT_FIELD(SDRAM_SDCMR_MRD,  9, 0x003FFE00);

/**
 * Definitions for the different modes supported in the SDCMR register.
 */
typedef enum
{
    SDCMR_NORMAL = 0x0,
    SDCMR_CLOCK_CFG = 0x1,
    SDCMR_PALL = 0x2,
    SDCMR_AUTO_REFRESH = 0x3,
    SDCMR_LOAD_MODE_REG = 0x4,
    SDCMR_SELF_REFRESH = 0x5,
    SDCMR_POWER_DOWN = 0x6
} SDCMR_MODE;
/**
 * @}
 */

/**
 * SDRAM Refresh Timer Register Bit Definitions.
 *
 * @{
 */
BIT_FIELD(SDRAM_SDRTR_CRE,      0, 0x00000001);
BIT_FIELD(SDRAM_SDRTR_COUNT,    2, 0x00003FFE);
BIT_FIELD(SDRAM_SDRTR_REIE,     14, 0x00004000);
/**
 * @}
 */

/**
 * SDRAM Status Register Bit Definitions.
 *
 * @{
 */
BIT_FIELD(SDRAM_SDSR_RE,        0, 0x00000001);
BIT_FIELD(SDRAM_SDSR_MODES1,    1, 0x00000006);
BIT_FIELD(SDRAM_SDSR_MODES2,    3, 0x00000018);
BIT_FIELD(SDRAM_SDSR_BUSY,      5, 0x00000020);

/**
 * Definitions for the BUSY bit.
 */
typedef enum
{
    SDSR_READY = 0x0,
    SDSR_BUSY = 0x1
} SDSR_BUSY_BIT;
/**
 * @}
 */

#endif
#endif