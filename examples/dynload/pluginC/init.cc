#include <cstddef>

extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));

void statics_init()
{
	size_t count = __preinit_array_end - __preinit_array_start;
	for (size_t i = 0; i < count; i++)
		__preinit_array_start[i]();

	count = __init_array_end - __init_array_start;
	for (size_t i = 0; i < count; i++)
		__init_array_start[i]();
}
