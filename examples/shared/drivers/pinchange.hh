#pragma once
#include "stm32mp1xx.h"

template<uint32_t GPIO_BASE_ADDR, uint32_t PIN_NUM>
class PinChangeISR {
public:
	PinChangeISR()
		: _gpio_num{(GPIO_BASE_ADDR == GPIOA_BASE) ? 0 :
					(GPIO_BASE_ADDR == GPIOB_BASE) ? 1 :
					(GPIO_BASE_ADDR == GPIOC_BASE) ? 2 :
					(GPIO_BASE_ADDR == GPIOD_BASE) ? 3 :
					(GPIO_BASE_ADDR == GPIOE_BASE) ? 4 :
					(GPIO_BASE_ADDR == GPIOF_BASE) ? 5 :
					(GPIO_BASE_ADDR == GPIOG_BASE) ? 6 :
					(GPIO_BASE_ADDR == GPIOH_BASE) ? 7 :
					(GPIO_BASE_ADDR == GPIOI_BASE) ? 8 :
					(GPIO_BASE_ADDR == GPIOJ_BASE) ? 9 :
					(GPIO_BASE_ADDR == GPIOK_BASE) ? 10 :
					(GPIO_BASE_ADDR == GPIOZ_BASE) ? 11 :
													   0}
	{
		static_assert(PIN_NUM < 16, "PIN_NUM must be between 0 and 15");
	}

	void enable_isr_on_rising_falling_edges(bool rising, bool falling)
	{
		if (_gpio_num > 12 || PIN_NUM > 15)
			return;

		uint32_t tmp = EXTI->EXTICR[PIN_NUM / 4];
		uint32_t shift = (PIN_NUM % 4) * 8;
		uint32_t mask = 0xFFUL << shift;
		tmp &= ~mask;
		tmp |= (uint32_t)_gpio_num << shift;
		EXTI->EXTICR[PIN_NUM / 4] = tmp;

		if (rising)
			EXTI->RTSR1 = EXTI->RTSR1 | (1 << PIN_NUM);
		else
			EXTI->RTSR1 = EXTI->RTSR1 & ~(1 << PIN_NUM);

		if (falling)
			EXTI->FTSR1 = EXTI->FTSR1 | (1 << PIN_NUM);
		else
			EXTI->FTSR1 = EXTI->FTSR1 & ~(1 << PIN_NUM);
	}

	void clear_rising_isr_flag()
	{
		EXTI->RPR1 = (1 << PIN_NUM); // clear on write
	}

	void clear_falling_isr_flag()
	{
		EXTI->FPR1 = (1 << PIN_NUM); // clear on write
	}

	void enable()
	{
		EXTI->C1IMR1 = EXTI->C1IMR1 | (1 << PIN_NUM);
	}

	void disable()
	{
		EXTI->C1IMR1 = EXTI->C1IMR1 & ~(1 << PIN_NUM);
	}

	static auto get_IRQ_num()
	{
		if constexpr (PIN_NUM == 0)
			return EXTI0_IRQn;
		if constexpr (PIN_NUM == 1)
			return EXTI1_IRQn;
		if constexpr (PIN_NUM == 2)
			return EXTI2_IRQn;
		if constexpr (PIN_NUM == 3)
			return EXTI3_IRQn;
		if constexpr (PIN_NUM == 4)
			return EXTI4_IRQn;
		if constexpr (PIN_NUM == 5)
			return EXTI5_IRQn;
		if constexpr (PIN_NUM == 6)
			return EXTI6_IRQn;
		if constexpr (PIN_NUM == 7)
			return EXTI7_IRQn;
		if constexpr (PIN_NUM == 8)
			return EXTI8_IRQn;
		if constexpr (PIN_NUM == 9)
			return EXTI9_IRQn;
		if constexpr (PIN_NUM == 10)
			return EXTI10_IRQn;
		if constexpr (PIN_NUM == 11)
			return EXTI11_IRQn;
		if constexpr (PIN_NUM == 12)
			return EXTI12_IRQn;
		if constexpr (PIN_NUM == 13)
			return EXTI13_IRQn;
		if constexpr (PIN_NUM == 14)
			return EXTI14_IRQn;
		return EXTI15_IRQn;
	}

private:
	const uint8_t _gpio_num;
};
