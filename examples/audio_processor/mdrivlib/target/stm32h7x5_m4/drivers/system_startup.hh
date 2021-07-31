#pragma once
#include "hsem.hh"
#include "interrupt.hh"
#include "rcc.hh"
#include "stm32xx.h"

namespace mdrivlib
{
struct SystemStartup {
	SystemStartup() = delete;

	static void init_clocks() {
		RCC_Enable::HSEM_::set();

		// Enable notification in order to wakeup
		HWSemaphore<15>::enable_channel_ISR();
		HAL_NVIC_EnableIRQ(HSEM2_IRQn);
		InterruptManager::registerISR(HSEM2_IRQn, 0, 0, []() {
			HWSemaphore<15>::clear_ISR();
			HAL_NVIC_DisableIRQ(HSEM2_IRQn);
		});

		// Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
		// perform system initialization
		HAL_PWREx_ClearPendingEvent();
		HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);

		// Wakeup here
		__HAL_RCC_ART_CLK_ENABLE();
		__HAL_ART_CONFIG_BASE_ADDRESS(0x08100000UL);
		__HAL_ART_ENABLE();

		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

		uint32_t common_system_clock;
		common_system_clock = HAL_RCC_GetSysClockFreq() >>
							  ((D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_D1CPRE) >> RCC_D1CFGR_D1CPRE_Pos]) & 0x1FU);

		SystemD2Clock = (common_system_clock >>
						 ((D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_HPRE) >> RCC_D1CFGR_HPRE_Pos]) & 0x1FU));
		SystemCoreClock = SystemD2Clock;
		HAL_InitTick(TICK_INT_PRIORITY);

		RCC_Enable::SYSCFG_::set();
	}
};
} // namespace mdrivlib
