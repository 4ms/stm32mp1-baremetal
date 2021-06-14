#pragma once
#include "stm32xx.h"
#include <cstdint>

namespace mdrivlib {
namespace stm32f7xx {
namespace peripherals {
struct TIM {
  static uint32_t max_freq(TIM_TypeDef *TIM) {
    // APB2 --> divider = 1;
    // APB1 --> divider = 2;
    uint32_t divider;
    auto TIMx_BASE_ADDR = reinterpret_cast<uintptr_t>(TIM);
    if (TIMx_BASE_ADDR >= APB1PERIPH_BASE && TIMx_BASE_ADDR < APB2PERIPH_BASE)
      divider = 2;
    else if (TIMx_BASE_ADDR >= APB2PERIPH_BASE)
      divider = 1;
    else
      divider = 1; // unknown, error?
    return HAL_RCC_GetHCLKFreq() / divider;
  }
  static constexpr uint32_t max_period(unsigned tim_periph_num) { return 65535; }
  static constexpr uint32_t max_prescaler(unsigned tim_periph_num) { return 255; }
  static constexpr uint32_t max_clockdivider(unsigned tim_periph_num) {
    // if (tim_periph_num == 1)
    // 	return 255;
    return 255;
  }
  static constexpr uint32_t next_allowed_clockdivision(uint32_t tim_periph_num, uint32_t clock_division) { return clock_division + 1; }
};

} // namespace peripherals

} // namespace stm32f7xx

} // namespace mdrivlib
