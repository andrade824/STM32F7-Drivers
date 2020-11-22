#include "config.h"
#include "debug.h"
#include "fat.h"
#include "fmc_sdram.h"
#include "gpio.h"
#include "graphics.h"
#include "interrupt.h"
#include "sdmmc.h"
#include "spi.h"
#include "spi/nokia5110.h"
#include "spi/pmod_jstk.h"
#include "spi/rfm69_radio.h"
#include "system.h"
#include "usart.h"

#include "registers/fmc_sdram_reg.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if ENABLE_SDRAM
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
#endif /* ENABLE_SDRAM */

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

#if ENABLE_SDRAM && ENABLE_LCD_GRAPHICS
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
#endif /* ENABLE_LCD_GRAPHICS */

/**
 * Dump out the joystick x/y-axis values over the console. BTN1 controls LED1
 * and BTN2 controls LED2. Press the Joystick button to turn both LEDs on.
 */
void jstk_test(void)
{
#ifdef config_stm32f7_dev_board
	/* SPI2 pins on the STM32F7 dev board. */
	#define GPIO_ARD_D13 GPIO_PB13 /* SCK */
	#define GPIO_ARD_D12 GPIO_PB14 /* MISO */
	#define GPIO_ARD_D11 GPIO_PB15 /* MOSI */
	#define GPIO_ARD_D5  GPIO_PB12 /* NSS */
#endif

	gpio_request_alt(GPIO_ARD_D13, AF5, GPIO_OSPEED_25MHZ); /* SPI2 SCK */
	gpio_request_alt(GPIO_ARD_D12, AF5, GPIO_OSPEED_25MHZ); /* SPI2 MISO */
	gpio_request_alt(GPIO_ARD_D11, AF5, GPIO_OSPEED_25MHZ); /* SPI2 MOSI */
	gpio_request_alt(GPIO_ARD_D5, AF5, GPIO_OSPEED_25MHZ);  /* SPI2 NSS */

	PmodJstkInst jstk;
	jstk_init(&jstk, SPI2);
	jstk_set_leds(&jstk, false, true);
	while(1) {
		uint16_t x, y;
		uint8_t btns;
		jstk_get_data(&jstk, &x, &y, &btns);

		bool led1 = JSTK_BTN_BTN1(btns) | JSTK_BTN_JOYSTICK(btns);
		bool led2 = JSTK_BTN_BTN2(btns) | JSTK_BTN_JOYSTICK(btns);
		jstk_set_leds(&jstk, led1, led2);

		dbprintf("X: %04d | Y: %04d | BTN1: %d | BTN2: %d | JSTK_BTN: %d\n",
		         x, y, JSTK_BTN_BTN1(btns), JSTK_BTN_BTN2(btns), JSTK_BTN_JOYSTICK(btns));
	}
}

/**
 * Draw columns of pixels on a Nokia 5110 display using a joystick module as
 * input. Press the joystick button in to clear the screen out.
 *
 * Both devices are expected to be connected to SPI2 but with different slave
 * select lines.
 */
