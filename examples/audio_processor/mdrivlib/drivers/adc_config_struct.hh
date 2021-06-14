#pragma once
#include "stm32xx.h"

#if defined(STM32H755xx) || defined(STM32H745xx)
struct AdcPeriphConfig {
	ADC_TypeDef *adc;
	uint32_t CommonClock;
	uint32_t Multimode;
	uint32_t MultiDMATransfer;
	uint32_t MultiTwoSamplingDelay;
	uint32_t Resolution;
	uint32_t LeftBitShift;
	uint32_t LowPowerMode;
};
#elif defined(STM32F7)

#endif
