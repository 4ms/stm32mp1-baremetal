// clang-format off
#include "doctest.h"
#include "stubs/stm32/fake_gpio.h"
#include "drivers/pin.hh"
#include "drivers/rotary.hh"
// clang-format on

using namespace mdrivlib;

static void set_fake_rotary(uint8_t AB);
static void set_fake_rotary(uint8_t AB) {
	set_fake_pin(FakeGPIO::A, 0, AB & 0b10 ? false : true);
	set_fake_pin(FakeGPIO::B, 1, AB & 0b01 ? false : true);
}

// FixMe: Rotary tests will fail. Need to pass a Pin<> to RotaryEncoder::ctor, and override it with our own fake Pin
// class
TEST_CASE("Rotary tests" * doctest::skip(true)) {
	RotaryEncoder<RotaryHalfStep> rotary_half_step{GPIO::A, 0, GPIO::B, 1};
	RotaryEncoder<RotaryFullStep> rotary_full_step{GPIO::A, 0, GPIO::B, 1};

	SUBCASE("rotary_tests, starts_at_zero") {

		int32_t init_state = rotary_half_step.read();
		CHECK_EQ(0, init_state);
	}

	SUBCASE("rotary_tests, half_step_clockwise") {
		set_fake_pin(FakeGPIO::A, 0, true);
		set_fake_pin(FakeGPIO::B, 1, true);
		rotary_half_step.update();
		set_fake_pin(FakeGPIO::A, 0, false);
		set_fake_pin(FakeGPIO::B, 1, true);
		rotary_half_step.update();
		set_fake_pin(FakeGPIO::A, 0, false);
		set_fake_pin(FakeGPIO::B, 1, false);
		rotary_half_step.update();

		int32_t new_state = rotary_half_step.read();
		CHECK_EQ(1, new_state);
	}
	SUBCASE("rotary_tests, zeros_after_read") {
		set_fake_rotary(0b00);
		rotary_half_step.update();
		set_fake_rotary(0b10);
		rotary_half_step.update();
		set_fake_rotary(0b11);
		rotary_half_step.update();

		rotary_half_step.read();
		int32_t second_read = rotary_half_step.read();
		CHECK_EQ(0, second_read);
	}

	SUBCASE("rotary_tests, half_step_cw_starts_at_00") {
		set_fake_rotary(0b00);
		rotary_half_step.update();
		set_fake_rotary(0b10);
		rotary_half_step.update();
		set_fake_rotary(0b11);
		rotary_half_step.update();

		int32_t new_state = rotary_half_step.read();
		CHECK_EQ(1, new_state);
	}

	SUBCASE("rotary_tests, half_step_ccw_start_at_11") {
		set_fake_rotary(0b11);
		rotary_half_step.update();
		set_fake_rotary(0b10);
		rotary_half_step.update();
		set_fake_rotary(0b00);
		rotary_half_step.update();

		int32_t new_state = rotary_half_step.read();
		CHECK_EQ(-1, new_state);
	}

	SUBCASE("rotary_tests, half_step_ccw_starts_at_00") {
		set_fake_rotary(0b00);
		rotary_half_step.update();
		set_fake_rotary(0b01);
		rotary_half_step.update();
		set_fake_rotary(0b11);
		rotary_half_step.update();

		int32_t new_state = rotary_half_step.read();
		CHECK_EQ(-1, new_state);
	}
	SUBCASE("rotary_tests, two_ccw_half_steps") {
		set_fake_rotary(0b00);
		rotary_half_step.update();
		set_fake_rotary(0b01);
		rotary_half_step.update();
		set_fake_rotary(0b11);
		rotary_half_step.update();
		set_fake_rotary(0b10);
		rotary_half_step.update();
		set_fake_rotary(0b00);
		rotary_half_step.update();

		int32_t new_state = rotary_half_step.read();
		CHECK_EQ(-2, new_state);
	}

	SUBCASE("rotary_tests, full_step_ccw") {
		set_fake_rotary(0b00);
		rotary_full_step.update();
		set_fake_rotary(0b01);
		rotary_full_step.update();
		set_fake_rotary(0b11);
		rotary_full_step.update();
		set_fake_rotary(0b10);
		rotary_full_step.update();
		set_fake_rotary(0b00);
		rotary_full_step.update();

		CHECK_EQ(-1, rotary_full_step.read());
	}
	SUBCASE("rotary_tests, full_step_cw") {
		set_fake_rotary(0b00);
		rotary_full_step.update();
		set_fake_rotary(0b10);
		rotary_full_step.update();
		set_fake_rotary(0b11);
		rotary_full_step.update();
		set_fake_rotary(0b01);
		rotary_full_step.update();
		set_fake_rotary(0b00);
		rotary_full_step.update();

		CHECK_EQ(1, rotary_full_step.read());
	}
	SUBCASE("rotary_tests, two_full_steps_cw") {
		set_fake_rotary(0b00);
		rotary_full_step.update();
		set_fake_rotary(0b10);
		rotary_full_step.update();
		set_fake_rotary(0b11);
		rotary_full_step.update();
		set_fake_rotary(0b01);
		rotary_full_step.update();
		set_fake_rotary(0b00);
		rotary_full_step.update();

		set_fake_rotary(0b10);
		rotary_full_step.update();
		set_fake_rotary(0b11);
		rotary_full_step.update();
		set_fake_rotary(0b01);
		rotary_full_step.update();
		set_fake_rotary(0b00);
		rotary_full_step.update();

		CHECK_EQ(2, rotary_full_step.read());
	}
}
