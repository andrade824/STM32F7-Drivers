#include "config.h"
#include "debug.h"
#include "fat.h"
#include "fmc_sdram.h"
#include "gpio.h"
#include "graphics.h"
#include "interrupt.h"
#include "sdmmc.h"
#include "system.h"
#include "usart.h"

#include "registers/fmc_sdram_reg.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef INCLUDE_SDRAM_DRIVER
/**
 * Perform a basic memory check on the external SDRAM.
 */
void fmc_memcheck_test(void)
{
	fmc_sdram_init();

	dbprintf("Beginning Memcheck...\n");

	for (uint32_t uwIndex = 0; uwIndex < 0x00800000; uwIndex += 4)
	{
		*(volatile uint32_t*) (SDRAM_BASE + uwIndex) = uwIndex;
	}

	for (uint32_t uwIndex = 0; uwIndex < 0x00800000; uwIndex += 4)
	{
		uint32_t value = *(volatile uint32_t*) (SDRAM_BASE + uwIndex);

		if(value != uwIndex)
		{
			dbprintf("Memcheck failure: value 0x%lx != index 0x%lx\n", value, uwIndex);
			gpio_set_output(GPIO_ARD_D13, GPIO_HIGH);
			while(1) { }
		}

		if(uwIndex % 0x10000 == 0) {
			dbprintf("Checkpoint... value 0x%lx -- index 0x%lx\n", value, uwIndex);
		}

	}

	dbprintf("Memcheck complete!!\n");
}
#endif /* INCLUDE_SDRAM_DRIVER */

#ifdef INCLUDE_SDMMC_DRIVER
/**
 * Write multiple blocks of data, then read the blocks back and make sure the
 * data matches.
 */
void sd_read_write_test(void)
{
	/* Initialize the SDMMC module */
	gpio_request_alt(GPIO_USD_D0, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D0, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D1, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D1, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D2, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D2, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D3, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D3, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_CLK, AF12, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_USD_CMD, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_CMD, GPIO_PULL_UP);
	ABORT_IF_NOT(sdmmc_init());
	dbprintf("SDMMC appears to have initialized!\n");

	uint8_t write_data[2048];
	for(unsigned int i = 0; i < 8; i++) {
		for(unsigned int j = 0; j < 256; j++) {
			write_data[(i * 256) + j] = (uint8_t)j;
		}
	}

	dbprintf("------Writing Data at %lu...-------\n", sd_get_card_info().total_blocks - 5);
	SdStatus status = sd_write_data(write_data, sd_get_card_info().total_blocks - 5, 4);
	if(status != SD_SUCCESS) {
		ABORT("Here's the SD status for write %d\n", status);
	}

	uint8_t read_data[2048];
	status = sd_read_data(read_data, sd_get_card_info().total_blocks - 5, 4);
	if(status != SD_SUCCESS) {
		ABORT("Here's the SD status for read %d\n", status);
	}

	dbprintf("------Checking Data at %lu...-------\n", sd_get_card_info().total_blocks - 5);
	for(unsigned int i = 0; i < 8; i++) {
		for(unsigned int j = 0; j < 256; j++) {
			const size_t index = (i * 256) + j;
			if(read_data[index] != write_data[index]) {
				ABORT("DATA MISMATCH AT INDEX %d, %d != %d\n", index, read_data[index], write_data[index]);
			}
		}
	}

	dbprintf("Data verified correctly!\n");
}

/**
 * Read MBR partition and the first FAT32 Partition.
 */
