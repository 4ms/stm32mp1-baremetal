#include "../interp_array.hh"
#include "doctest.h"

TEST_CASE("interp_array_tests: inits_with_single_value_sets_just_first_element")
{
	InterpArray<long, 4> x = {42};
	CHECK_EQ(42, x[0]);
	CHECK_EQ(0, x[1]);
	CHECK_EQ(0, x[2]);
	CHECK_EQ(0, x[2]);
}

TEST_CASE("interp_array_tests: inits_to_multiple_values")
{
	InterpArray<long, 3> x = {42, 43, 44};
	CHECK_EQ(42, x[0]);
	CHECK_EQ(43, x[1]);
	CHECK_EQ(44, x[2]);
}

TEST_CASE("interp_array_tests: basic_usage_by_phase")
{
	InterpArray<long, 4> x;
	x[0] = 200;	 // 0
	x[1] = 300;	 // 0.333
	x[2] = 400;	 // 0.666
	x[3] = 4000; // 1
	CHECK(((float)x.interp(1.f)) == doctest::Approx(4000.));
	CHECK(((float)x.interp(0.666666666f)) == doctest::Approx(400.));
	CHECK(((float)x.interp(0.5f)) == doctest::Approx(350.));
	CHECK(((float)x.interp(0.3333333333f)) == doctest::Approx(300.));
	CHECK(((float)x.interp(0.f)) == doctest::Approx(200.));
}

TEST_CASE("interp_array_tests: floats_by_phase")
{
	InterpArray<float, 3> x;
	x[0] = 100.f;  // 0
	x[1] = 200.f;  // 0.5
	x[2] = 2000.f; // 1

	float expected_0_75 = (x[1] + x[2]) / 2.f;
	CHECK(x.interp(0.75f) == doctest::Approx((double)expected_0_75));

	float expected_0_875 = (expected_0_75 + x[2]) / 2.f;
	CHECK(x.interp(0.875f) == doctest::Approx((double)expected_0_875));
}

TEST_CASE("interp_array_tests: doubles_by_phase")
{
	InterpArray<double, 3, double> x;
	x[0] = 100.;  // 0
	x[1] = 200.;  // 0.5
	x[2] = 2000.; // 1

	double expected_0_75 = (x[1] + x[2]) / 2.;
	CHECK(x.interp(0.75) == doctest::Approx(expected_0_75));

	double expected_0_875 = (expected_0_75 + x[2]) / 2.;
	CHECK(x.interp(0.875) == doctest::Approx(expected_0_875));
}

TEST_CASE("interp_array_tests: basic_usage_by_index")
{
	InterpArray<long, 300> x;
	x[201] = 4200;
	x[202] = 4300;
	CHECK(((float)x.interp_by_index(202.f)) == doctest::Approx(4300.));
	CHECK(((float)x.interp_by_index(201.75f)) == doctest::Approx(4275.));
	CHECK(((float)x.interp_by_index(201.5f)) == doctest::Approx(4250.));
	CHECK(((float)x.interp_by_index(201.25f)) == doctest::Approx(4225.));
	CHECK(((float)x.interp_by_index(201.f)) == doctest::Approx(4200.));
}

TEST_CASE("interp_array_tests: floats_by_index")
{
	InterpArray<float, 3> x;
	x[0] = 100.f;
	x[1] = 200.f;
	CHECK(((float)x.interp_by_index(0.156f)) == doctest::Approx(115.6));
}

TEST_CASE("interp_array_tests: interp_wrap_period_is_1")
{
	InterpArray<float, 3> x;
	x[0] = 100.f;
	x[1] = 200.f;
	x[2] = 2000.f;
	CHECK_EQ(x.interp_wrap(0.0f), x.interp_wrap(1.0f));
}

