// clang-format off
#include "tests/stubs/stm32/fake_gpio.h"
#include "drivers/pin.hh"
// clang-format on

static FakeGPIO fake_port(GPIO port);
static FakeGPIO fake_port(GPIO port) {
	// clang-format off
	return port==GPIO::A ? FakeGPIO::A
		: port==GPIO::B ? FakeGPIO::B
		: port==GPIO::C ? FakeGPIO::C
		: port==GPIO::D ? FakeGPIO::D
		: port==GPIO::E ? FakeGPIO::E
		: port==GPIO::F ? FakeGPIO::F
		: port==GPIO::G ? FakeGPIO::G
		: port==GPIO::H ? FakeGPIO::H
#ifdef GPIOI
		: port==GPIO::I ? FakeGPIO::I
#endif
#ifdef GPIOJ
		: port==GPIO::J ? FakeGPIO::J
#endif
#ifdef GPIOK
		: port==GPIO::K ? FakeGPIO::K
#endif
		: FakeGPIO::A;
}
// clang-format on

Pin::Pin(GPIO port,
		 uint8_t pin,
		 PinMode mode,
		 uint8_t af,
		 PinPull pull,
		 PinPolarity polarity,
		 PinSpeed speed,
		 PinOType otype)
	: port_(port)
	, pin_(pin)
	, polarity_(polarity) {
}

void Pin::high() const {
}
void Pin::low() const {
}
void Pin::on() const {
}
void Pin::off() const {
}
void Pin::set_to(uint32_t v) {
}

bool Pin::read_raw() {
	bool state = read_fake_pin(fake_port(port_), pin_);
	// printf("Reading raw pin %d = %d\n", pin_, state);
	return state;
}
uint8_t Pin::is_on() {
	if (polarity_ == PinPolarity::Normal) {
		bool state = read_fake_pin(fake_port(port_), pin_);
		// printf("Reading pin %d = %d\n", pin_, state);
		return state;
	} else {
		bool state = !read_fake_pin(fake_port(port_), pin_);
		// printf("Reading inverted pin %d = %d\n", pin_, state);
		return state;
	}
}

void Pin::set_mode(PinMode mode) {
}
void Pin::set_speed(PinSpeed speed) {
}
void Pin::set_pull(PinPull pull) {
}
void Pin::set_alt(uint8_t af) {
}
void Pin::set_otype(PinOType otype) {
}