void sd_read_mbr_test(void)
{
	/* Initialize the SDMMC module */
	gpio_request_alt(GPIO_USD_D0, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D0, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D1, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D1, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D2, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D2, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D3, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D3, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_CLK, AF12, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_USD_CMD, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_CMD, GPIO_PULL_UP);
	ABORT_IF_NOT(sdmmc_init());
	dbprintf("SDMMC appears to have initialized!\n");

	uint8_t data[512];
	SdStatus status = sd_read_data(data, 0, 1);
	if(status != SD_SUCCESS) {
		ABORT("Here's the SD status %d\n", status);
	}
	printf("------MBR Partition:-------\n");
	for(int i = 0; i < 512; ++i) {
		dbprintf("0x%x ", data[i]);
	}
	dbprintf("\n");

	uint32_t fat32_lba = (data[457] << 24U) | (data[456] << 16U) | (data[455] << 8U) | data[454];
	printf("Logical block address: 0x%lx\n", fat32_lba);
	status = sd_read_data(data, fat32_lba, 1);
	if(status != SD_SUCCESS) {
		ABORT("Here's the SD status %d\n", status);
	}
	printf("------FAT32 Partition:-------\n");
	for(int i = 0; i < 512; ++i) {
		dbprintf("0x%x ", data[i]);
	}
	dbprintf("\n");
}
#endif /* INCLUDE_SDMMC_DRIVER */

#if defined(INCLUDE_SDMMC_DRIVER) && defined(INCLUDE_FAT_DRIVER)

/**
 * Dump out the contents of the file located at "path" to the screen.
 */
void fat_dump_file_test(char *path)
{
	/* Initialize the SDMMC module */
	gpio_request_alt(GPIO_USD_D0, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D0, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D1, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D1, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D2, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D2, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_D3, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_D3, GPIO_PULL_UP);
	gpio_request_alt(GPIO_USD_CLK, AF12, GPIO_OSPEED_50MHZ);
	gpio_request_alt(GPIO_USD_CMD, AF12, GPIO_OSPEED_50MHZ);
	gpio_set_pullstate(GPIO_USD_CMD, GPIO_PULL_UP);
	ABORT_IF_NOT(sdmmc_init());
	dbprintf("SDMMC appears to have initialized!\n");

	FatOperations ops = {
		.total_size = sd_get_card_info().total_size,
		.total_sectors = sd_get_card_info().total_blocks,
		&sd_read_data,
		&sd_write_data
	};
	ABORT_IF_NOT(fat_init(ops));

	FatFile file;
	fat_open(&file, path, FAT_READ_MODE);
	dbprintf("----Opened file %s %lu:----\n", path, file.size);
	#define BUFSIZE 1024
	char temp[BUFSIZE];
	memset((void*)temp, 0, BUFSIZE);
	uint32_t bytes_read = 0;
	while((bytes_read = fat_read(&file, (void*)&temp, BUFSIZE)) > 0) {
		for(int i = 0; (i < BUFSIZE) && (temp[i] != '\0'); ++i) {
			dbprintf("%c", temp[i]);
		}
		memset((void*)temp, 0, BUFSIZE);
	}
	dbprintf("Done reading...\n");
}

#endif /* defined(INCLUDE_SDMMC_DRIVER) && defined(INCLUDE_FAT_DRIVER) */

#if defined(INCLUDE_GRAPHICS_MODULE) && defined(INCLUDE_USART_DRIVER) && defined(INCLUDE_SDRAM_DRIVER)
/**
 * Print characters received over USART onto the screen.
 */
void usart_gfx_test(void)
{
	const uint32_t draw_buffer = SDRAM_BASE;
	const uint32_t render_buffer = SDRAM_BASE + (LCD_CONFIG_WIDTH * LCD_CONFIG_HEIGHT * LCD_CONFIG_PIXEL_SIZE);

	fmc_sdram_init();
	gfx_init(render_buffer, draw_buffer);

	gfx_clear_screen(PIXEL(0,0,0));
	gfx_text_set_cursor(20, 10);
	gfx_text_foreground(PIXEL(0, 255, 0));
	gfx_text_background(PIXEL(0, 0, 0));

	gfx_swap_buffers();

	/* Initialize the USART module */
	gpio_request_alt(GPIO_PC6, AF8, GPIO_OSPEED_4MHZ);
	gpio_request_alt(GPIO_PC7, AF8, GPIO_OSPEED_4MHZ);
	usart_init(USART6, 115200, USART_8_DATA, USART_1_STOP);
	usart_enable_rx(USART6, true);
	usart_enable_tx(USART6, true);
	usart_send(USART6, (uint8_t*)"Hello There!\r\n", 14);
	uint8_t ascii = 0;

	while(1) {
		ascii = usart_receive(USART6);
		//dbprintf("%d\n", ascii);
		usart_send_byte(USART6, ascii);
		if(ascii >= 32 && ascii < 127) {
			gfx_draw_char(ascii);

			/* Wait for the DMA transfer to complete. */
			gfx_swap_buffers();
		}
	}
}
#endif /* defined(INCLUDE_GRAPHICS_MODULE) && defined(INCLUDE_USART_DRIVER) && defined(INCLUDE_SDRAM_DRIVER) */

