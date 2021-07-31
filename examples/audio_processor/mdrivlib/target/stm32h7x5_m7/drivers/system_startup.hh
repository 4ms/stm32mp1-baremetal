#pragma once
#include "drivers/hsem.hh"
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{

struct SystemStartup {

	static constexpr bool DEBUG_MODE_DISABLE_I_CACHE = false;
	static constexpr bool DEBUG_MODE_DISABLE_D_CACHE = false;

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

		if constexpr (DEBUG_MODE_DISABLE_I_CACHE)
			SCB_DisableICache();
		else
			SCB_EnableICache();

		if constexpr (DEBUG_MODE_DISABLE_D_CACHE)
			SCB_DisableDCache();
		else
			SCB_EnableDCache();
	}

	static void init_clocks(const RCC_OscInitTypeDef &osc_def,
							const RCC_ClkInitTypeDef &clk_def,
							const RCC_PeriphCLKInitTypeDef &pclk_def,
							const uint32_t systick_freq_hz = 1000) {
		poweron();

		// Todo: const_cast or fix HAL_RCC_ functions to take a const struct
		RCC_OscInitTypeDef osc_def_ = osc_def;
		HAL_RCC_OscConfig(&osc_def_);

		RCC_ClkInitTypeDef clk_def_ = clk_def;
		HAL_RCC_ClockConfig(&clk_def_, FLASH_LATENCY_7);

		RCC_PeriphCLKInitTypeDef pclk_def_ = pclk_def;
		HAL_RCCEx_PeriphCLKConfig(&pclk_def_);

		HAL_RCC_EnableCSS();

		HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / systick_freq_hz);
		HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	}

	static void poweron() {
		// Replace with register access:
		HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
		__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
		while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
		}
		__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
	}
};
} // namespace mdrivlib