TEST_CASE("interp_array_tests: wrapping_by_phase")
{
	InterpArray<float, 3> x;
	x[0] = 100.f;  // 0.00, 1.00, 2.00...
	x[1] = 200.f;  // 0.33, 1.33, 2.33...
	x[2] = 2000.f; // 0.66, 1.66, 2.66...

	for (int i = 0; i < 10; i++) {
		float y = (float)i / 3.f;
		CHECK_EQ(x[i % 3], x.interp_wrap(y));
	}
}

TEST_CASE("interp_array_tests: wrapping_by_phase_loop_transition_is_smooth")
{
	InterpArray<float, 3> x;
	x[0] = 100.f;  // 0.00, 1.00, 2.00...
	x[1] = 200.f;  // 0.33, 1.33, 2.33...
	x[2] = 2000.f; // 0.66, 1.66, 2.66...

	// Start point
	CHECK_EQ(x[2], x.interp_wrap(2.0f / 3.0f));

	// End point
	CHECK_EQ(x[0], x.interp_wrap(3.0f / 3.0f));

	// Half-way point
	float loop_transition_middle = (x[2] + x[0]) / 2.0f;
	CHECK_EQ(loop_transition_middle, x.interp_wrap(2.5f / 3.0f));
}

TEST_CASE("interp_array_tests: index_wrap_period_is_Size")
{
	const int Size = 3;
	InterpArray<float, Size> x;
	x[0] = 100.f;
	x[1] = 200.f;
	x[2] = 2000.f;
	CHECK_EQ(x.interp_by_index_wrap(0), x.interp_by_index_wrap((float)Size));
}

TEST_CASE("interp_array_tests: wrapping_by_index")
{
	InterpArray<float, 3> x;
	x[0] = 100.f;  // 0.00, 1.00, 2.00...
	x[1] = 200.f;  // 0.33, 1.33, 2.33...
	x[2] = 2000.f; // 0.66, 1.66, 2.66...

	for (int i = 0; i < 10; i++) {
		CHECK_EQ(x[i % 3], x.interp_by_index_wrap((float)i));
	}
}

TEST_CASE("interp_array_tests: basic_usage_of_closest")
{
	InterpArray<float, 3> x;
	x[0] = 100.f;
	x[1] = 200.f;
	x[2] = 150.f;
	CHECK(((float)x.closest(0.0f)) == doctest::Approx(100.));
	CHECK(((float)x.closest(0.49999f)) == doctest::Approx(100.));
	CHECK(((float)x.closest(0.5f)) == doctest::Approx(200.));
	CHECK(((float)x.closest(0.6f)) == doctest::Approx(200.));
	CHECK(((float)x.closest(0.9999f)) == doctest::Approx(200.));
	CHECK(((float)x.closest(1.f)) == doctest::Approx(150.));
}

TEST_CASE("interp_array_tests: wrapping_with_closest")
{
	InterpArray<float, 3> x;
	x[0] = 100.f; // 0.00, 1.00, 2.00, ...
	x[1] = 200.f; // 0.33, 1.33, 2.33, ...
	x[2] = 150.f; // 0.66, 1.66, 2.66, ...

	CHECK_EQ(x[0], x.closest_wrap(0.f / 3.f));
	CHECK_EQ(x[0], x.closest_wrap(1.f + 0.f / 3.f));

	CHECK_EQ(x[1], x.closest_wrap(1.f / 3.f));
	CHECK_EQ(x[1], x.closest_wrap(1.f + 1.f / 3.f));

	CHECK_EQ(x[2], x.closest_wrap(2.f / 3.f));
	CHECK_EQ(x[2], x.closest_wrap(1.f + 2.f / 3.f));

	CHECK_EQ(x[0], x.closest_wrap(0.499f / 3.f));
	CHECK_EQ(x[0], x.closest_wrap(0.999f / 3.f));

	CHECK_EQ(x[1], x.closest_wrap(1.499f / 3.f));
	CHECK_EQ(x[1], x.closest_wrap(1.999f / 3.f));
}
