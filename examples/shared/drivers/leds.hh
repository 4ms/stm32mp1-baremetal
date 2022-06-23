#pragma once
#include "drivers/pin.hh"
#include "stm32mp1xx.h"
#include "stm32mp1xx_ll_gpio.h"

enum class LedActive { Low, High };

// Led is a generic class that controls a GPIO output.
// Template parameters are:
// GPIO_BASE_ADDR: the base address of the GPIO peripheral. Typically you'll use something like GPIOA_BASE or
// GPIOD_BASE. PIN_NUM: this is the pin number, from 0 to 15 POLARITY: either LedActive::Low or LedActive::High
// (default). It selects whether the LED turns on when the pin is low or high.
template<GPIO GPIO_BASE_ADDR, uint32_t PIN_NUM, LedActive POLARITY = LedActive::High>
class Led {
	GPIO_TypeDef *const gpio;
	const uint16_t pin_bit;

public:
	Led()
		: gpio{reinterpret_cast<GPIO_TypeDef *>(GPIO_BASE_ADDR)}
		, pin_bit{1 << PIN_NUM}
	{
		PinConf pin{GPIO_BASE_ADDR, PIN_NUM, PinAF::AFNone};
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
