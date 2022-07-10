#pragma once
#include <stdint.h>

// FIXME: not really usec
static inline void udelay(uint32_t usec)
{
	const uint32_t loops_per_us = 5;
	const uint32_t loops = usec * loops_per_us;
	volatile uint32_t t = loops;
	while (t)
		t = t - 1;
}