void nokia_jstk_test(void)
{
#ifdef config_stm32f7_dev_board
	/* SPI2 pins on the STM32F7 dev board. */
	#define GPIO_ARD_D13 GPIO_PB13 /* SCK */
	#define GPIO_ARD_D12 GPIO_PB14 /* MISO */
	#define GPIO_ARD_D11 GPIO_PB15 /* MOSI */
	#define GPIO_ARD_D5  GPIO_PB12 /* NOKIA NSS */

	#define GPIO_ARD_D0 GPIO_PB9  /* NOKIA CD */
	#define GPIO_ARD_D1 GPIO_PB10 /* NOKIA RST */
	#define GPIO_ARD_D2 GPIO_PB11 /* JOYSTICK NSS */
#endif

	gpio_request_alt(GPIO_ARD_D13, AF5, GPIO_OSPEED_25MHZ); /* SPI2 SCK */
	gpio_request_alt(GPIO_ARD_D12, AF5, GPIO_OSPEED_25MHZ); /* SPI2 MISO */
	gpio_request_alt(GPIO_ARD_D11, AF5, GPIO_OSPEED_25MHZ); /* SPI2 MOSI */
	gpio_request_output(GPIO_ARD_D5, GPIO_HIGH); /* NOKIA NSS */
	gpio_request_output(GPIO_ARD_D2, GPIO_HIGH); /* JOYSTICK NSS */

	#define NOKIA_DC  GPIO_ARD_D0
	#define NOKIA_RST GPIO_ARD_D1

	gpio_request_output(NOKIA_DC, GPIO_LOW); /* Command/Data */
	gpio_request_output(NOKIA_RST, GPIO_HIGH); /* RST */

	Nokia5110Inst nokia;
	nokia_init(&nokia, SPI2, NOKIA_DC, NOKIA_RST);
	spi_use_software_ss(nokia_get_spi_inst(&nokia), GPIO_ARD_D5);
	nokia_set_params(&nokia, 0xB1, 0, 0x14);
	nokia_set_disp_mode(&nokia, NOKIA_DISP_INVERSE);
	nokia_clear_screen(&nokia);
	nokia_set_position(&nokia, 0, 0);
	nokia_set_column(&nokia, 0xFF);

	PmodJstkInst jstk;
	jstk_init(&jstk, SPI2);
	spi_use_software_ss(jstk_get_spi_inst(&jstk), GPIO_ARD_D2);
	jstk_set_leds(&jstk, false, false);

	uint8_t column = 0;
	uint8_t bank = 0;
	while(1) {
		uint8_t new_column = column;
		uint8_t new_bank = bank;

		uint16_t x, y;
		uint8_t btns;
		spi_reinit(jstk_get_spi_inst(&jstk));
		jstk_get_data(&jstk, &x, &y, &btns);

		bool led1 = JSTK_BTN_BTN1(btns) | JSTK_BTN_JOYSTICK(btns);
		bool led2 = JSTK_BTN_BTN2(btns) | JSTK_BTN_JOYSTICK(btns);
		jstk_set_leds(&jstk, led1, led2);

		if((x < 200) && (column > 0)) {
			new_column--;
		} else if((x > 600) && (column < (NOKIA_WIDTH_PIXELS - 1))) {
			new_column++;
		}

		if((y < 200) && (bank < (NOKIA_HEIGHT_BANKS - 1))) {
			new_bank++;
		} else if((y > 600) && (bank > 0)) {
			new_bank--;
		}

		if(JSTK_BTN_JOYSTICK(btns)) {
			spi_reinit(nokia_get_spi_inst(&nokia));
			nokia_clear_screen(&nokia);
		}

		if((new_column != column) || (new_bank != bank)) {
			spi_reinit(nokia_get_spi_inst(&nokia));
			// nokia_set_position(&nokia, column, bank);
			// nokia_set_column(&nokia, 0);

			nokia_set_position(&nokia, new_column, new_bank);
			nokia_set_column(&nokia, 0xFF);

			const uint64_t timer = (new_bank != bank) ? MSECS(150) : MSECS(50);
			sleep(timer);

			bank = new_bank;
			column = new_column;
		}
	}
}

/**
 * Send a single byte of data from one device to another using RFM69 radio
 * modules. The receiver will send an acknowledgement after every packet it
 * receives, and the transmitter will resend the packet if it doesn't get the
 * ACK.
 *
 * Compile the transmitter: make gdb_[jlink/openocd] CPPFLAGS=-DTRANSMITTER
 * Compile the receiver without the "TRANSMITTER" define.
 */
void rfm69_test(void)
{
	gpio_request_alt(GPIO_PB13, AF5, GPIO_OSPEED_25MHZ); /* SPI2 SCK */
	gpio_request_alt(GPIO_PB14, AF5, GPIO_OSPEED_25MHZ); /* SPI2 MISO */
	gpio_request_alt(GPIO_PB15, AF5, GPIO_OSPEED_25MHZ); /* SPI2 MOSI */
	gpio_request_output(GPIO_PB12, GPIO_HIGH);           /* SPI2 NSS */
	gpio_request_output(GPIO_PB11, GPIO_HIGH);           /* RFM69 RST */

	Rfm69Inst radio;
	rfm69_init_radio(&radio, SPI2, GPIO_PB11, GPIO_PB12);
	rfm69_set_payload_length(&radio, RFM69_VARIABLE_LENGTH_PAYLOAD);
	rfm69_set_power_mode(&radio, RFM69_PA1, 31); /* 13dBm */

	#define DATA_SIZE 5

#ifdef TRANSMITTER
	dbprintf("Transmitter\n");
	gpio_request_output(GPIO_LED_USER, GPIO_LOW);
	DigitalState led_ctrl = GPIO_LOW;

	uint8_t data[DATA_SIZE] = { 0, 0, 2, 3, 4 };
	while(1) {
		sleep(MSECS(100));

		if(!rfm69_send_with_ack(&radio, data, DATA_SIZE, 2, MSECS(30))) {
			dbprintf("Failed to send packet %d\n", data[0]);
		}

		data[0]++;

		if(data[0] == 0) {
			/* Keep track of how many times data[0] overflows. */
			data[1]++;
		}

		if(led_ctrl == GPIO_LOW) {
			led_ctrl = GPIO_HIGH;
		} else {
			led_ctrl = GPIO_LOW;
		}

		gpio_set_output(GPIO_LED_USER, led_ctrl);
	}
#else
	dbprintf("Receiver\n");
	uint8_t num_read = 0;
	uint8_t data[DATA_SIZE] = { 0 };
	while(1) {
		uint8_t start_data = data[0];
		num_read = rfm69_receive_with_ack(&radio, data, DATA_SIZE);
		ASSERT(num_read == DATA_SIZE);

		/* Make it easy to spot dropped packets. */
		if(data[0] != (start_data + 1)) {
			dbprintf("---");
		}

		dbprintf("Iter: %d | Data: %d %d %d %d | RSSI: %d dBm\n",
			data[1], data[0], data[2], data[3], data[4], rfm69_get_last_rssi(&radio));
	}
#endif
}
