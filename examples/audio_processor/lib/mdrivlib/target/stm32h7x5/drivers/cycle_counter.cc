#include "drivers/cycle_counter.hh"

namespace mdrivlib
{
void CycleCounter::init() {
	if (!(DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk)) {
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->LAR = 0xC5ACCE55;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
}

uint32_t CycleCounter::read_cycle_count() {
	return DWT->CYCCNT;
}
} // namespace mdrivlib
