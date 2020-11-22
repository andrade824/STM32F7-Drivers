/**
 * @author Devon Andrade
 * @created 12/26/2019
 *
 * Driver to control an RFM69 radio module.
 *
 * https://cdn-shop.adafruit.com/product-files/3076/RFM69HCW-V1.1.pdf
 */
#pragma once

#include "gpio.h"
#include "spi.h"
#include "spi/rfm69_radio_reg.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * To simplify operation, restrict the maximum payload length to the internal
 * FIFO size (66 bytes) minus some bytes for overhead (like payload length and CRC).
 */
#define RFM69_MAX_PAYLOAD_LEN 61U

/* Sentinel used to represent a variable length payload in Rfm69Inst->payload_length. */
#define RFM69_VARIABLE_LENGTH_PAYLOAD 0U

/**
 * Possible power modes.
 *
 * PA0 is only available on the lower power radio modules (RFM69W).
 */
typedef enum {
	RFM69_PA0,          /* -18dBm to +13dBm on RFM69W */
	RFM69_PA1,          /* -2dBm to +13dBm on RFM69HW/RFM69HCW */
	RFM69_PA1_PA2,      /* +2dBm to +17dBm on RFM69HW/RFM69HCW */
	RFM69_PA1_PA2_BOOST /* +5dBm to +20dBm on RFM69HW/RFM69HCW */
} Rfm69PowerMode;

/* Structure used to represent a single RFM69 radio. */
typedef struct {
	/* SPI module the radio is connected to. */
	SpiInst spi;

	/* RSSI (received power) at time of last packet reception. */
	int16_t last_rssi;

	/* Current radio mode (standby, transmit, receive, etc). */
	Rfm69Mode mode;

	/* 0 for variable length, >0 for fixed length payloads. */
	uint8_t payload_length;

	/* Which power mode the radio is operating in. */
	Rfm69PowerMode power_mode;
} Rfm69Inst;

void rfm69_init_radio(
	Rfm69Inst *inst,
	SpiReg *spi,
	GpioReg *rst_reg,
	GpioPin rst_pin,
	GpioReg *nss_reg,
	GpioPin nss_pin);

SpiInst* rfm69_get_spi_inst(Rfm69Inst *inst);

void rfm69_set_payload_length(Rfm69Inst *inst, uint8_t length);
void rfm69_set_power_mode(Rfm69Inst *inst, Rfm69PowerMode mode, uint8_t level);

void rfm69_send(Rfm69Inst *inst, uint8_t *data, uint8_t length);
uint8_t rfm69_receive(Rfm69Inst *inst, uint8_t *data, uint8_t length);

bool rfm69_send_with_ack(
	Rfm69Inst *inst,
	uint8_t *data,
	uint8_t length,
	uint8_t max_retries,
	uint32_t timeout);
uint8_t rfm69_receive_with_ack(Rfm69Inst *inst, uint8_t *buffer, uint8_t buffer_len);

int16_t rfm69_get_last_rssi(Rfm69Inst *inst);
void rfm69_dump_regs(Rfm69Inst *inst, bool compact);