#if defined(INCLUDE_GRAPHICS_MODULE) && defined(INCLUDE_SDRAM_DRIVER)
/**
 * Draw multiple rectangles on the screen at once.
 */
void gfx_drawing_test(void)
{
	const uint32_t draw_buffer = SDRAM_BASE;
	const uint32_t render_buffer = SDRAM_BASE + (LCD_CONFIG_WIDTH * LCD_CONFIG_HEIGHT * LCD_CONFIG_PIXEL_SIZE);

	fmc_sdram_init();
	gfx_init(render_buffer, draw_buffer);

	gfx_clear_screen(PIXEL(0,0,0));
	gfx_swap_buffers();

	srand(37); // Number chosen randomly through multiple dice throws.

	uint16_t x = 0;
	const uint16_t y = 100;
	const uint16_t rect_width = 50;
	const uint16_t rect_height = 50;

	while(1) {
		gfx_clear_screen(PIXEL(255, 255, 255));

		/* Draw a random rectangle. */
		uint8_t red = rand() % 256;
		uint8_t blue = rand() % 256;
		uint8_t green = rand() % 256;

		uint16_t x0 = rand() % (gfx_width() / 2);
		uint16_t y0 = rand() % (gfx_height() / 2);
		uint16_t x1 = x0 + (rand() % (gfx_width() / 2));
		uint16_t y1 = y0 + (rand() % (gfx_height() / 2));

		gfx_draw_rect(x0, y0, x1, y1, PIXEL(red, green, blue));

		gfx_draw_rect(0, 0, 50, 50, PIXEL(0, 0, 255));
		gfx_draw_rect(100, 170, 150, 220, PIXEL(0, 255, 0));
		gfx_draw_rect(x, y, x + rect_width, y + rect_height, PIXEL(255, 0, 0));

		x = ((x + rect_width + 1) >= gfx_width()) ? 0 : x + 1;

		/**
		 * With CLEAR_SCREEN: 31.8ms per frame (~31fps)
		 * Without CLEAR_SCREEN: 16.6ms per frame (~60fps)
		 */

		/* Wait for the DMA transfer to complete. */
		gfx_swap_buffers();
	}
}

/**
 * Repeatedly print "Merry Christmas!" to the screen.
 */
void gfx_text_test(void)
{
	const uint32_t draw_buffer = SDRAM_BASE;
	const uint32_t render_buffer = SDRAM_BASE + (LCD_CONFIG_WIDTH * LCD_CONFIG_HEIGHT * LCD_CONFIG_PIXEL_SIZE);

	fmc_sdram_init();
	gfx_init(render_buffer, draw_buffer);

	gfx_clear_screen(PIXEL(0,0,0));
	gfx_text_set_cursor(20, 10);
	gfx_text_foreground(PIXEL(0, 255, 0));
	gfx_text_background(PIXEL(0, 0, 0));

	gfx_swap_buffers();

	while(1) {
		gfx_text_foreground(PIXEL(255, 0, 0));
		gfx_draw_text("Merry ");
		gfx_text_foreground(PIXEL(0, 255, 0));
		gfx_draw_text("Christmas! ");

		/* Wait for the DMA transfer to complete. */
		gfx_swap_buffers();
	}
}
#endif /* defined(INCLUDE_GRAPHICS_MODULE) && defined(INCLUDE_SDRAM_DRIVER) */
