#include "fake_gpio.h"

static bool FAKE_PINS[11][16] = {{0}};

void set_fake_pin(FakeGPIO port, uint16_t pin_num, bool newstate)
{
	auto portnum = static_cast<uint8_t>(port);
	FAKE_PINS[portnum][pin_num] = newstate;
}

bool read_fake_pin(FakeGPIO port, uint16_t pin_num)
{
	auto portnum = static_cast<uint8_t>(port);
	return FAKE_PINS[portnum][pin_num];
}
