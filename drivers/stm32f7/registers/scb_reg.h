/**
 * @author Devon Andrade
 * @created 12/23/2018
 *
 * Definitions and functions used to manipulate the System Control Block (SCB).
 */
#ifndef SCB_REG_H
#define SCB_REG_H

#include "bitfield.h"
#include "mem_map.h"

#include <stdint.h>

/**
 * These definitions are provided by CMSIS, so no need to redefine them.
 *
 * If I eventually get rid of CMSIS, then enable this.
 */
#if 0
/**
 * Type defining the System Control Block module register map.
 */
typedef struct
{
	volatile uint32_t CPUID;         /*!< Offset: 0x000 (R/ )  CPUID Base Register */
	volatile uint32_t ICSR;          /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
	volatile uint32_t VTOR;          /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
	volatile uint32_t AIRCR;         /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
	volatile uint32_t SCR;           /*!< Offset: 0x010 (R/W)  System Control Register */
	volatile uint32_t CCR;           /*!< Offset: 0x014 (R/W)  Configuration Control Register */
	volatile uint8_t  SHPR[12U];     /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
	volatile uint32_t SHCSR;         /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
	volatile uint32_t CFSR;          /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register */
	volatile uint32_t HFSR;          /*!< Offset: 0x02C (R/W)  HardFault Status Register */
	volatile uint32_t DFSR;          /*!< Offset: 0x030 (R/W)  Debug Fault Status Register */
	volatile uint32_t MMFAR;         /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register */
	volatile uint32_t BFAR;          /*!< Offset: 0x038 (R/W)  BusFault Address Register */
	volatile uint32_t AFSR;          /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register */
	volatile uint32_t ID_PFR[2U];    /*!< Offset: 0x040 (R/ )  Processor Feature Register */
	volatile uint32_t ID_DFR;        /*!< Offset: 0x048 (R/ )  Debug Feature Register */
	volatile uint32_t ID_AFR;        /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register */
	volatile uint32_t ID_MFR[4U];    /*!< Offset: 0x050 (R/ )  Memory Model Feature Register */
	volatile uint32_t ID_ISAR[5U];   /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register */
	uint32_t          RESERVED0[1U];
	volatile uint32_t CLIDR;         /*!< Offset: 0x078 (R/ )  Cache Level ID register */
	volatile uint32_t CTR;           /*!< Offset: 0x07C (R/ )  Cache Type register */
	volatile uint32_t CCSIDR;        /*!< Offset: 0x080 (R/ )  Cache Size ID Register */
	volatile uint32_t CSSELR;        /*!< Offset: 0x084 (R/W)  Cache Size Selection Register */
	volatile uint32_t CPACR;         /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register */
	uint32_t          RESERVED3[93U];
	volatile uint32_t STIR;          /*!< Offset: 0x200 ( /W)  Software Triggered Interrupt Register */
	uint32_t          RESERVED4[15U];
	volatile uint32_t MVFR0;         /*!< Offset: 0x240 (R/ )  Media and VFP Feature Register 0 */
	volatile uint32_t MVFR1;         /*!< Offset: 0x244 (R/ )  Media and VFP Feature Register 1 */
	volatile uint32_t MVFR2;         /*!< Offset: 0x248 (R/ )  Media and VFP Feature Register 1 */
	uint32_t          RESERVED5[1U];
	volatile uint32_t ICIALLU;       /*!< Offset: 0x250 ( /W)  I-Cache Invalidate All to PoU */
	uint32_t          RESERVED6[1U];
	volatile uint32_t ICIMVAU;       /*!< Offset: 0x258 ( /W)  I-Cache Invalidate by MVA to PoU */
	volatile uint32_t DCIMVAC;       /*!< Offset: 0x25C ( /W)  D-Cache Invalidate by MVA to PoC */
	volatile uint32_t DCISW;         /*!< Offset: 0x260 ( /W)  D-Cache Invalidate by Set-way */
	volatile uint32_t DCCMVAU;       /*!< Offset: 0x264 ( /W)  D-Cache Clean by MVA to PoU */
	volatile uint32_t DCCMVAC;       /*!< Offset: 0x268 ( /W)  D-Cache Clean by MVA to PoC */
	volatile uint32_t DCCSW;         /*!< Offset: 0x26C ( /W)  D-Cache Clean by Set-way */
	volatile uint32_t DCCIMVAC;      /*!< Offset: 0x270 ( /W)  D-Cache Clean and Invalidate by MVA to PoC */
	volatile uint32_t DCCISW;        /*!< Offset: 0x274 ( /W)  D-Cache Clean and Invalidate by Set-way */
	uint32_t          RESERVED7[6U];
	volatile uint32_t ITCMCR;        /*!< Offset: 0x290 (R/W)  Instruction Tightly-Coupled Memory Control Register */
	volatile uint32_t DTCMCR;        /*!< Offset: 0x294 (R/W)  Data Tightly-Coupled Memory Control Registers */
	volatile uint32_t AHBPCR;        /*!< Offset: 0x298 (R/W)  AHBP Control Register */
	volatile uint32_t CACR;          /*!< Offset: 0x29C (R/W)  L1 Cache Control Register */
	volatile uint32_t AHBSCR;        /*!< Offset: 0x2A0 (R/W)  AHB Slave Control Register */
	uint32_t          RESERVED8[1U];
	volatile uint32_t ABFSR;         /*!< Offset: 0x2A8 (R/W)  Auxiliary Bus Fault Status Register */
} ScbReg;

