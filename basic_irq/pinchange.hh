#pragma once
#include "stm32mp157cxx_ca7.h"

class PinChangeISR {
public:
	// gpio_num is: 0 => GPIOA, 1 => GPIOB, 2 => GPIOC, ... 10 => GPIOK, 11 => GPIOZ
	// pin_num is 0 - 15
	PinChangeISR(uint8_t gpio_num, uint8_t pin_num)
		: _pin(pin_num)
		, _gpio_num(gpio_num)
	{}

	void enable_isr_on_rising_falling_edges(bool rising, bool falling)
	{
		if (_gpio_num > 12 || _pin > 15)
			return;

		uint32_t tmp = EXTI->EXTICR[_pin / 4];
		uint32_t shift = (_pin % 4) * 8;
		uint32_t mask = 0xFFUL << shift;
		tmp &= ~mask;
		tmp |= (uint32_t)_gpio_num << shift;
		EXTI->EXTICR[_pin / 4] = tmp;

		if (rising)
			EXTI->RTSR1 = EXTI->RTSR1 | (1 << _pin);
		else
			EXTI->RTSR1 = EXTI->RTSR1 & ~(1 << _pin);

		if (falling)
			EXTI->FTSR1 = EXTI->FTSR1 | (1 << _pin);
		else
			EXTI->FTSR1 = EXTI->FTSR1 & ~(1 << _pin);
	}

	void clear_rising_isr_flag()
	{
		EXTI->RPR1 = (1 << _pin); // clear on write
	}

	void clear_falling_isr_flag()
	{
		EXTI->FPR1 = (1 << _pin); // clear on write
	}

	void enable()
	{
		EXTI->C1IMR1 = EXTI->C1IMR1 | (1 << _pin);
	}

	void disable()
	{
		EXTI->C1IMR1 = EXTI->C1IMR1 & ~(1 << _pin);
	}

	auto get_IRQ_num()
	{
		if (_pin == 0)
			return EXTI0_IRQn;
		if (_pin == 1)
			return EXTI1_IRQn;
		if (_pin == 2)
			return EXTI2_IRQn;
		if (_pin == 3)
			return EXTI3_IRQn;
		if (_pin == 4)
			return EXTI4_IRQn;
		if (_pin == 5)
			return EXTI5_IRQn;
		if (_pin == 6)
			return EXTI6_IRQn;
		if (_pin == 7)
			return EXTI7_IRQn;
		if (_pin == 8)
			return EXTI8_IRQn;
		if (_pin == 9)
			return EXTI9_IRQn;
		if (_pin == 10)
			return EXTI10_IRQn;
		if (_pin == 11)
			return EXTI11_IRQn;
		if (_pin == 12)
			return EXTI12_IRQn;
		if (_pin == 13)
			return EXTI13_IRQn;
		if (_pin == 14)
			return EXTI14_IRQn;

		return EXTI15_IRQn;
	}

private:
	const uint8_t _pin;
	const uint8_t _gpio_num;
};
