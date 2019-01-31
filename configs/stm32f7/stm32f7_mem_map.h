/**
 * @author Devon Andrade
 * @created 12/6/2018
 *
 * Configuration settings common to all STM32F7 based devices.
 */
#ifndef STM32F7_MEM_MAP_H
#define STM32F7_MEM_MAP_H

/**
 * Memory Map definitions.
 */
#define SCS_BASE        (0xE000E000UL)         /*!< System Control Space Base Address */
#define ITM_BASE        (0xE0000000UL)         /*!< ITM Base Address */
#define DWT_BASE        (0xE0001000UL)         /*!< DWT Base Address */
#define TPI_BASE        (0xE0040000UL)         /*!< TPI Base Address */
#define CoreDebug_BASE  (0xE000EDF0UL)         /*!< Core Debug Base Address */
#define SysTick_BASE    (SCS_BASE +  0x0010UL) /*!< SysTick Base Address */
#define NVIC_BASE       (SCS_BASE +  0x0100UL) /*!< NVIC Base Address */
#define SCB_BASE        (SCS_BASE +  0x0D00UL) /*!< System Control Block Base Address */

#define RAMITCM_BASE    0x00000000U /*!< Base address of : 16KB RAM reserved for CPU execution/instruction accessible over ITCM  */
#define FLASHITCM_BASE  0x00200000U /*!< Base address of : (up to 1 MB) embedded FLASH memory  accessible over ITCM              */
#define FLASHAXI_BASE   0x08000000U /*!< Base address of : (up to 1 MB) embedded FLASH memory accessible over AXI                */
#define RAMDTCM_BASE    0x20000000U /*!< Base address of : System data RAM accessible over DTCM                                  */
#define PERIPH_BASE     0x40000000U /*!< Base address of : AHB/ABP Peripherals                                                   */
#define BKPSRAM_BASE    0x40024000U /*!< Base address of : Backup SRAM(4 KB)                                                     */
#define QSPI_BASE       0x90000000U /*!< Base address of : QSPI memories  accessible over AXI                                    */
#define FMC_R_BASE      0xA0000000U /*!< Base address of : FMC Control registers                                                 */
#define QSPI_R_BASE     0xA0001000U /*!< Base address of : QSPI Control  registers                                               */
#define SRAM1_BASE      0x20010000U /*!< Base address of : RAM1 accessible over AXI/AHB                                          */

#define FLASH_BASE      FLASHAXI_BASE

#define APB1PERIPH_BASE PERIPH_BASE
#define APB2PERIPH_BASE (PERIPH_BASE + 0x00010000U)
#define AHB1PERIPH_BASE (PERIPH_BASE + 0x00020000U)
#define AHB2PERIPH_BASE (PERIPH_BASE + 0x10000000U)

#endif /* STM32F7_MEM_MAP_H */
