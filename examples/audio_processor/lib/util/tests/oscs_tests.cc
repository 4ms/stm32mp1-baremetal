#include "../oscs.hh"
#include "doctest.h"
#include <iostream>

TEST_CASE("Basic TriangleOscillator usage")
{
	// 1Hz triangle wave that we update (call Process()) at 100Hz
	const uint32_t updates_per_cycle = 120;
	TriangleOscillator<updates_per_cycle> tri;
	tri.set_frequency(1);

	uint32_t val;
	val = tri.val();
	CHECK(val == 0);

	int loops;
	float val_approx;
	// simulate 1 second, checking value every 0.25 seconds

	loops = updates_per_cycle / 8;
	while (loops--) {
		val = tri.Process();
	}
	val_approx = (float)val / (float)(0xFFFFFFFF);
	CHECK(val_approx == doctest::Approx(0.25));

	loops = updates_per_cycle / 8;
	while (loops--) {
		val = tri.Process();
	}
	val_approx = (float)val / (float)(0xFFFFFFFF);
	CHECK(val_approx == doctest::Approx(0.50));

	loops = updates_per_cycle / 4;
	while (loops--) {
		val = tri.Process();
	}
	val_approx = (float)val / (float)(0xFFFFFFFF);
	CHECK(val_approx == doctest::Approx(1.00));

	loops = updates_per_cycle / 4;
	while (loops--) {
		val = tri.Process();
	}
	val_approx = (float)val / (float)(0xFFFFFFFF);
	CHECK(val_approx == doctest::Approx(0.50));

	loops = updates_per_cycle / 4;
	while (loops--) {
		val = tri.Process();
	}
	val_approx = (float)val / (float)(0xFFFFFFFF);
	CHECK(val_approx == doctest::Approx(0.00));
}
