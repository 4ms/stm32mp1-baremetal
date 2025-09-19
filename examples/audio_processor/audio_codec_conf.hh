#include "drivers/i2c_config_struct.hh"
#include "drivers/sai_config_struct.hh"

using namespace mdrivlib;

using mdrivlib::I2CConfig;

const I2CConfig i2c_conf = {
	.I2Cx = I2C6,
	.SCL = {GPIO::A, PinNum::_11, PinAF::AltFunc2},
	.SDA = {GPIO::A, PinNum::_12, PinAF::AltFunc2},
	.timing =
		{
			.PRESC = 0x40,
			.SCLDEL_SDADEL = 0xFF,
			.SCLH = 0x90,
			.SCLL = 0x90,
		},
	.priority1 = 2,
	.priority2 = 1,
};

using mdrivlib::SaiConfig;

const SaiConfig sai_conf = {
	.sai = SAI2,
	.tx_block = SAI2_Block_A,
	.rx_block = SAI2_Block_B,

	.mode = SaiConfig::TXMaster,

	.dma_init_tx =
		{
			.DMAx = DMA2,
			.stream = DMA2_Stream1,
			.channel = DMA_REQUEST_SAI2_A,
			.IRQn = DMA2_Stream1_IRQn,
			.pri = 1,
			.subpri = 1,
		},
	.dma_init_rx =
		{
			.DMAx = DMA2,
			.stream = DMA2_Stream2,
			.channel = DMA_REQUEST_SAI2_B,
			.IRQn = DMA2_Stream2_IRQn,
			.pri = 1,
			.subpri = 1,
		},

	.datasize = SAI_DATASIZE_24,
	.framesize = 32,
	.samplerate = 48000,

	.MCLK = {GPIO::E, PinNum::_0, PinAF::AltFunc10},
	.SCLK = {GPIO::D, PinNum::_13, PinAF::AltFunc10},
	.LRCLK = {GPIO::D, PinNum::_12, PinAF::AltFunc10},
	.SD_DAC = {GPIO::D, PinNum::_11, PinAF::AltFunc10}, // SD A
	.SD_ADC = {GPIO::G, PinNum::_10, PinAF::AltFunc10}, // SD B

	.reset_pin = {GPIO::A, PinNum::_10},

	.bus_address = 0b00,

	.num_tdm_ins = 2,
	.num_tdm_outs = 2,

	.sync_send = SaiConfig::NoSendSync,
	.sync_receive_from = SaiConfig::NoReceiveSync,
};
