/**
 * @author Devon Andrade
 * @created 12/22/2016
 *
 * Contains initialization functions used to get the system into a usable
 * state after startup.
 */
#include "config.h"
#include "debug.h"
#include "interrupt.h"
#include "system_timer.h"

#include "registers/flash_reg.h"
#include "registers/pwr_reg.h"
#include "registers/rcc_reg.h"

#ifdef SEMIHOSTING_ENABLED
/**
 * This method is provided by the rdimon library for initializing semihosting
 * operations. This must be called before any printf() method is called.
 */
extern void initialise_monitor_handles(void);
#endif

/**
 * Enable the caches using the CMSIS-provided methods.
 */
static void caches_init(void)
{
	SCB_EnableICache();
	SCB_EnableDCache();
}

/**
 * Enable prefetch unit and flash accelerator (ART) with the correct number
 * of wait states.
 *
 * You MUST call this function before setting up the clocks. This ensures that
 * the wait states are properly initialized before boosting the clock up.
 */
static void flash_init(void)
{
	SET_FIELD(FLASH->ACR, FLASH_ACR_ARTRST());
	SET_FIELD(FLASH->ACR, SET_FLASH_ACR_LATENCY(FLASH_WAIT_STATES) |
	                      FLASH_ACR_PRFTEN() |
	                      FLASH_ACR_ARTEN());

	ABORT_IF_NOT(GET_FLASH_ACR_LATENCY(FLASH->ACR) == FLASH_WAIT_STATES);
}

/**
 * Initialize the clock generator to utilize the external clock and PLL.
 */
static void clocks_init(void)
{
	/* Disable all RCC interrupts. */
	RCC->CIR = 0x0;

	/* Configure and enable the high-speed external (HSE) clock. */
#if HSE_BYPASS
	SET_FIELD(RCC->CR, RCC_CR_HSEBYP());
#endif

	SET_FIELD(RCC->CR, RCC_CR_HSEON());
	while(GET_RCC_CR_HSERDY(RCC->CR) == 0);

	/* Configure the 48MHz clock. */
#if ENABLE_48MHZ_CLOCK
	SET_FIELD(RCC->DCKCFGR2, SET_RCC_DCKCFGR2_CK48MSEL(CLK_CK48MSEL) |
	                         SET_RCC_DCKCFGR2_SDMMC1SEL(CLK_SDMMCSEL));
#endif /* ENABLE_48MHZ_CLOCK */

	/**
	 * Configure and enable the main PLL.
	 *
	 * fPLL = (fInput * (PLLN / PLLM)) / PLLP
	 *
	 * fInput is 25MHz on the STM32F7 discovery board.
	 */
	RCC->PLLCFGR = SET_RCC_PLLCFGR_PLLQ(CLK_PLLQ) |
	               RCC_PLLCFGR_PLLSRC() |
	               SET_RCC_PLLCFGR_PLLM(CLK_PLLM) |
	               SET_RCC_PLLCFGR_PLLN(CLK_PLLN) |
	               SET_RCC_PLLCFGR_PLLP(CLK_PLLP);

	SET_FIELD(RCC->CR, RCC_CR_PLLON());

#if ENABLE_LCD_GRAPHICS
	/* Configure and enable PLLSAI (used to drive the LCD pixel clock). */
	CLEAR_FIELD(RCC->PLLSAICFGR, RCC_PLLSAICFGR_PLLN() |
	                             RCC_PLLSAICFGR_PLLSAIR());
	SET_FIELD(RCC->PLLSAICFGR, SET_RCC_PLLSAICFGR_PLLN(CLK_PLLSAI_PLLN) |
	                           SET_RCC_PLLSAICFGR_PLLSAIR(CLK_PLLSAIR));

	SET_FIELD(RCC->DCKCFGR1, SET_RCC_DCKCFGR1_PLLSAIDIVR(CLK_PLLSAIDIVR));

	SET_FIELD(RCC->CR, RCC_CR_PLLSAION());

	/* Wait for PLLSAI to lock. */
	while(GET_RCC_CR_PLLSAIRDY(RCC->CR) == 0) { }
#endif /* ENABLE_LCD_GRAPHICS */

	/**
	 * Enable overdrive mode on the voltage regulator to allow the chip to
	 * reach its highest frequencies. To access the power controller registers
	 * you first need to enable the peripheral clock to the power controller.
	 */
	SET_FIELD(RCC->APB1ENR, RCC_APB1ENR_PWREN());
	__asm("dsb");

	SET_FIELD(PWR->CR1, PWR_CR1_ODEN());
	while(GET_PWR_CSR1_ODRDY(PWR->CR1) == 0) { }

	SET_FIELD(PWR->CR1, PWR_CR1_ODSWEN());
	while(GET_PWR_CSR1_ODSWRDY(PWR->CSR1) == 0) { }

	/* Configure the bus clocks. */
	SET_FIELD(RCC->CFGR, SET_RCC_CFGR_PPRE1(CLK_APB1_DIV) |
	                     SET_RCC_CFGR_PPRE2(CLK_APB2_DIV));

	/* Wait for the PLL to lock. */
	while(GET_RCC_CR_PLLRDY(RCC->CR) == 0) { }

	/* Switch the system clock to use the main PLL. */
	SET_FIELD(RCC->CFGR, SET_RCC_CFGR_SW(0x2));

	/* Disable the high-speed internal (HSI) clock. */
	CLEAR_FIELD(RCC->CR, RCC_CR_HSION());

	/* Ensure the system clock was switched to the main PLL successfully. */
	ABORT_IF_NOT(GET_RCC_CFGR_SWS(RCC->CFGR) == 0x2);
}

/**
 * Setup the clocks, caches, interrupts, and other low-level systems.
 */
void system_init(void)
{
	/**
	 * Initialize FPU. The weird macro checking is done to ensure that you setup
	 * the toolchain to utilize the FPU correctly.
	 *
	 * Give both privileged and unprivileged code full access to coprocessors
	 * 10 and 11 (the ones dealing with floating point).
	 */
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
#endif

#ifdef SEMIHOSTING_ENABLED
	initialise_monitor_handles();
#endif

	caches_init();
	flash_init();
	clocks_init();
	interrupt_init();
	system_timer_init();
}
