#include "../interp_param.hh"
#include "doctest.h"
#include <array>

TEST_CASE("interp_param_tests: inits_to_zero")
{
	InterpParam<float, 6> x;
	CHECK(x.cur_val == 0.f);
	CHECK(x.next() == 0.f);
}

TEST_CASE("interp_param_tests: basic_usage")
{
	InterpParam<float, 6> x;
	x.set_new_value(12.f);
	CHECK(x.next() == 2.0f);
	CHECK(x.next() == 4.0f);
	CHECK(x.next() == 6.0f);
	CHECK(x.next() == 8.0f);
	CHECK(x.next() == 10.0f);
	CHECK(x.next() == 12.0f);
}

TEST_CASE("interp_param_tests: many_many_updates")
{
	const unsigned int updates = 0x1000000;
	InterpParam<double, updates> x;

	x.set_new_value(123456789.0);
	for (unsigned int i = 0; i < (updates - 1); i++)
		x.next();

	CHECK_EQ(x.next(), doctest::Approx(123456789.0).epsilon(0.000001));
}

TEST_CASE("interp_param_tests: goes_negative")
{
	InterpParam<long, 3> x;
	x.set_new_value(-12L);
	CHECK_EQ(-4L, x.next());
	CHECK_EQ(-8L, x.next());
	CHECK_EQ(-12L, x.next());
}

TEST_CASE("interp_param_tests: two_updates_opposite_directions")
{
	InterpParam<long, 3> x;
	x.set_new_value(-12L);
	CHECK_EQ(-4L, x.next());
	CHECK_EQ(-8L, x.next());
	CHECK_EQ(-12L, x.next());
	x.set_new_value(9L);
	CHECK_EQ(-5L, x.next());
	CHECK_EQ(2L, x.next());
	CHECK_EQ(9L, x.next());
}

TEST_CASE("interp_param_tests: overflow_keeps_incrementing")
{
	InterpParam<float, 6> x;
	x.set_new_value(12.f);
	CHECK(x.next() == 2.0f);
	CHECK(x.next() == 4.0f);
	CHECK(x.next() == 6.0f);
	CHECK(x.next() == 8.0f);
	CHECK(x.next() == 10.0f);
	CHECK(x.next() == 12.0f);
	CHECK_EQ(x.next(), doctest::Approx(14.));
}

TEST_CASE("interp_param_tests: zero_breaks")
{
	// InterpParam<float, 0> x;
}

TEST_CASE("interp_param_tests: step_size_is_nearly_zero_if_value_doesnt_change")
{
	InterpParam<float, 1> x;
	x.set_new_value(12.f);
	CHECK(x.next() == 12.f);
	x.set_new_value(12.f);
	CHECK_EQ(x.get_step_size(), doctest::Approx(0.));
}

