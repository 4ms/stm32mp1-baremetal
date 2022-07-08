#pragma once
#include "arch.hh"
#include "drivers/rcc.hh"
#include "periph.hh"
#include "stm32xx.h"

// Todo: once all drivers are using constexpr init, much of these can go away,
// and we'll use RCC_Enable::ADC<N>::set() instead

// Wrapper class for RCC_Enable, which allows for enabling/disabling the clock (RCC) of PeriphUtil using run-time
// values

namespace mdrivlib::Clocks
{
struct ADC {
	static void enable(const unsigned periph_num) {
		if (periph_num == 1)
			RCC_Enable::ADC1_::set();
		else if (periph_num == 2)
			RCC_Enable::ADC2_::set();
		else if (periph_num == 3)
			RCC_Enable::ADC3_::set();
	}
	static void enable(ADC_TypeDef *ADCx) {
		enable(PeriphUtil::ADC::to_num(ADCx));
	}
	static void disable(const unsigned periph_num) {
		if (periph_num == 1)
			RCC_Enable::ADC1_::clear();
		else if (periph_num == 2)
			RCC_Enable::ADC2_::clear();
		else if (periph_num == 3)
			RCC_Enable::ADC3_::clear();
	}
	static void disable(ADC_TypeDef *ADCx) {
		disable(PeriphUtil::ADC::to_num(ADCx));
	}
};

struct DMA {
	static void enable(const DMA_TypeDef *DMAx) {
		if (DMAx == nullptr)
			return;
		else if (DMAx == DMA1)
			RCC_Enable::DMA1_::set();
		else if (DMAx == DMA2)
			RCC_Enable::DMA2_::set();
		else if (DMAx == reinterpret_cast<DMA_TypeDef *>(BDMA))
			RCC_Enable::BDMA_::set();
	}
	static void disable(const DMA_TypeDef *DMAx) {
		if (DMAx == nullptr)
			return;
		else if (DMAx == DMA1)
			RCC_Enable::DMA1_::clear();
		else if (DMAx == DMA2)
			RCC_Enable::DMA2_::clear();
		else if (BDMA && DMAx == reinterpret_cast<DMA_TypeDef *>(BDMA))
			RCC_Enable::BDMA_::clear();
	}
};

struct DMAMUX {
	static void enable() {
		RCC_Enable::DMAMUX_::set();
	}
};

struct I2C {
	static void enable(I2C_TypeDef *I2Cx) {
		if (I2Cx == nullptr)
			return;
		else if (I2Cx == I2C1)
			RCC_Enable::I2C1_::set();
		else if (I2Cx == I2C2)
			RCC_Enable::I2C2_::set();
		else if (I2Cx == I2C3)
			RCC_Enable::I2C3_::set();
	}
	static void disable(I2C_TypeDef *I2Cx) {
		if (I2Cx == nullptr)
			return;
		else if (I2Cx == I2C1)
			RCC_Enable::I2C1_::clear();
		else if (I2Cx == I2C2)
			RCC_Enable::I2C2_::clear();
		else if (I2Cx == I2C3)
			RCC_Enable::I2C3_::clear();
	}
	static void force_reset(I2C_TypeDef *I2Cx) {
		if (I2Cx == nullptr) {
			return;
		} else if (I2Cx == I2C1) {
			RCC_Reset::I2C1_::set();
			RCC_Reset::I2C1_::clear();
		} else if (I2Cx == I2C2) {
			RCC_Reset::I2C2_::set();
			RCC_Reset::I2C2_::clear();
		} else if (I2Cx == I2C3) {
			RCC_Reset::I2C3_::set();
			RCC_Reset::I2C3_::clear();
		}
	}
};

struct SAI {
	static void enable(SAI_TypeDef *SAIx) {
		if (SAIx == nullptr)
			return;
		else if (SAIx == SAI1)
			RCC_Enable::SAI1_::set();
		else if (SAIx == SAI2)
			RCC_Enable::SAI2_::set();
		else if (SAIx == SAI3)
			RCC_Enable::SAI3_::set();
		else if (SAIx == SAI4)
			RCC_Enable::SAI4_::set();
	}
	static void disable(SAI_TypeDef *SAIx) {
		if (SAIx == nullptr)
			return;
		else if (SAIx == SAI1)
			RCC_Enable::SAI1_::clear();
		else if (SAIx == SAI2)
			RCC_Enable::SAI2_::clear();
		else if (SAIx == SAI3)
			RCC_Enable::SAI3_::clear();
		else if (SAIx == SAI4)
			RCC_Enable::SAI4_::clear();
	}
	static void reset(SAI_TypeDef *SAIx) {
		if (SAIx == nullptr) {
			return;
		} else if (SAIx == SAI1) {
			RCC_Reset::SAI1_::set();
			RCC_Reset::SAI1_::clear();
		} else if (SAIx == SAI2) {
			RCC_Reset::SAI2_::set();
			RCC_Reset::SAI2_::clear();
		} else if (SAIx == SAI3) {
			RCC_Reset::SAI3_::set();
			RCC_Reset::SAI3_::clear();
		} else if (SAIx == SAI4) {
			RCC_Reset::SAI4_::set();
			RCC_Reset::SAI4_::clear();
		}
	}
};

struct TIM {
	static void enable(unsigned periph_num) {
		if (periph_num < 1 || periph_num > PeriphUtil::TIM::NumPeriph)
			return;
		else if (periph_num == 1)
			RCC_Enable::TIM1_::set();
		else if (periph_num == 2)
			RCC_Enable::TIM2_::set();
		else if (periph_num == 3)
			RCC_Enable::TIM3_::set();
		else if (periph_num == 4)
			RCC_Enable::TIM4_::set();
		else if (periph_num == 5)
			RCC_Enable::TIM5_::set();
		else if (periph_num == 6)
			RCC_Enable::TIM6_::set();
		else if (periph_num == 7)
			RCC_Enable::TIM7_::set();
		else if (periph_num == 8)
			RCC_Enable::TIM8_::set();
		else if (periph_num == 9)
			RCC_Enable::TIM9_::set();
		else if (periph_num == 10)
			RCC_Enable::TIM10_::set();
		else if (periph_num == 11)
			RCC_Enable::TIM11_::set();
		else if (periph_num == 12)
			RCC_Enable::TIM12_::set();
		else if (periph_num == 13)
			RCC_Enable::TIM13_::set();
		else if (periph_num == 14)
			RCC_Enable::TIM14_::set();
		else if (periph_num == 15)
			RCC_Enable::TIM15_::set();
		else if (periph_num == 16)
			RCC_Enable::TIM16_::set();
		else if (periph_num == 17)
			RCC_Enable::TIM17_::set();
	}
	static void disable(unsigned periph_num) {
		if (periph_num < 1 || periph_num > PeriphUtil::TIM::NumPeriph)
			return;
		else if (periph_num == 1)
			RCC_Enable::TIM1_::clear();
		else if (periph_num == 2)
			RCC_Enable::TIM2_::clear();
		else if (periph_num == 3)
			RCC_Enable::TIM3_::clear();
		else if (periph_num == 4)
			RCC_Enable::TIM4_::clear();
		else if (periph_num == 5)
			RCC_Enable::TIM5_::clear();
		else if (periph_num == 6)
			RCC_Enable::TIM6_::clear();
		else if (periph_num == 7)
			RCC_Enable::TIM7_::clear();
		else if (periph_num == 8)
			RCC_Enable::TIM8_::clear();
		else if (periph_num == 9)
			RCC_Enable::TIM9_::clear();
		else if (periph_num == 10)
			RCC_Enable::TIM10_::clear();
		else if (periph_num == 11)
			RCC_Enable::TIM11_::clear();
		else if (periph_num == 12)
			RCC_Enable::TIM12_::clear();
		else if (periph_num == 13)
			RCC_Enable::TIM13_::clear();
		else if (periph_num == 14)
			RCC_Enable::TIM14_::clear();
		else if (periph_num == 15)
			RCC_Enable::TIM15_::clear();
		else if (periph_num == 16)
			RCC_Enable::TIM16_::clear();
		else if (periph_num == 17)
			RCC_Enable::TIM17_::clear();
	}
	static void enable(TIM_TypeDef *TIMx) {
		enable(PeriphUtil::TIM::to_num(TIMx));
	}
	static void disable(TIM_TypeDef *TIMx) {
		disable(PeriphUtil::TIM::to_num(TIMx));
	}
};
} // namespace mdrivlib::Clocks
