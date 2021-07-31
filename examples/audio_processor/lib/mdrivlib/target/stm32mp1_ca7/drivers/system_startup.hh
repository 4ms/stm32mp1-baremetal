#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib
{

struct SystemStartup {
	// Todo: Put this into mdrivlib
	static void init_clocks(const RCC_OscInitTypeDef &osc_def,
							const RCC_ClkInitTypeDef &clk_def,
							const RCC_PeriphCLKInitTypeDef &pclk_def,
							const uint32_t systick_freq_hz = 1000) {
		// Reset MPU clock selection, so we can change it
		RCC->MPCKSELR = 0;
		while ((RCC->MPCKSELR & RCC_MPCKSELR_MPUSRCRDY_Msk) == 0)
			;

		RCC_OscInitTypeDef osc_def_ = osc_def;
		HAL_RCC_OscConfig(&osc_def_);

		RCC_ClkInitTypeDef clk_def_ = clk_def;
		HAL_RCC_ClockConfig(&clk_def_);

		RCC_PeriphCLKInitTypeDef pclk_def_ = pclk_def;
		HAL_RCCEx_PeriphCLKConfig(&pclk_def_);
	}
};
} // namespace mdrivlib
