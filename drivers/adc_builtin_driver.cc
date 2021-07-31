/*
 * adc_builtin_driver.c - adc driver for built-in adcs
 * Uses DMA to dump ADC values into buffers
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
#include "adc_builtin_driver.hh"
#include "arch.hh"
#include "interrupt_control.hh"
#include "stm32xx.h"

namespace mdrivlib
{
template class AdcPeriph<AdcPeriphNum::_1>;
template class AdcPeriph<AdcPeriphNum::_2>;
template class AdcPeriph<AdcPeriphNum::_3>;

template<AdcPeriphNum p>
AdcPeriph<p> &AdcPeriph<p>::AdcInstance() {
	static AdcPeriph<p> Adc_;
	return Adc_;
}

// Todo: Make this work on multiple platforms/archs
// Maybe have an arch-defined config struct, basically merge the LL_ADC structs for each arch
// use #ifdefs in the _config_struct.hh file to select which struct type is active
// Also use #ifdefs here to construct
//
// Or, avoid all preprocessor stuff, and don't use LL_ADC.
// Instead create our own register structs, mirroring the ones in stm32DEVICE.h file,
// but namespace them.
// namespace STM32H745xx {
// struct ADC1Registers_T {
//    volatile uint32_t DR;
//     ...
// };
// uint32_t ADC1Registers_T * const ADC1Registers {0x40001000;}
//
// struct ADCPeriph {
//  ...do stuff with ADC1Registers (which is STM32H745xx::ADC1Registers)
// }
// Then in our client code we need to use STM32H745xx::ADCPeriph<ADCP::_1> myperiph { my_conf };
//
// Another idea: how to allow user to pass conf for adc peripheral (clock settings, etc)?
// AdcChan freqCV {AdcChanNum::_1, Pin{GPIO::A, 2, PinMode::Analog}, optional_conf{.samplingtime=...}};
// AdcChan resCV {...};
// AdcPeriph<ADCPeriphNum::_1> {periph_conf, {freqCV, resCV, ...}};
// But is this clear that we need to construct the channels first, then pass them to the peripheral?
//
// Maybe ditch this approach and just treat the ADC Peripheral as the gateway
// const ADCChannelDef freqCV = {.channum=1, .sampletime=144, pin=2, .port=GPIO::A};
// AdcPeriph<AdcPNum::_1> adc {my_conf, {freqCV, resCV}...};
// uint16_t AdcPeriph<p>::read_channel(ADCCHannelDef chan) { return read_channel(chan.channum); }
// uint16_t AdcPeriph<p>::read_channel(unsigned channum) { return dma_buffer[channum]; }
//
// clent code says: adc.read_channel(freqCV); or adc.read_channel(1)
//

template<AdcPeriphNum p>
AdcPeriph<p>::AdcPeriph() {
	Clocks::ADC::enable(get_ADC_base(p));

	dma_buffer_ = default_dma_buffer_;

	num_channels_ = 0;
	LL_ADC_Disable(get_ADC_base(p));

	NVIC_DisableIRQ(ADC_IRQn);

#if defined(ADC123_COMMON)
	LL_ADC_SetCommonClock(ADC123_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#elif defined(ADC12_COMMON)
	if constexpr (p == AdcPeriphNum::_1 || p == AdcPeriphNum::_2)
		LL_ADC_SetCommonClock(ADC12_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#if defined(ADC3_COMMON)
	if constexpr (p == AdcPeriphNum::_3)
		LL_ADC_SetCommonClock(ADC3_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#endif
#elif defined(ADC1_COMMON)
	if constexpr (p == AdcPeriphNum::_1)
		LL_ADC_SetCommonClock(ADC1_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#elif defined(ADC2_COMMON)
	if constexpr (p == AdcPeriphNum::_2)
		LL_ADC_SetCommonClock(ADC2_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#elif defined(ADC3_COMMON)
	if constexpr (p == AdcPeriphNum::_3)
		LL_ADC_SetCommonClock(ADC3_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#endif

	LL_ADC_SetResolution(get_ADC_base(p), LL_ADC_RESOLUTION_12B);

#if !defined(STM32H755xx) && !defined(STM32H745xx)
	LL_ADC_SetDataAlignment(get_ADC_base(p), LL_ADC_DATA_ALIGN_RIGHT);
	LL_ADC_SetSequencersScanMode(get_ADC_base(p), LL_ADC_SEQ_SCAN_ENABLE);
#endif

#if defined(STM32F7)
	LL_ADC_REG_InitTypeDef adcreginit;

	adcreginit.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	adcreginit.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
	adcreginit.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	adcreginit.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
	adcreginit.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
	LL_ADC_REG_Init(get_ADC_base(p), &adcreginit);
	LL_ADC_REG_SetFlagEndOfConversion(get_ADC_base(p), LL_ADC_REG_FLAG_EOC_SEQUENCE_CONV);
#endif
}

constexpr uint32_t _LL_ADC_DECIMAL_NB_TO_RANK(const uint8_t x) {
	return ((x <= 5)	? (ADC_SQR3_REGOFFSET | (x * 5))
			: (x <= 11) ? (ADC_SQR2_REGOFFSET | ((x - 6) * 5))
			: (x <= 15) ? (ADC_SQR1_REGOFFSET | ((x - 12) * 5))
						: 0);
}

constexpr uint32_t _LL_ADC_DECIMAL_NB_TO_REG_SEQ_LENGTH(const uint8_t x) {
	return (x << ADC_SQR1_L_Pos);
}

template<AdcPeriphNum p>
void AdcPeriph<p>::add_channel(const AdcChanNum channel, const uint32_t sampletime) {
	uint32_t channel_int = static_cast<uint32_t>(channel);
	LL_ADC_REG_SetSequencerRanks(
		get_ADC_base(p), _LL_ADC_DECIMAL_NB_TO_RANK(num_channels_), __LL_ADC_DECIMAL_NB_TO_CHANNEL(channel_int));
	LL_ADC_REG_SetSequencerLength(get_ADC_base(p), _LL_ADC_DECIMAL_NB_TO_REG_SEQ_LENGTH(num_channels_));
	LL_ADC_SetChannelSamplingTime(get_ADC_base(p), __LL_ADC_DECIMAL_NB_TO_CHANNEL(channel_int), sampletime);
	uint8_t rank_decimal = num_channels_;
	num_channels_++;
	ranks_[channel_int] = rank_decimal;
}

template<AdcPeriphNum p>
void AdcPeriph<p>::init_dma(const DMA_LL_Config &dma_defs, uint16_t *dma_buffer) {
	dma_buffer_ = dma_buffer;

	if (!num_channels_)
		return;

	Clocks::DMA::enable(dma_defs.DMAx);

#if defined(STM32F7)
	LL_DMA_SetChannelSelection(dma_defs.DMAx, dma_defs.stream, dma_defs.channel);
#endif
	LL_DMA_ConfigTransfer(dma_defs.DMAx,
						  dma_defs.stream,
						  LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
							  LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_HALFWORD | LL_DMA_MDATAALIGN_HALFWORD |
							  LL_DMA_PRIORITY_HIGH);
	LL_DMA_ConfigAddresses(dma_defs.DMAx,
						   dma_defs.stream,
						   LL_ADC_DMA_GetRegAddr(get_ADC_base(p), LL_ADC_DMA_REG_REGULAR_DATA),
						   (uint32_t)(dma_buffer_),
						   LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetDataLength(dma_defs.DMAx, dma_defs.stream, num_channels_);
	LL_DMA_EnableIT_TC(dma_defs.DMAx, dma_defs.stream);
	LL_DMA_DisableIT_HT(dma_defs.DMAx, dma_defs.stream);
	LL_DMA_EnableIT_TE(dma_defs.DMAx, dma_defs.stream);
	LL_DMA_EnableStream(dma_defs.DMAx, dma_defs.stream);

	DMA_IRQn = dma_defs.IRQn;
	DMA_IRQ_pri = dma_defs.pri;
	DMA_IRQ_subpri = dma_defs.subpri;
}

template<AdcPeriphNum p>
void AdcPeriph<p>::enable_DMA_IT() {
	InterruptControl::set_irq_priority(DMA_IRQn, DMA_IRQ_pri, DMA_IRQ_subpri);
	InterruptControl::enable_irq(DMA_IRQn);
}

template<AdcPeriphNum p>
void AdcPeriph<p>::start_adc() {
	LL_ADC_Enable(get_ADC_base(p));

#if defined(STM32F7)
	LL_ADC_REG_StartConversionSWStart(get_ADC_base(p));
#elif defined(STM32H755xx)
	LL_ADC_REG_StartConversion(get_ADC_base(p));
#endif
}
} // namespace mdrivlib
