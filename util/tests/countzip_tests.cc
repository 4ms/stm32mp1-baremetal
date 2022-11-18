#include "../countzip.hh"
#include "doctest.h"

#include <array>
// #include <iostream>

struct Frame {
	int l;
	int r;
};

TEST_CASE("Basic countzip usage")
{
	std::array<float, 4> x_arr = {0, 0, 0, 0};
	std::array<Frame, 4> y_arr = {{{0, 1}, {2, 3}, {4, 5}, {6, 7}}};
	std::array<Frame, 4> z_arr = {{{9, 1}, {8, 3}, {7, 5}, {6, 7}}};

	SUBCASE("Zip 1 array, read-only")
	{
		int check_i = 0;
		for (auto [n, x] : countzip(x_arr)) {
			CHECK(x == x_arr[n]);
			CHECK(n == check_i);
			check_i++;
		}
	}

	SUBCASE("Zip 2 arrays, read-only")
	{
		int check_i = 0;
		for (auto [n, x, y] : countzip(x_arr, y_arr)) {
			CHECK(x == x_arr[n]);
			CHECK(y.l == y_arr[n].l);
			CHECK(y.r == y_arr[n].r);
			CHECK(n == check_i);
			check_i++;
		}
	}

	SUBCASE("Zip 3 arrays, read-only")
	{
		int check_i = 0;
		for (auto [n, x, y, z] : countzip(x_arr, y_arr, z_arr)) {
			CHECK(x == x_arr[n]);
			CHECK(y.l == y_arr[n].l);
			CHECK(y.r == y_arr[n].r);
			CHECK(z.l == z_arr[n].l);
			CHECK(z.r == z_arr[n].r);
			CHECK(n == check_i);
			check_i++;
		}
	}

	SUBCASE("Zip 3 arrays, modify one and the new data persists")
	{
		for (auto [n, x, y, z] : countzip(x_arr, y_arr, z_arr)) {
			// std::cout << n << ": " << x << ", " << y.l << "|" << y.r << "    " << z.l << "|" << z.r << std::endl;
			float f = n * 0.2f;
			x = y.l * f + y.r * (1.f - f);
		}

		CHECK(x_arr[0] == doctest::Approx(1.f));
		CHECK(x_arr[1] == doctest::Approx(2.8f));
		CHECK(x_arr[2] == doctest::Approx(4.6f));
		CHECK(x_arr[3] == doctest::Approx(6.4f));
	}

	SUBCASE("Can modify values even if using `const auto`")
	{
		for (const auto [n, x] : countzip(x_arr)) {
			x = n;
		}

		CHECK(x_arr[0] == doctest::Approx(0.f));
		CHECK(x_arr[1] == doctest::Approx(1.f));
		CHECK(x_arr[2] == doctest::Approx(2.f));
		CHECK(x_arr[3] == doctest::Approx(3.f));
	}
}
