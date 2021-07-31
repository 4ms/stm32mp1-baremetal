// clang-format off
#include "doctest.h"
#include "stubs/stm32/fake_gpio.h"
#include "drivers/pin.hh"
// clang-format on

TEST_CASE("fake_gpio_tests, fake_pin_init_at_0") {
	bool initval = read_fake_pin(FakeGPIO::B, 1);
	CHECK_FALSE(initval);
}

TEST_CASE("fake_gpio_tests, fake_pin_reads_what_was_written") {
	// Port A
	//
	// high
	set_fake_pin(FakeGPIO::A, 0, true);
	bool postval = read_fake_pin(FakeGPIO::A, 0);
	CHECK(postval);

	// low
	set_fake_pin(FakeGPIO::A, 0, false);
	postval = read_fake_pin(FakeGPIO::A, 0);
	CHECK_FALSE(postval);

	// low again
	set_fake_pin(FakeGPIO::A, 0, false);
	postval = read_fake_pin(FakeGPIO::A, 0);
	CHECK_FALSE(postval);

	// high
	set_fake_pin(FakeGPIO::A, 0, true);
	postval = read_fake_pin(FakeGPIO::A, 0);
	CHECK(postval);

	// high again
	set_fake_pin(FakeGPIO::A, 0, true);
	postval = read_fake_pin(FakeGPIO::A, 0);
	CHECK(postval);

	// Port B
	// high
	set_fake_pin(FakeGPIO::B, 1, true);
	postval = read_fake_pin(FakeGPIO::B, 1);
	CHECK(postval);

	// low
	set_fake_pin(FakeGPIO::B, 1, false);
	postval = read_fake_pin(FakeGPIO::B, 1);
	CHECK_FALSE(postval);

	// low again
	set_fake_pin(FakeGPIO::B, 1, false);
	postval = read_fake_pin(FakeGPIO::B, 1);
	CHECK_FALSE(postval);

	// high
	set_fake_pin(FakeGPIO::B, 1, true);
	postval = read_fake_pin(FakeGPIO::B, 1);
	CHECK(postval);

	// high again
	set_fake_pin(FakeGPIO::B, 1, true);
	postval = read_fake_pin(FakeGPIO::B, 1);
	CHECK(postval);
}