/**
 * Define the SCB register map accessor.
 */
#define SCB ((ScbReg *) SCB_BASE)
#endif

/**
 * System Handler Control and State Register.
 */
BIT_FIELD(SCB_SHCSR_MEMFAULTACT,     0, 0x00000001);
BIT_FIELD(SCB_SHCSR_BUSFAULTACT,     1, 0x00000002);
BIT_FIELD(SCB_SHCSR_USGFAULTACT,     3, 0x00000008);
BIT_FIELD(SCB_SHCSR_SVCALLACT,       7, 0x00000080);
BIT_FIELD(SCB_SHCSR_MONITORACT,      8, 0x00000100);
BIT_FIELD(SCB_SHCSR_PENDSVACT,      10, 0x00000400);
BIT_FIELD(SCB_SHCSR_SYSTICKACT,     11, 0x00000800);
BIT_FIELD(SCB_SHCSR_USGFAULTPENDED, 12, 0x00001000);
BIT_FIELD(SCB_SHCSR_MEMFAULTPENDED, 13, 0x00002000);
BIT_FIELD(SCB_SHCSR_BUSFAULTPENDED, 14, 0x00004000);
BIT_FIELD(SCB_SHCSR_SVCALLPENDED,   15, 0x00008000);
BIT_FIELD(SCB_SHCSR_MEMFAULTENA,    16, 0x00010000);
BIT_FIELD(SCB_SHCSR_BUSFAULTENA,    17, 0x00020000);
BIT_FIELD(SCB_SHCSR_USGFAULTENA,    18, 0x00040000);

/**
 * Configuration and Control Register.
 */
BIT_FIELD(SCB_CCR_NONBASETHRDENA, 0, 0x00000001);
BIT_FIELD(SCB_CCR_USERSETMPEND,   1, 0x00000002);
BIT_FIELD(SCB_CCR_UNALIGN_TRP,    3, 0x00000008);
BIT_FIELD(SCB_CCR_DIV_0_TRP,      4, 0x00000010);
BIT_FIELD(SCB_CCR_BFHFNMIGN,      8, 0x00000100);
BIT_FIELD(SCB_CCR_STKALIGN,       9, 0x00000200);
BIT_FIELD(SCB_CCR_DC,            16, 0x00010000);
BIT_FIELD(SCB_CCR_IC,            17, 0x00020000);
BIT_FIELD(SCB_CCR_BP,            18, 0x00040000);

/**
 * Configurable Fault Status Register.
 */
BIT_FIELD(SCB_CFSR_IACCVIOL,     0, 0x00000001);
BIT_FIELD(SCB_CFSR_DACCVIOL,     1, 0x00000002);
BIT_FIELD(SCB_CFSR_MUNSTKERR,    3, 0x00000004);
BIT_FIELD(SCB_CFSR_MSTKERR,      4, 0x00000010);
BIT_FIELD(SCB_CFSR_MLSPERR,      5, 0x00000020);
BIT_FIELD(SCB_CFSR_MMARVALID,    7, 0x00000080);
BIT_FIELD(SCB_CFSR_IBUSERR,      8, 0x00000100);
BIT_FIELD(SCB_CFSR_PRECISERR,    9, 0x00000200);
BIT_FIELD(SCB_CFSR_IMPRECISERR, 10, 0x00000400);
BIT_FIELD(SCB_CFSR_UNSTKERR,    11, 0x00000800);
BIT_FIELD(SCB_CFSR_STKERR,      12, 0x00001000);
BIT_FIELD(SCB_CFSR_LSPERR,      13, 0x00002000);
BIT_FIELD(SCB_CFSR_BFARVALID,   15, 0x00008000);
BIT_FIELD(SCB_CFSR_UNDEFINSTR,  16, 0x00010000);
BIT_FIELD(SCB_CFSR_INVSTATE,    17, 0x00020000);
BIT_FIELD(SCB_CFSR_INVPC,       18, 0x00040000);
BIT_FIELD(SCB_CFSR_NOCP,        19, 0x00080000);
BIT_FIELD(SCB_CFSR_UNALIGNED,   24, 0x01000000);
BIT_FIELD(SCB_CFSR_DIVBYZERO,   25, 0x02000000);

/**
 * Hard Fault Status Register.
 */
BIT_FIELD(SCB_HFSR_VECTTBL,   1, 0x00000002);
BIT_FIELD(SCB_HFSR_FORCED,   30, 0x40000000);
BIT_FIELD(SCB_HFSR_DEBUGEVT, 31, 0x80000000);

#endif
