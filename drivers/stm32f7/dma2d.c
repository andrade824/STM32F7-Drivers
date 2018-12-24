/**
 * @author Devon Andrade
 * @created 9/23/2018
 *
 * Definitions and functions used to manipulate the DMA2D Controller [9].
 *
 * Don't call into this module directly, use the dedicated "graphics" module.
 */
#ifdef INCLUDE_DMA2D_DRIVER

#include "debug.h"
#include "dma2d.h"
#include "interrupt.h"
#include "status.h"

#include "registers/dma2d_reg.h"
#include "registers/lcd_ctrl_reg.h"
#include "registers/rcc_reg.h"

#include <stdbool.h>
#include <stdint.h>

/* Flag that gets set to true when the DMA is not performing transfers. */
static volatile bool dma_complete = true;

/* The callback function to call when a DMA transfer finishes. */
static void (*dma_callback) (void) = NULL;

/**
 * DMA2D ISR. Set dma_complete if transfer complete or spin on error.
 */
void dma2d_isr(void)
{
	if(GET_DMA2D_ISR_TCIF(DMA2D->ISR)) {
		SET_FIELD(DMA2D->IFCR, DMA2D_IFCR_CTCIF());
		dma_complete = true;

		if(dma_callback != NULL) {
			dma_callback();
		}
	} else if(GET_DMA2D_ISR_TEIF(DMA2D->ISR)) {
		SET_FIELD(DMA2D->IFCR, DMA2D_IFCR_CTEIF());
		dbprintf("DMA2D Error: Transfer Error\n");
		while(1) { }
	} else if(GET_DMA2D_ISR_CAEIF(DMA2D->ISR)) {
		SET_FIELD(DMA2D->IFCR, DMA2D_IFCR_CCAEIF());
		dbprintf("DMA2D Error: CLUT Access Error\n");
		while(1) { }
	} else if(GET_DMA2D_ISR_CEIF(DMA2D->ISR)) {
		SET_FIELD(DMA2D->IFCR, DMA2D_IFCR_CCEIF());
		dbprintf("DMA2D Error: Configuration Error\n");
		while(1) { }
	}
}

/**
 * Initialize the DMA2D Controller.
 */
status_t init_dma2d(void)
{
	/**
	 * Enable the DMA2D clock.
	 */
	SET_FIELD(RCC->AHB1ENR, RCC_AHB1ENR_DMA2DEN());
	__asm("dsb");

	// Setup interrupt service routine
	SET_FIELD(DMA2D->CR, DMA2D_CR_TCIE() |
						 DMA2D_ISR_TEIF() |
						 DMA2D_ISR_CAEIF() |
						 DMA2D_ISR_CEIF());

	ABORT_IF_NOT(request_interrupt(DMA2D_IRQn, dma2d_isr));

	return Success;
}

/**
 * Peform a memory-to-memory transaction.
 *
 * @param src_addr The address of the first pixel in the source buffer.
 * @param dst_addr The address of the first pixel in the destination buffer.
 * @param width The number of pixels per line to transfer.
 * @param height The number of lines to transfer.
 * @param callback Function to call when transfer is complete or NULL for no
 *                 callback.
 */
status_t dma2d_mem_to_mem(uint32_t src_addr,
						  uint32_t dst_addr,
						  uint16_t width,
						  uint16_t height,
						  void (*callback) (void))
{
	/**
	 * Abort early if the DMA is already running.
	 */
#if 1
	ABORT_IF_NOT(is_dma2d_complete());
#endif

	/**
	 * Set the transfer mode to memory-to-memory.
	 */
	CLEAR_FIELD(DMA2D->CR, DMA2D_CR_MODE());
	SET_FIELD(DMA2D->CR, SET_DMA2D_CR_MODE(DMA2D_MEM_MEM));

	/**
	 * Setup the transaction parameters.
	 */
	DMA2D->FGMAR = src_addr;
	DMA2D->FGOR = 0;
	DMA2D->FGPFCCR = SET_DMA2D_FGPFCCR_CM(LCD_CONFIG_PIXEL_FORMAT) |
	                 SET_DMA2D_FGPFCCR_AM(AM_REPLACE) |
	                 SET_DMA2D_FGPFCCR_ALPHA(0xFF);

	DMA2D->OPFCCR = LCD_CONFIG_PIXEL_FORMAT;
	DMA2D->OMAR = dst_addr;
	DMA2D->OOR = 0;
	DMA2D->NLR = SET_DMA2D_NLR_PL(width) |
				 SET_DMA2D_NLR_NL(height);

	/**
	 * Start the transaction.
	 */
	dma_callback = callback;
	dma_complete = false;
	SET_FIELD(DMA2D->CR, DMA2D_CR_START());

	return Success;
}

/**
 * @return true if the DMA is done transferring data.
 */
bool is_dma2d_complete(void)
{
	return dma_complete;
}

#endif
