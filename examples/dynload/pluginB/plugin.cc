#include <cstdint>

// defined in calc.cc
uint32_t internal_func(uint32_t x, uint32_t y);

// not defined in plugin
void external_func();

const uint32_t exported_data[] = {
	0xAABBCCDD,
	0xABBCCDD,
	0xBBCCDD,
	0xBCCDD,
	0xCCDD,
};

// This creates the init_array symbol
const uint32_t runtime_init_data = internal_func(0, 0);

void init(int *p)
{
	*p += internal_func(exported_data[*p], *p);
	external_func();
}
