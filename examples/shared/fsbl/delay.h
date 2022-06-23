#include <stdint.h>

static inline void udelay(uint32_t usec)
{
	const unsigned long cycles = 120UL * 800000000UL * usec / 1000UL;
	volatile unsigned long t = cycles;
	while (t)
		t = t - 1;
}
