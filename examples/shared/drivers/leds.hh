#pragma once
#include "drivers/pin.hh"
#include "stm32mp1xx.h"
#include "stm32mp1xx_ll_gpio.h"

enum class LedActive { Low, High };

// Led is a generic class that controls a GPIO output.
// See drivers/pin.hh for enums. Template parameters are:
// GPIOx: the GPIO to use, such as GPIO::B
// PINx: the pin to use, such as PinNum::_2
// POLARITY: either LedActive::Low or LedActive::High (default).
//           It selects whether the LED turns on when the pin is low or high.

template<GPIO GPIOx, uint32_t PINx, LedActive POLARITY = LedActive::High>
class Led {
	GPIO_TypeDef *const gpio = reinterpret_cast<GPIO_TypeDef *>(GPIOx);
	constexpr static uint16_t pin_bit = static_cast<uint16_t>(PINx);

public:
	Led()
	{
		PinConf pin{static_cast<GPIO>(GPIOx), static_cast<PinNum>(1 << PINx), PinAF::AFNone};
		pin.init(PinMode::Output);
	}

	void on()
	{
		if constexpr (POLARITY == LedActive::High)
			LL_GPIO_SetOutputPin(gpio, pin_bit);
		else
			LL_GPIO_ResetOutputPin(gpio, pin_bit);
	}

	void off()
	{
		if constexpr (POLARITY == LedActive::High)
			LL_GPIO_ResetOutputPin(gpio, pin_bit);
		else
			LL_GPIO_SetOutputPin(gpio, pin_bit);
	}
};
