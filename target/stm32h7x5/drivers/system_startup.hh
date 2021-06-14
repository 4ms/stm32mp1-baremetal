#pragma once
#include "drivers/hsem.hh"
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"

// #define DEBUG_MODE_DISABLE_I_CACHE
// #define DEBUG_MODE_DISABLE_D_CACHE

namespace mdrivlib
{
namespace stm32h7x5
{
namespace corem7
{
struct SystemStartup {
	static void wait_for_cm4_sleep() {
		int32_t timeout = 0xFFFF;
		while (RCC_Clocks::D2CKREADY::read()) {
			if (timeout-- == 0)
				break;
		}
		if (timeout <= 0)
			__BKPT(); // Handle error
	}

	static void tell_cm4_to_wakeup() {
		// Signal to Core M4 to power up (triggers an ISR on M4)
		HWSemaphore<15>::lock();
		HWSemaphore<15>::unlock();
	}

	static void init_caches() {
		SCB_InvalidateDCache();

#ifdef DEBUG_MODE_DISABLE_I_CACHE
		SCB_DisableICache();
#else
		SCB_EnableICache();
#endif

#ifdef DEBUG_MODE_DISABLE_D_CACHE
		SCB_DisableDCache();
#else
		SCB_EnableDCache();
#endif
	}
};
} // namespace corem7

} // namespace stm32h7x5
} // namespace mdrivlib
