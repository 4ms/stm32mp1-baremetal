#include <cstddef>

// extern void (*__preinit_array_start[])(void) __attribute__((weak));
// extern void (*__preinit_array_end[])(void) __attribute__((weak));

// TODO: use a link script to set these symbols before/after .rel.init_array
// extern void (*__init_array_start[])(void) __attribute__((weak));
// extern void (*__init_array_end[])(void) __attribute__((weak));

extern void (*init_array)(void) __attribute__((weak));

void statics_init()
{
	// size_t count = __preinit_array_end - __preinit_array_start;
	// for (size_t i = 0; i < count; i++)
	// 	__preinit_array_start[i]();

	// size_t count = __init_array_end - __init_array_start;
	// for (size_t i = 0; i < count; i++)
	// 	__init_array_start[i]();
	init_array();
}
