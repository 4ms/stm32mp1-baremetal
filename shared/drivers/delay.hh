#include <cstdint>

struct Delay {
	static void cycles(uint32_t cycles)
	{
		while (cycles--)
			asm volatile("NOP \n");
	}
};
