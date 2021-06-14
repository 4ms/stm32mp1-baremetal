#include "pin.hh"
#include "util/debouncer.hh"
#include <stdint.h>

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

// struct DebouncedSwitch : public Debouncer<0x0000003F, 0xFFFFFFC0, 0x00000FFF> {
// 	DebouncedSwitch(GPIO port, uint8_t pin_num, PinPolarity polarity = PinPolarity::Inverted)
// 		: pin_(port,
// 			   pin_num,
// 			   PinMode::Input,
// 			   LL_GPIO_AF_0,
// 			   polarity == PinPolarity::Inverted ? PinPull::Up : PinPull::None,
// 			   polarity) {
// 	}

// 	void update() {
// 		unsigned pin_state = pin_.is_on() ? 1 : 0;
// 		register_state(pin_state);
// 	}

// private:
// 	Pin pin_;
// };
