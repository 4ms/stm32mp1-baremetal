#include <cstdint>

static uint32_t readwrite_data[] = {
	0xCCDDEEFF,
	0xCDDEEFFD,
	0xDDEEFFDD,
	0xDEEFFCCD,
};

uint32_t internal_func(uint32_t x, uint32_t y)
{
	// Modify readwrite_data so compiler doesn't treat it as const
	auto x0 = readwrite_data[y];
	readwrite_data[y] = x;
	return x0;
}
