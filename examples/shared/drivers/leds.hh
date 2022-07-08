#pragma once
#include "pinconf.hh"
#include "stm32mp1xx.h"
#include "stm32mp1xx_ll_gpio.h"

enum class LedActive { Low, High };

// Led is a generic class that controls a GPIO output to, e.g., drive an LED.
// See drivers/pin.hh for enum definitions for the template parameters.
// GPIOx: the GPIO to use, such as GPIO::B
// PINMASK: the pin to use, such as PinNum::_2
// POLARITY: either LedActive::Low or LedActive::High (default).
//           It selects whether the LED turns on when the pin is low or high.

template<GPIO GPIOx, PinNum PINMASK, LedActive POLARITY = LedActive::High>
class Led {
public:
	constexpr static uint32_t gpio = static_cast<uint32_t>(GPIOx);
	constexpr static uint16_t pin_mask = static_cast<uint16_t>(PINMASK);
	constexpr static uint32_t pin_num = PinConf::bit_to_num(PINMASK);

	Led()
	{
		PinConf{GPIOx, PINMASK, PinAF::AFNone}.init(PinMode::Output);
	}

	void on()
	{
		if constexpr (POLARITY == LedActive::High)
			PinConf{GPIOx, PINMASK}.high();
		else
			PinConf{GPIOx, PINMASK}.low();
	}

	void off()
	{
		if constexpr (POLARITY == LedActive::High)
			PinConf{GPIOx, PINMASK}.low();
		else
			PinConf{GPIOx, PINMASK}.high();
	}
};
