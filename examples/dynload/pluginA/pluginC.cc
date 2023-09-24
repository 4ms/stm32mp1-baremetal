#include <cstddef>
void statics_init();

// Global data (bss)
int zero_data{};

// Global data
int init_data[4]{1, 2, 3, 4};

// Global data (RO)
const int const_data[4]{11, 22, 33, 44};

// Runtime calculated data (on plugin load)
unsigned calc_something(unsigned);
unsigned init_runtime[4]{calc_something(3), calc_something(6), calc_something(9), calc_something(12)};

// C++ static data
struct StaticData {
	static inline const unsigned data[4]{9, 10, 11, 12};
	static inline constexpr unsigned constexpr_data[4]{9, 10, 11, 12};
};

__attribute__((section(".ismain"))) void init_plugin(int *result)
{
	statics_init();

	*result = zero_data;

	for (auto d : init_data)
		*result += d;

	for (auto d : const_data)
		*result += d;

	for (auto d : init_runtime)
		*result += d;

	for (auto d : StaticData::data)
		*result += d;

	for (auto d : StaticData::constexpr_data)
		*result += d;
}
