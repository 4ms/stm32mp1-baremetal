#include "doctest.h"
#include "drivers/frame_buffer_led.hh"

TEST_CASE("frame_buffer_led_tests") {
	uint32_t fb[12];
	const uint32_t DUMMYVAL = 55;

	for (uint8_t i = 0; i < 12; i++) {
		fb[i] = DUMMYVAL;
	}

	SUBCASE("init_doesnt_change_buffer") {
		FrameBufferLED led{&fb[0]};
		CHECK_EQ(DUMMYVAL, fb[0]);
	}

	SUBCASE("set_and_read_first_value_of_array") {
		FrameBufferLED led{&fb[0]};
		int kShiftAmount = 20; // Todo: this is the result of what should be a template param for FrameBufferLED
		led.set(12);
		CHECK_EQ(12 << kShiftAmount, fb[0]);
	}

	SUBCASE("set_and_read_last_value_of_array") {
		FrameBufferLED led{&fb[11]};
		led.set(65536U);
		CHECK_EQ(65536U << 16, fb[11]);
	}

	SUBCASE("set_32b_value_doesnt_overflow") {
		FrameBufferLED led{&fb[2]};
		led.set(0xFFFFFFFF);
		CHECK_EQ(DUMMYVAL, fb[1]);
		CHECK_EQ(DUMMYVAL, fb[3]);
	}
};
