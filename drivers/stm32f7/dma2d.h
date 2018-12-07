/**
 * @author Devon Andrade
 * @created 9/23/2018
 *
 * Definitions and functions used to manipulate the DMA2D Controller [9].
 */
#ifdef INCLUDE_DMA2D_DRIVER
#ifndef DMA2D_H
#define DMA2D_H

#include "status.h"

#include <stdbool.h>
#include <stdint.h>

status_t init_dma2d(void);

status_t dma2d_mem_to_mem(uint32_t src_addr,
                          uint32_t dst_addr,
                          uint16_t width,
                          uint16_t height);

bool is_dma2d_complete(void);

#endif
#endif