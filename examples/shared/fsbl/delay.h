#pragma once
#include <stdint.h>

static inline void udelay(uint32_t usec)
{
	// at -O0 the loop is 6 cycles: 2 loads, 1 store, 1 branch, 1 sub, 1 cmp
	// So if usec=2 = 1.10us, and usec=10 = 2.87us,
	// then 8 loops = 48cycles is 1.77us, so 1 cycle = 36.87ns so clock is at 27MHz (assuming SYSRAM has no
	// wait states)
	const uint32_t loops_per_us = 5;
	const uint32_t loops = usec * loops_per_us;
	volatile uint32_t t = loops;
	while (t)
		t = t - 1;
}
