#pragma once
#include "dma_config_struct.hh"
#include "pin.hh"
#include "stm32xx.h"

namespace mdrivlib
{
struct SaiConfig {
	SAI_TypeDef *sai;
	SAI_Block_TypeDef *tx_block;
	SAI_Block_TypeDef *rx_block;

	enum SAIRxTxMode { RXMaster, TXMaster, ExtSynced } mode;

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
	PinNoInit reset_pin;

	uint32_t bus_address;

	uint32_t num_tdm_ins;
	uint32_t num_tdm_outs;

	enum SyncSendMode { NoSendSync = 0b00, BlockASendsSync = 0b01, BlockBSendsSync = 0b11 } sync_send;

	enum SyncReceiveFrom {
		NoReceiveSync = -1,
		SyncToSAI1 = 0,
		SyncToSAI2 = 1,
		SyncToSAI3 = 2,
		SyncToSAI4 = 3
	} sync_receive_from;
};
} // namespace mdrivlib
