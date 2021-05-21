#pragma once
#include "stm32mp157cxx_ca7.h"
#include "stm32mp1xx_ll_gpio.h"

enum class LedActive { Low, High };

template<uint32_t GPIO_BASE_ADDR, uint32_t PIN_NUM, LedActive POLARITY>
class Led {
	GPIO_TypeDef *const gpio;
	const uint16_t pin_bit;

public:
	Led()
		: gpio{reinterpret_cast<GPIO_TypeDef *>(GPIO_BASE_ADDR)}
		, pin_bit{1 << PIN_NUM}
	{}

	void init()
	{
		if constexpr (GPIO_BASE_ADDR == GPIOA_BASE)
			RCC->MP_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIOAEN;
		if constexpr (GPIO_BASE_ADDR == GPIOB_BASE)
			RCC->MP_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIOBEN;
		if constexpr (GPIO_BASE_ADDR == GPIOC_BASE)
			RCC->MP_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIOCEN;
		if constexpr (GPIO_BASE_ADDR == GPIOD_BASE)
			RCC->MP_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIODEN;
		if constexpr (GPIO_BASE_ADDR == GPIOE_BASE)
			RCC->MP_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIOEEN;
		if constexpr (GPIO_BASE_ADDR == GPIOF_BASE)
			RCC->MP_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIOFEN;
		if constexpr (GPIO_BASE_ADDR == GPIOH_BASE)
			RCC->MP_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIOHEN;
		if constexpr (GPIO_BASE_ADDR == GPIOI_BASE)
			RCC->MP_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIOIEN;
		if constexpr (GPIO_BASE_ADDR == GPIOJ_BASE)
			RCC->MP_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIOJEN;
		if constexpr (GPIO_BASE_ADDR == GPIOJ_BASE)
			RCC->MP_AHB4ENSETR = RCC_MC_AHB4ENSETR_GPIOJEN;
		if constexpr (GPIO_BASE_ADDR == GPIOZ_BASE)
			RCC->MP_AHB5ENSETR = RCC_MC_AHB5ENSETR_GPIOZEN;

		LL_GPIO_SetPinMode(gpio, pin_bit, LL_GPIO_MODE_OUTPUT);
		LL_GPIO_SetPinOutputType(gpio, pin_bit, LL_GPIO_OUTPUT_PUSHPULL);
		LL_GPIO_SetPinSpeed(gpio, pin_bit, LL_GPIO_SPEED_FREQ_MEDIUM);
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
