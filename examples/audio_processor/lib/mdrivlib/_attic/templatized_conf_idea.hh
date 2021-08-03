#pragma once
#include "drivers/dma_config_struct.hh"
#include "drivers/pin.hh"
#include "drivers/sai_config_struct.hh"
#include "drivers/stm32xx.h"
#include <variant>

// This works!
// But.. we'll need to templatize Codec (which contains a SaiPeriph)
// and maybe Audio, since it takes a Codec (but maybe not since it takes a generic ICodec&)

enum class SAIRxTxMode { RXMaster, TXMaster };

template<unsigned periph_num>
struct SaiConf {
	static inline const unsigned PeriphNum = periph_num;
	SAI_TypeDef *SAIx;
	SAI_Block_TypeDef *tx_block;
	SAI_Block_TypeDef *rx_block;

	SAIRxTxMode mode;
	DMA_Config dma_init_tx;
	DMA_Config dma_init_rx;

	uint32_t datasize;
	uint32_t framesize;
	uint32_t samplerate;

	PinNoInit MCLK;
	PinNoInit SCLK;
	PinNoInit LRCLK;
	PinNoInit SD_DAC;
	PinNoInit SD_ADC;
};

// In project conf file:
const SaiConf<1> project_sai_conf = {
	.SAIx = SAI1,
	.tx_block = SAI1_Block_B,
	.rx_block = SAI1_Block_A,

	.mode = SAIRxTxMode::RXMaster,

	.dma_init_tx =
		{
			.DMAx = DMA1,
			.stream = DMA1_Stream1,
			.channel = DMA_REQUEST_SAI1_B,
			.IRQn = DMA1_Stream1_IRQn,
			.pri = 1,
			.subpri = 0,
		},
	.dma_init_rx =
		{
			.DMAx = DMA1,
			.stream = DMA1_Stream0,
			.channel = DMA_REQUEST_SAI1_A,
			.IRQn = DMA1_Stream0_IRQn,
			.pri = 1,
			.subpri = 0,
		},

	.datasize = SAI_DATASIZE_24,
	.framesize = 32,
	.samplerate = 48000,

	.MCLK = {GPIO::E, 2, GPIO_AF6_SAI1},
	.SCLK = {GPIO::E, 5, GPIO_AF6_SAI1},
	.LRCLK = {GPIO::E, 4, GPIO_AF6_SAI1},
	.SD_DAC = {GPIO::E, 3, GPIO_AF6_SAI1},
	.SD_ADC = {GPIO::E, 6, GPIO_AF6_SAI1},
};

namespace mdrivlib
{

// rcc.hh: (it's already in rcc.hh, just leaving it there)
// namespace ___target__
// {
// namespace RCC_Controls
// {
// using SAI = std::variant<SAI_1, SAI_2, SAI_3, SAI_4>;
// }
// }

// In system.hh:
namespace Clocks
{
template<unsigned pn>
struct SAIs {
	static void enable() {
		std::variant_alternative_t<pn - 1, target::RCC_Enable::SAI>::set();
		// instead of if periph_num==1/else if/else if/...
	}
};
} // namespace Clocks
} // namespace mdrivlib

// In sai.hh:
template<typename ConfT>
class SaiPeripheral {
public:
	SaiPeripheral(const ConfT &defs)
		: defs_(defs) {
		mdrivlib::Clocks::SAIs<ConfT::PeriphNum>::enable();
	}

private:
	const ConfT &defs_;
};

// In main()

// SaiPeripheral sai{project_sai_conf};
// Template type deduction figures out it's SaiPeripheral<SaiConf<1>> since project_sai_conf is type SaiConf<1>
// and Clocks::SAIs<1>::enable() calls target::RCC_Control::SAI_1::set();
