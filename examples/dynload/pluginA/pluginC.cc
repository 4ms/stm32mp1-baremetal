#include <cstddef>

// internal functions
unsigned calc_something(unsigned);
void statics_init();

// foreign function
int external_call(int);

// Global data (bss)
int zero_data{};

// Global data
int init_data[4]{1, 2, 3, 4};

// Runtime calculated data (on plugin load)
unsigned init_runtime = calc_something(3);

// External RO data
extern const int const_data[4]; //{0xAAAA, 0x55BB, 0x1234, 0x12345678};

// Internal RO data
struct StaticData {
	static inline const unsigned data[4]{9, 10, 11, 12};
};

__attribute__((section(".ismain"))) void init_plugin(int *result)
{
	statics_init();

	*result = zero_data;

	for (auto d : init_data)
		*result += d;

	for (auto d : const_data)
		*result += d;

	*result += init_runtime;

	for (auto d : StaticData::data)
		*result += d;

	*result += external_call(*result);
}
