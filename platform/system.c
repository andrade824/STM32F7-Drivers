/**
 * @author Devon Andrade
 * @created 12/22/2016
 *
 * Contains initialization functions used to get the system into a usable
 * state after startup.
 */
#include "debug.h"
#include "interrupt.h"
#include "platform.h"
#include "registers/flash_reg.h"
#include "registers/pwr_reg.h"
#include "registers/rcc_reg.h"
#include "status.h"

/**
 * Enable the caches using the CMSIS-provided methods.
 *
 * @return success or fail.
 */
static status_t init_caches(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    return success;
}

/**
 * Enable prefetch unit and flash accelerator (ART) with the correct number
 * of wait states.
 *
 * You MUST call this function before setting up the clocks. This ensures that
 * the wait states are properly initialized before boosting the clock up.
 *
 * @return success or fail.
 */
static status_t init_flash(void)
{
    SET_FIELD(FLASH->ACR, FLASH_ACR_ARTRST());
    SET_FIELD(FLASH->ACR, SET_FLASH_ACR_LATENCY(FLASH_WAIT_STATES) |
                          FLASH_ACR_PRFTEN() |
                          FLASH_ACR_ARTEN());

    ABORT_IF_NOT(GET_FLASH_ACR_LATENCY(FLASH->ACR) == FLASH_WAIT_STATES);

    return success;
}

/**
 * Initialize the clock generator to utilize the external clock and PLL.
 *
 * @return success or fail.
 */
static status_t init_clocks(void)
{
    /**
     * Disable all RCC interrupts.
     */
    RCC->CIR = 0x0;

    /**
     * Configure and enable the high-speed external (HSE) clock.
     */
    SET_FIELD(RCC->CR, RCC_CR_HSEON());
    while(GET_RCC_CR_HSERDY(RCC->CR) == 0);

    /**
     * Configure and enable the main PLL.
     *
     * fPLL = (fInput * (PLLN / PLLM)) / PLLP
     *
     * fInput is 25MHz on the STM32F7 discovery board.
     */
    SET_FIELD(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC() |
                            SET_RCC_PLLCFGR_PLLM(25) |
                            SET_RCC_PLLCFGR_PLLN(432) |
                            SET_RCC_PLLCFGR_PLLP(0));

    SET_FIELD(RCC->CR, RCC_CR_PLLON());

    /**
     * Enable overdrive mode on the voltage regulator to allow the chip to
     * reach its highest frequencies. To access the power controller registers
     * you first need to enable the peripheral clock to the power controller.
     */
    SET_FIELD(RCC->APB1ENR, RCC_APB1ENR_PWREN());

    SET_FIELD(PWR->CR1, PWR_CR1_ODEN());
    while(GET_PWR_CSR1_ODRDY(PWR->CR1) == 0) { }

    SET_FIELD(PWR->CR1, PWR_CR1_ODSWEN());
    while(GET_PWR_CSR1_ODSWRDY(PWR->CSR1) == 0) { }

    /**
     * Configure the bus clocks.
     */
    SET_FIELD(RCC->CFGR, SET_RCC_CFGR_PPRE1(0x5) |
                         SET_RCC_CFGR_PPRE2(0x4));

    /**
     * Wait for the PLL to lock.
     */
    while(GET_RCC_CR_PLLRDY(RCC->CR) == 0);

    /**
     * Switch the system clock to use the main PLL.
     */
    SET_FIELD(RCC->CFGR, SET_RCC_CFGR_SW(0x2));

    /**
     * Disable the high-speed internal (HSI) clock.
     */
    CLEAR_FIELD(RCC->CR, RCC_CR_HSION());

    /**
     * Ensure the system clock was switched to the main PLL successfully.
     */
    ABORT_IF_NOT(GET_RCC_CFGR_SWS(RCC->CFGR) == 0x2);

    return success;
}

/**
  * Setup the clocks, caches, memory protection unit (MPU), and other low-level
  * systems.
  *
  * @return success or fail.
  */
status_t init_system(void)
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

    ABORT_IF_NOT(init_caches());
    ABORT_IF_NOT(init_flash());
    ABORT_IF_NOT(init_clocks());
    ABORT_IF_NOT(init_interrupts());

    return success;
}
