/**
 * @author Devon Andrade
 * @created 11/27/2019
 *
 * Driver to control the PMOD_JSTK Joystick module (controlled using SPI).
 *
 * https://store.digilentinc.com/pmod-jstk-2-axis-joystick-retired/
 */
#pragma once

#include "gpio.h"
#include "spi.h"

#include <stdbool.h>
#include <stdint.h>

/* Accessor macros for the "btns" output of jstk_get_data. */
#define JSTK_BTN_JOYSTICK(x) (x & 1)
#define JSTK_BTN_BTN1(x) ((x >> 1) & 1)
#define JSTK_BTN_BTN2(x) ((x >> 2) & 1)

/* Structure used to represent a single joystick module. */
typedef struct {
	/* SPI module the joystick is connected to. */
	SpiReg *spi;

	/* True to use hardware-managed slave select, false otherwise. */
	bool use_hardware_ss;

	/* If using software-managed slave select, this is the GPIO slave select pin. */
	GpioReg *ss_reg;
	GpioPin ss_pin;
} PmodJstkInst;

void jstk_init(PmodJstkInst *inst, SpiReg *spi, bool use_hardware_ss);
void jstk_set_ss_pin(PmodJstkInst *inst, GpioReg *ss_reg, GpioPin ss_pin);
void jstk_reinit_spi(PmodJstkInst *inst);

void jstk_get_data(PmodJstkInst *inst, uint16_t *x, uint16_t *y, uint8_t *btns);
void jstk_set_leds(PmodJstkInst *inst, bool led1, bool led2);
