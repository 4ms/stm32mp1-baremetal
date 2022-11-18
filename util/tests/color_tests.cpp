#include "../colors.hh"
#include "doctest.h"
#include <stdio.h>

TEST_CASE("Rgb565 conversion tests")
{
	// Using online rgb565 color picker
	// values from: https://chrishewett.com/blog/true-rgb565-colour-picker/
	uint16_t red565 = 0xf800;
	uint16_t green565 = 0x07e0;
	uint16_t blue565 = 0x1f;
	uint16_t goldenrod565 = 0xee66;
	uint16_t greyish565 = 0x7bef;
	uint16_t white565 = 0xffff;
	uint16_t black565 = 0x0000;

	uint16_t mandarian565 = 0xb9e0; // 75% red, 25% green, no blue

	Color red{255, 0, 0};
	Color green{0, 255, 0};
	Color blue{0, 0, 255};
	Color goldenrod{0xef, 0xce, 0x31};
	Color greyish{0x7b, 0x7d, 0x7b};
	Color black{0, 0, 0};
	Color white{255, 255, 255};

	SUBCASE("Color to Rgb565")
	{
		CHECK(red.Rgb565() == red565);
		CHECK(green.Rgb565() == green565);
		CHECK(blue.Rgb565() == blue565);
		CHECK(goldenrod.Rgb565() == goldenrod565);
		CHECK(greyish.Rgb565() == greyish565);
		CHECK(black.Rgb565() == black565);
		CHECK(white.Rgb565() == white565);
	}

	SUBCASE("Rgb565 to Color")
	{
		Color c{goldenrod565};
		// values from: https://chrishewett.com/blog/true-rgb565-colour-picker/
		CHECK(c.red() == 29 << (8 - 5));
		CHECK(c.green() == 51 << (8 - 6));
		CHECK(c.blue() == 6 << (8 - 5));

		SUBCASE("..and back to Rgb565")
		{
			CHECK(c.Rgb565() == goldenrod565);
		}
	}

	SUBCASE("Blending Rgb565")
	{
		CHECK(Color::blend(black565, white565, 0.5f) == greyish565);
		CHECK(Color::blend(red565, green565, 0.25f) == mandarian565);
		CHECK(Color::blend(red565, green565, 0.f) == red565);
		CHECK(Color::blend(red565, green565, 1.f) == green565);
		CHECK(Color::blend(green565, red565, 1.f) == red565);
		CHECK(Color::blend(green565, red565, 0.f) == green565);
	}
}
