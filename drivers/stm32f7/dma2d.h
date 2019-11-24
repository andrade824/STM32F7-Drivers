/**
 * @author Devon Andrade
 * @created 9/23/2018
 *
 * Definitions and functions used to manipulate the DMA2D Controller [9].
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

void dma2d_init(void);

void dma2d_mem_to_mem(
	uint32_t src_addr,
	uint32_t dst_addr,
	uint16_t width,
	uint16_t height,
	void (*callback) (void));

bool is_dma2d_complete(void);
