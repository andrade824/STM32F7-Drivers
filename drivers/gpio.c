/**
 * @author Devon Andrade
 * @created 12/28/2016
 *
 * Definitions and functions used to manipulate the GPIO module [6].
 */
#include "registers/gpio_reg.h"
#include "gpio.h"

#include "bitfield.h"
#include "debug.h"
#include "registers/rcc_reg.h"
#include "status.h"

#include <stdbool.h>

/*
 * Check if a pin is already in use, and enable it's port clock if not.
 *
 * @param pin The pin to check.
 *
 * @return Fail if the pin was already requested, Success if configured
 *         correctly.
 */
static inline status_t gpio_setup_pin(GpioPin pin)
{
#ifdef DEBUG_ON
    /**
     * Keeps track of which pins have already been requested.
     */
    static bool requested_gpios[NUM_GPIO_PINS];

    /**
     * Fail if a pin was already requested.
     */
    ABORT_IF(requested_gpios[pin] == true);
    requested_gpios[pin] = true;
#endif

    /**
     * Enable the GPIO port's clock if it isn't already enabled. This utilizes
     * the property that the GPIO port enable bits are contiguous starting from
     * bit 0 of RCC->AHB1ENR for PORTA and going up to bit 10 for PORTK.
     */
    SET_FIELD(RCC->AHB1ENR, (1 << GPIO_GET_PORT(pin)));
    __asm("dsb");

    return Success;
}

/**
 * Request a GPIO pin to be set as an input.
 *
 * @param reg The port register to use.
 * @param pin The pin to set as an input.
 * @param pull The pull state for this pin.
 *
 * @return Fail if the pin was already requested, Success if the pin was
 *         configured correctly.
 */
status_t gpio_request_input(GpioReg *reg, GpioPin pin, GpioPull pull)
{
    ABORT_IF_NOT(gpio_setup_pin(pin));

    /**
     * Set GPIO mode to input (MODE = 0x0).
     */
    reg->MODER &= ~(0x3 << (GPIO_GET_PIN(pin) * 2));

    gpio_set_pullstate(reg, pin, pull);

    return Success;
}

/**
 * Request a GPIO pin to be set as an output.
 *
 * @param reg The port register to use.
 * @param pin The pin to set as an output.
 * @param default_state Default state of the pin (high or low).
 *
 * @return Fail if the pin was already requested, Success if the pin was
 *         configured correctly.
 */
status_t gpio_request_output(GpioReg *reg, GpioPin pin,
                             DigitalState default_state)
{
    ABORT_IF_NOT(gpio_setup_pin(pin));

    /**
     * Set GPIO mode to output (MODE = 0x1).
     */
    reg->MODER |= (GPIO_OUTPUT << (GPIO_GET_PIN(pin) * 2));

    gpio_set_otype(reg, pin, GPIO_PUSH_PULL);
    gpio_set_ospeed(reg, pin, GPIO_OSPEED_4MHZ);
    gpio_set_pullstate(reg, pin, GPIO_NO_PULL);
    gpio_set_output(reg, pin, default_state);

    return Success;
}

/**
 * Request a GPIO pin to be set as an alternate function.
 *
 * @param reg The port register to use.
 * @param pin The pin to set as an alternate function.
 *
 * @return Fail if the pin was already requested, Success if the pin was
 *         configured correctly.
 */
status_t gpio_request_alt(GpioReg *reg, GpioPin pin)
{
    ABORT_IF_NOT(gpio_setup_pin(pin));

    /**
     * Set GPIO mode to alternate function (MODE = 0x2).
     */
    reg->MODER |= (GPIO_ALT_FUNC << (GPIO_GET_PIN(pin) * 2));

    gpio_set_otype(reg, pin, GPIO_PUSH_PULL);
    gpio_set_ospeed(reg, pin, GPIO_OSPEED_4MHZ);
    gpio_set_pullstate(reg, pin, GPIO_NO_PULL);

    return Success;
}

/**
 * Set the output type (push-pull or open-drain) for a GPIO output.
 *
 * @param reg The port register to use.
 * @param pin The pin whose output type to set.
 * @param type The wanted output type.
 */
void gpio_set_otype(GpioReg *reg, GpioPin pin, GpioOType type)
{
    reg->OTYPER &= ~(1 << GPIO_GET_PIN(pin));
    reg->OTYPER |= type << GPIO_GET_PIN(pin);
}

/**
 * Set the output speed (slew rate) for a GPIO output.
 *
 * @param reg The port register to use.
 * @param pin The pin whose output speed to set.
 * @param type The wanted output speed.
 */
void gpio_set_ospeed(GpioReg *reg, GpioPin pin, GpioOSpeed speed)
{
    reg->OTYPER &= ~(3 << (GPIO_GET_PIN(pin) * 2));
    reg->OTYPER |= speed << (GPIO_GET_PIN(pin) * 2);
}

/**
 * Set the pull state (pull-up, pull-down, no-pull) for a GPIO pin.
 *
 * @param reg The port register to use.
 * @param pin The pin whose pull state to set.
 * @param type The wanted pull state.
 */
void gpio_set_pullstate(GpioReg *reg, GpioPin pin, GpioPull pull)
{
    reg->PUPDR &= ~(3 << (GPIO_GET_PIN(pin) * 2));
    reg->PUPDR |= pull << (GPIO_GET_PIN(pin) * 2);
}

/**
 * Set the digital state for an output pin.
 *
 * @param reg The port register to use.
 * @param pin The pin whose output state should be set.
 * @param state The digital state to set the pin to.
 */
void gpio_set_output(GpioReg *reg, GpioPin pin, DigitalState state)
{
    if(state == high)
        reg->BSRR |= 1 << GPIO_GET_PIN(pin);
    else
        reg->BSRR |= 1 << (GPIO_GET_PIN(pin) + 16);
}

/**
 * Get the input state for a pin.
 *
 * @param reg The port register to use.
 * @param pin The pin whose input state should be polled.
 *
 * @return The digital state of the requested pin.
 */
DigitalState gpio_get_input(GpioReg *reg, GpioPin pin)
{
    return (DigitalState)((reg->IDR >> GPIO_GET_PIN(pin)) & 0x1);
}
