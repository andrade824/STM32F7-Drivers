#ifndef STM32F7_TESTS_H
#define STM32F7_TESTS_H

#ifdef INCLUDE_SDRAM_DRIVER
void fmc_memcheck_test(void);
#endif /* INCLUDE_SDRAM_DRIVER */

#ifdef INCLUDE_SDMMC_DRIVER
void sd_read_write_test(void);
void sd_read_mbr_test(void);
#endif /* INCLUDE_SDMMC_DRIVER */

#if defined(INCLUDE_SDMMC_DRIVER) && defined(INCLUDE_FAT_DRIVER)
void fat_dump_file_test(char *path);
#endif /* defined(INCLUDE_SDMMC_DRIVER) && defined(INCLUDE_FAT_DRIVER) */

#if defined(INCLUDE_GRAPHICS_MODULE) && defined(INCLUDE_USART_DRIVER) && defined(INCLUDE_SDRAM_DRIVER)
void usart_gfx_test(void);
#endif /* defined(INCLUDE_GRAPHICS_MODULE) && defined(INCLUDE_USART_DRIVER) && defined(INCLUDE_SDRAM_DRIVER) */

#if defined(INCLUDE_GRAPHICS_MODULE) && defined(INCLUDE_SDRAM_DRIVER)
void gfx_drawing_test(void);
void gfx_text_test(void);
#endif /* defined(INCLUDE_GRAPHICS_MODULE) && defined(INCLUDE_SDRAM_DRIVER) */

#endif /* STM32F7_TESTS_H */