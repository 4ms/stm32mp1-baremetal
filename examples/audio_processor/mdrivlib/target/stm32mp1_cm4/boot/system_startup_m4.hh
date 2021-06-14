#pragma once
#include "drivers/hsem.hh"
#include "drivers/interrupt.hh"
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"
#include "drivers/system.hh"

namespace mdrivlib
{
namespace stm32mp1
{
namespace core_a7
{
struct SystemClocks {
	SystemClocks() {
		// Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
		// perform system initialization
		// HAL_PWREx_ClearPendingEvent();
		// HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);

		// // Wakeup here
		// __HAL_RCC_ART_CLK_ENABLE();
		// __HAL_ART_CONFIG_BASE_ADDRESS(0x10000000UL);
		// __HAL_ART_ENABLE();

		NVIC_SetPriorityGrouping(0b101); // 2/2

		SystemCoreClock = HAL_RCC_GetSystemCoreClockFreq();

		HAL_InitTick(TICK_INT_PRIORITY);

		// target::RCC_Enable::SYSCFG_::set();
	}
};
} // namespace core_a7
} // namespace stm32mp1
} // namespace mdrivlib
