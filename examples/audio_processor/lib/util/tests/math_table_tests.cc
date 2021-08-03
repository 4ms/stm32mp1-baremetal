#include "../math.hh"
#include "../math_tables.hh"
#include "doctest.h"
#include <cmath>

const double TWOPI = 2. * (double)M_PI;

TEST_CASE("math_table tests: sinTable InterpArray")
{
	SUBCASE("sinTable starts at 0")
	{
		CHECK_EQ(0.f, sinTable[0]);
	}

	SUBCASE("sinTable by [index] is approx sin")
	{
		int table_size = sizeof(sinTable) / sizeof(float);

		for (int i = 0; i < table_size; i++) {
			double x = ((double)i / (double)table_size) * TWOPI;
			CHECK_EQ(sinTable[i], doctest::Approx(sin(x)));
		}
	}

	SUBCASE("sinTable wrapping by [index]")
	{
		SUBCASE("Linearity when wrapping: diff between sinTable[0] and [1] is approx diff between last element and [0]")
		{
			int table_size = sizeof(sinTable) / sizeof(sinTable[0]);
			auto diff_0_1 = sinTable[1] - sinTable[0];
			auto diff_last_first = sinTable[0] - sinTable[table_size - 1];
			CHECK_EQ(diff_0_1, doctest::Approx((double)diff_last_first));
		}
	}

	SUBCASE("sinTable.interp(x) is approx sin(2π * x)")
	{
		for (float x = 0; x < 1.f; x += 0.001f) {
			double rad = (double)x * TWOPI;
			CHECK_EQ(sinTable.interp(x), doctest::Approx(sin(rad)).epsilon(0.01));
		}
	}
	SUBCASE("sinTable.interp(x) for x > 1.f is invalid (not between -1 and +1)")
	{
		// FixMe: This can cause a crash, since it access memory out of bounds
		// CHECK(fabsf(sinTable.interp(1.001f)) > 1.f);
	}

	SUBCASE("sinTable.interp_wrap()")
	{
		SUBCASE("wrap(0) == wrap(1) == wrap(2) == 0")
		{
			CHECK_EQ(sinTable.interp_wrap(0.), doctest::Approx((double)sinTable.interp_wrap(1.f)));
			CHECK_EQ(sinTable.interp_wrap(1.), doctest::Approx((double)sinTable.interp_wrap(2.f)));
			CHECK_EQ(sinTable.interp_wrap(1.), doctest::Approx(0.));
		}

		SUBCASE("interp_wrap(x) == interp_wrap(1 + x)")
		{
			for (float x = 0; x < 2.f; x += 0.001f) {
				CHECK_EQ(sinTable.interp_wrap(x), doctest::Approx((double)sinTable.interp_wrap(x + 1.f)));
			}
		}

		SUBCASE("Linearity when wrapping: diff between interp_wrap(1) and interp_wrap(1 + epsilon) is same as "
				"interp_wrap(1 - epsilon) and interp_wrap(1) ")
		{
			float eps = 0.00001f;
			auto diff_1_1plusE = sinTable.interp_wrap(1.) - sinTable.interp_wrap(1.f + eps);
			auto diff_1minusE_1 = sinTable.interp_wrap(1.f - eps) - sinTable.interp_wrap(1.f);
			CHECK_EQ(diff_1_1plusE, doctest::Approx((double)diff_1minusE_1));
		}
	}
}
