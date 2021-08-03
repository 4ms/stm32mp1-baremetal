#pragma once
#include "pin.hh"
#include "util/debouncer.hh"
#include <cstdint>

namespace mdrivlib
{
template<GPIO Gpio, uint16_t PinNum, PinPolarity Polarity>
struct DebouncedPin : public Debouncer<0x00000001, 0xFFFFFFFE, 0x00000FFF> {
	DebouncedPin() {
		FPin<Gpio, PinNum, PinMode::Input> init_pin{Polarity == PinPolarity::Inverted ? PinPull::Up : PinPull::None};
	}

	void update() {
		unsigned pin_state;
		if constexpr (Polarity == PinPolarity::Normal)
			pin_state = FPin<Gpio, PinNum, PinMode::Input>::read() ? 1 : 0;
		if constexpr (Polarity == PinPolarity::Inverted)
			pin_state = FPin<Gpio, PinNum, PinMode::Input>::read() ? 0 : 1;
		register_state(pin_state);
	}
};

template<GPIO Gpio, uint16_t PinNum, PinPolarity Polarity>
using DebouncedSwitch = DebouncedPin<Gpio, PinNum, Polarity>;

} // namespace mdrivlib
