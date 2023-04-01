/*
 * pin.hh
 *
 * copyright (c) 2022 Dan Green <danngreen1@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "drivers/register_access.hh"
#include "stm32xx.h"
#include <cstddef>

namespace mdrivlib
{
enum class PinPolarity {
	Normal,
	Inverted,
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

// GPIOx_BASE are defined in the CMSIS header that's included from stm32xx.h
enum class GPIO : uint32_t {
	A = GPIOA_BASE,
	B = GPIOB_BASE,
	C = GPIOC_BASE,
	D = GPIOD_BASE,
#ifdef GPIOE_BASE
	E = GPIOE_BASE,
#endif
#ifdef GPIOF_BASE
	F = GPIOF_BASE,
#endif
#ifdef GPIOG_BASE
	G = GPIOG_BASE,
#endif
#ifdef GPIOH_BASE
	H = GPIOH_BASE,
#endif
#ifdef GPIOI_BASE
	I = GPIOI_BASE,
#endif
#ifdef GPIOJ_BASE
	J = GPIOJ_BASE,
#endif
#ifdef GPIOK_BASE
	K = GPIOK_BASE,
#endif
#ifdef GPIOZ_BASE
	Z = GPIOZ_BASE,
#endif
	Unused = 0,
};

enum PinNum : uint8_t {
	_0 = 0,
	_1 = 1,
	_2 = 2,
	_3 = 3,
	_4 = 4,
	_5 = 5,
	_6 = 6,
	_7 = 7,
	_8 = 8,
	_9 = 9,
	_10 = 10,
	_11 = 11,
	_12 = 12,
	_13 = 13,
	_14 = 14,
	_15 = 15,
};

enum PinAF : uint8_t {
	AFNone = 0,
	AltFunc1,
	AltFunc2,
	AltFunc3,
	AltFunc4,
	AltFunc5,
	AltFunc6,
	AltFunc7,
	AltFunc8,
	AltFunc9,
	AltFunc10,
	AltFunc11,
	AltFunc12,
	AltFunc13,
	AltFunc14,
	AltFunc15,
};

// Container for passing pin defintions to a peripheral which performs the initialization
struct PinDef {
	GPIO gpio{GPIO::Unused};
	PinNum pin;
	PinAF af;
};

// Legacy code support:
using PinNoInit = PinDef;

class Pin {
public:
	Pin() = default;

	// Pin{GPIO::A, 10, PinMode::Input, ...};
	Pin(GPIO port,
		uint8_t pin,
		PinMode mode,
		uint8_t af = 0,
		PinPull pull = PinPull::None,
		PinPolarity polarity = PinPolarity::Normal,
		PinSpeed speed = PinSpeed::High,
		PinOType otype = PinOType::PushPull);

	Pin(const PinDef &other,
		PinMode mode,
		PinPull pull = PinPull::None,
		PinPolarity polarity = PinPolarity::Normal,
		PinSpeed speed = PinSpeed::High,
		PinOType otype = PinOType::PushPull);

	void init(const PinDef &other,
			  PinMode mode,
			  PinPull pull = PinPull::None,
			  PinPolarity polarity = PinPolarity::Normal,
			  PinSpeed speed = PinSpeed::High,
			  PinOType otype = PinOType::PushPull);

	void high() const;
	void low() const;
	void on() const;
	void off() const;
	void set_to(uint32_t v);
	void toggle() const;
	bool read_raw();
	uint8_t is_on();
	void set_mode(PinMode mode);
	void set_speed(PinSpeed speed);
	void set_pull(PinPull pull);
	void set_alt(uint8_t af);
	void set_otype(PinOType otype);

private:
	GPIO port_;
	uint16_t pin_;
	PinPolarity polarity_;

	template<typename X>
	uint32_t HALParam(X x) {
		return static_cast<uint32_t>(x);
	}

	[[nodiscard]] auto GPIOPort(GPIO port_) const {
		return reinterpret_cast<GPIO_TypeDef *>(port_);
	}
	void _init(PinMode mode, uint8_t af, PinPull pull, PinSpeed speed, PinOType otype);
};

/////////////////////
// FPin: Fast Pin

template<GPIO Gpio, uint8_t PinNum>
using PinSetHigh = RegisterBits<WriteOnly, static_cast<uint32_t>(Gpio) + offsetof(GPIO_TypeDef, BSRR), (1UL << PinNum)>;

template<GPIO Gpio, uint8_t PinNum>
using PinSetLow =
	RegisterBits<WriteOnly, static_cast<uint32_t>(Gpio) + offsetof(GPIO_TypeDef, BSRR), (1UL << (16 + PinNum))>;

template<GPIO Gpio, uint8_t PinNum>
using PinRead = RegisterBits<ReadOnly, static_cast<uint32_t>(Gpio) + offsetof(GPIO_TypeDef, IDR), (1UL << PinNum)>;

template<GPIO Gpio>
using PortRead = RegisterBits<ReadOnly, static_cast<uint32_t>(Gpio) + offsetof(GPIO_TypeDef, IDR), 0xFFFF>;

template<GPIO Gpio, uint8_t PinNum, PinMode Mode = PinMode::Output, PinPolarity Polarity = PinPolarity::Normal>
struct FPin {
	static constexpr auto Gpio_v = Gpio;
	static constexpr auto PinNum_v = PinNum;

	// Todo: do our own init here using RegisterBits, don't rely on Pin class
	// Will still need to depend on RCC_Enable class
	FPin() {
		if constexpr (Gpio != GPIO::Unused)
			Pin init{Gpio, PinNum, Mode};
	}

	FPin(PinPull pull, PinSpeed speed = PinSpeed::Low, PinOType otype = PinOType::OpenDrain) {
		if constexpr (Gpio != GPIO::Unused)
			Pin init{Gpio, PinNum, Mode, 0, pull, PinPolarity::Normal, speed, otype};
	}

	static void low() {
		if constexpr (Gpio != GPIO::Unused) {
			static_assert(Mode == PinMode::Output, "Pin is not an output, cannot set low");
			if constexpr (Polarity == PinPolarity::Normal)
				_setlow.set();
			else
				_sethigh.set();
		}
	}
	static void high() {
		if constexpr (Gpio != GPIO::Unused) {
			static_assert(Mode == PinMode::Output, "Pin is not an output, cannot set high");
			if constexpr (Polarity == PinPolarity::Normal)
				_sethigh.set();
			else
				_setlow.set();
		}
	}
	static void set(bool state) {
		if constexpr (Gpio != GPIO::Unused) {
			static_assert(Mode == PinMode::Output, "Pin is not an output, cannot set high");
			if (state == (Polarity == PinPolarity::Normal))
				_sethigh.set();
			else
				_setlow.set();
		}
	}
	[[nodiscard]] static bool read() {
		if constexpr (Gpio != GPIO::Unused) {
			static_assert(Mode == PinMode::Input, "Pin is not an input, cannot read");
			if constexpr (Polarity == PinPolarity::Normal)
				return _read.read();
			else
				return !_read.read();
		} else
			return false;
	}

private:
	static PinSetLow<Gpio, PinNum> _setlow;
	static PinSetHigh<Gpio, PinNum> _sethigh;
	static PinRead<Gpio, PinNum> _read;
};
} // namespace mdrivlib
