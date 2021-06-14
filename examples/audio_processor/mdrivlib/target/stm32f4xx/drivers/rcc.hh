#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib {
namespace stm32f4xx {
namespace RCC_Enable {
// Todo: Can we just copy F7 values?
using RegisterDataT = uint32_t;
} // namespace RCC_Enable
} // namespace stm32f4xx
// stm32f746xx.h:

// Todo: convert this to new format:
// enum class ADCEnableBit : uint32_t {
// 	_1 = RCC_APB2ENR_ADC1EN,
// 	_2 = RCC_APB2ENR_ADC2EN,
// 	_3 = RCC_APB2ENR_ADC3EN,
// };
// enum class DMAEnableBit : uint32_t {
// 	_1 = RCC_AHB1ENR_DMA1EN,
// 	_2 = RCC_AHB1ENR_DMA2EN,
// };
// enum class I2CEnableBit : uint32_t {
// 	_1 = RCC_APB1ENR_I2C1EN,
// 	_2 = RCC_APB1ENR_I2C2EN,
// 	_3 = RCC_APB1ENR_I2C3EN,
// };
// enum class SAIEnableBit : uint32_t {
// 	_1 = RCC_APB2ENR_SAI1EN,
// 	_2 = RCC_APB2ENR_SAI2EN,
// 	_3 = RCC_APB2ENR_SAI3EN,
// };
// enum class SAI4EnableBit : uint32_t {
// 	_4 = RCC_APB4ENR_SAI4EN,
// };

// struct TIM : RCCPeriphs<TIM> {
// 	volatile static inline RegisterT *const _regs[NumP] = {
// 		&RCC->APB2ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB2ENR,
// 	};
// 	const static inline RegisterT _bits[NumP] = {
// 		RCC_APB2ENR_TIM1EN,
// 		RCC_APB1ENR_TIM2EN,
// 		RCC_APB1ENR_TIM3EN,
// 		RCC_APB1ENR_TIM4EN,
// 		RCC_APB1ENR_TIM5EN,
// 		RCC_APB1ENR_TIM6EN,
// 		RCC_APB1ENR_TIM7EN,
// 		RCC_APB2ENR_TIM8EN,
// 		RCC_APB2ENR_TIM9EN,
// 		RCC_APB2ENR_TIM10EN,
// 		RCC_APB2ENR_TIM11EN,
// 		RCC_APB1ENR_TIM12EN,
// 		RCC_APB1ENR_TIM13EN,
// 		RCC_APB1ENR_TIM14EN,
// 		RCC_APB2ENR_TIM15EN,
// 		RCC_APB2ENR_TIM16EN,
// 		RCC_APB2ENR_TIM17EN,
// 	};
// };
} // namespace mdrivlib
