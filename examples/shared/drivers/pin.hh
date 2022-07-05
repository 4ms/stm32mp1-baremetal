#pragma once
#include "rcc.hh"
#include "stm32mp1xx.h"
#include "stm32mp1xx_ll_gpio.h"

enum class GPIO : uint32_t {
	A = GPIOA_BASE,
	B = GPIOB_BASE,
	C = GPIOC_BASE,
	D = GPIOD_BASE,
	E = GPIOE_BASE,
	F = GPIOF_BASE,
	G = GPIOG_BASE,
	H = GPIOH_BASE,
	I = GPIOI_BASE,
	J = GPIOJ_BASE,
	K = GPIOK_BASE,
	Z = GPIOZ_BASE,
};

enum class PinAF {
	AFNone = 0,
	AF_1,
	AF_2,
	AF_3,
	AF_4,
	AF_5,
	AF_6,
	AF_7,
	AF_8,
	AF_9,
	AF_10,
	AF_11,
	AF_12,
	AF_13,
	AF_14,
	AF_15,
};

enum class PinMode : uint32_t {
	Input = LL_GPIO_MODE_INPUT,
	Output = LL_GPIO_MODE_OUTPUT,
	Analog = LL_GPIO_MODE_ANALOG,
	Alt = LL_GPIO_MODE_ALTERNATE
};

enum class PinPull : uint32_t {
	Up = LL_GPIO_PULL_UP,
	Down = LL_GPIO_PULL_DOWN,
	None = LL_GPIO_PULL_NO,
};

enum class PinSpeed : uint32_t {
	Low = LL_GPIO_SPEED_FREQ_LOW,
	Medium = LL_GPIO_SPEED_FREQ_MEDIUM,
	High = LL_GPIO_SPEED_FREQ_HIGH,
	VeryHigh = LL_GPIO_SPEED_FREQ_VERY_HIGH
};

enum class PinOType : uint32_t {
	PushPull = LL_GPIO_OUTPUT_PUSHPULL,
	OpenDrain = LL_GPIO_OUTPUT_OPENDRAIN,
};

enum class PinPolarity {
	Normal,
	Inverted,
};

// Pin class
// Can be used as a convenient container for passing pin defintions
// to a peripheral which performs the register initialization later
struct PinConf {
	GPIO gpio;
	uint8_t pin;
	PinAF af;

	void init(PinMode mode,
			  PinPull pull = PinPull::None,
			  PinPolarity polarity = PinPolarity::Normal,
			  PinSpeed speed = PinSpeed::High,
			  PinOType otype = PinOType::PushPull) const
	{
		auto port_ = reinterpret_cast<GPIO_TypeDef *>(gpio);
		auto pin_ = static_cast<uint16_t>(1 << (pin & 0x0F));

		mdrivlib::RCC_Enable::GPIO::enable(port_);
		LL_GPIO_SetPinMode(port_, pin_, static_cast<uint32_t>(mode));
		LL_GPIO_SetPinPull(port_, pin_, static_cast<uint32_t>(pull));
		LL_GPIO_SetPinSpeed(port_, pin_, static_cast<uint32_t>(speed));

		if (mode == PinMode::Alt || mode == PinMode::Output)
			LL_GPIO_SetPinOutputType(port_, pin_, static_cast<uint32_t>(otype));

		if (mode == PinMode::Alt) {
			if (pin >= 8)
				LL_GPIO_SetAFPin_8_15(port_, pin_, static_cast<uint32_t>(af));
			else
				LL_GPIO_SetAFPin_0_7(port_, pin_, static_cast<uint32_t>(af));
		}
	}

	// Same as init(), just args in a different order
	// Makes it a little cleaner when init'ing AF OpenDrain pins such as I2C
	void init(PinMode mode,
			  PinOType otype,
			  PinPull pull = PinPull::None,
			  PinPolarity polarity = PinPolarity::Normal,
			  PinSpeed speed = PinSpeed::High) const
	{
		init(mode, pull, polarity, speed, otype);
	}

	void low() const
	{
		auto port_ = reinterpret_cast<GPIO_TypeDef *>(gpio);
		auto pin_ = static_cast<uint16_t>(1 << (pin & 0x0F));
		LL_GPIO_ResetOutputPin(port_, pin_);
	}

	void high() const
	{
		auto port_ = reinterpret_cast<GPIO_TypeDef *>(gpio);
		auto pin_ = static_cast<uint16_t>(1 << (pin & 0x0F));
		LL_GPIO_SetOutputPin(port_, pin_);
	}

	bool read() const
	{
		auto port_ = reinterpret_cast<GPIO_TypeDef *>(gpio);
		auto pin_ = static_cast<uint16_t>(1 << (pin & 0x0F));
		return LL_GPIO_IsInputPinSet(port_, pin_);
	}
};
