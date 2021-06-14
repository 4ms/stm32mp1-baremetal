/*
 * adc_builtin_driver.h - adc driver for built-in adcs
 *
 * Author: Dan Green (danngreen1@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * See http://creativecommons.org/licenses/MIT/ for more information.
 *
 * -----------------------------------------------------------------------------
 */

#pragma once

#include "clocks.hh"
#include "dma_config_struct.hh"
#include "pin.hh"
#include "stm32xx.h"
#include <array>

namespace mdrivlib
{
enum class AdcChanNum { _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15 };
enum class AdcPeriphNum { _1, _2, _3 };

template<AdcPeriphNum ADCN>
class AdcPeriph;

#ifdef STM32F7
const uint32_t ADC_DEFAULT_SAMPLINGTIME = LL_ADC_SAMPLINGTIME_144CYCLES;
#elif defined(STM32H7)
const uint32_t ADC_DEFAULT_SAMPLINGTIME = LL_ADC_SAMPLINGTIME_64CYCLES_5;
#endif

template<AdcPeriphNum ADCN, AdcChanNum c, typename T = uint16_t>
class AdcChan {
public:
	AdcChan(const uint32_t sampletime /* = ADC_DEFAULT_SAMPLINGTIME*/) {
		auto init_adc_once = AdcPeriph<ADCN>::AdcInstance();
		AdcPeriph<ADCN>::add_channel(c, sampletime);
	}

	static T *const get_val_ptr() {
		return AdcPeriph<ADCN>::get_val_ptr(c);
	}
	static const T &get_val_ref() {
		return AdcPeriph<ADCN>::get_val_ref(c);
	}

	T get_val() {
		return AdcPeriph<ADCN>::get_val(c);
	}

	// void start_dma(const DMA_LL_Config dma_defs)
	// {
	// 	AdcPeriph<ADCN>::init_dma(dma_defs);
	// 	AdcPeriph<ADCN>::start_adc();
	// }
};

template<AdcPeriphNum ADCN>
class AdcPeriph {
	template<AdcPeriphNum p, AdcChanNum c, typename T>
	friend class AdcChan;

public:
	static void init_dma(const DMA_LL_Config &dma_defs, uint16_t *dma_buffer = default_dma_buffer_);
	static void enable_DMA_IT();
	static void start_adc();

	static uint16_t get_val(const AdcChanNum channel) {
		return dma_buffer_[ranks_[static_cast<uint8_t>(channel)]];
	}

	static uint16_t *const get_val_ptr(const AdcChanNum channel) {
		return &(dma_buffer_[ranks_[static_cast<uint8_t>(channel)]]);
	}

	static const uint16_t &get_val_ref(const AdcChanNum channel) {
		return dma_buffer_[ranks_[static_cast<uint8_t>(channel)]];
	}

private:
	static void add_channel(const AdcChanNum channel, const uint32_t sampletime);

	AdcPeriph();
	static AdcPeriph<ADCN> &AdcInstance();

	static inline uint8_t num_channels_;
	static inline uint8_t ranks_[16];
	static inline uint16_t *dma_buffer_;
	static inline uint16_t default_dma_buffer_[16];

	static inline IRQn_Type DMA_IRQn;
	static inline uint8_t DMA_IRQ_pri;
	static inline uint8_t DMA_IRQ_subpri;

	static ADC_TypeDef *get_ADC_base(AdcPeriphNum p = ADCN) {
		return (p == AdcPeriphNum::_1) ? ADC1
			 : (p == AdcPeriphNum::_2) ? ADC2
			 : (p == AdcPeriphNum::_3) ? ADC3
									   : nullptr;
	}
};
} // namespace mdrivlib
