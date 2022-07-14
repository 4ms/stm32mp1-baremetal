#include "drv/i2c_config_struct.hh"
#include "drv/sai_config_struct.hh"

using mdrivlib::I2CConfig;

const I2CConfig i2c_conf = {
	.I2Cx = I2C1,
	.SCL = {mdrivlib::GPIO::D, 12, LL_GPIO_AF_5},
	.SDA = {mdrivlib::GPIO::F, 15, LL_GPIO_AF_5},
	.timing =
		{
			.PRESC = 0x20,
			.SCLDEL_SDADEL = 0x90,
			.SCLH = 0x19,
			.SCLL = 0x45,
		},
	.priority1 = 0,
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
			.DMAx = DMA1,
			.stream = DMA1_Stream0,
			.channel = DMA_REQUEST_SAI2_A,
			.IRQn = DMA1_Stream0_IRQn,
			.pri = 1,
			.subpri = 1,
		},
	.dma_init_rx =
		{
			.DMAx = DMA1,
			.stream = DMA1_Stream1,
			.channel = DMA_REQUEST_SAI2_B,
			.IRQn = DMA1_Stream1_IRQn,
			.pri = 1,
			.subpri = 1,
		},

	.datasize = SAI_DATASIZE_24,
	.framesize = 32,
	.samplerate = 48000,

	.MCLK = {mdrivlib::GPIO::E, 0, LL_GPIO_AF_10},
	.SCLK = {mdrivlib::GPIO::I, 5, LL_GPIO_AF_10},
	.LRCLK = {mdrivlib::GPIO::I, 7, LL_GPIO_AF_10},
	.SD_DAC = {mdrivlib::GPIO::I, 6, LL_GPIO_AF_10},  // PI6 = SAI2_SD_A
	.SD_ADC = {mdrivlib::GPIO::F, 11, LL_GPIO_AF_10}, // PF11 = SAI2_SD_B
	.reset_pin = {mdrivlib::GPIO::G, 9},

	.bus_address = 0,

	.num_tdm_ins = 2,
	.num_tdm_outs = 2,

	.sync_send = SaiConfig::NoSendSync,
	.sync_receive_from = SaiConfig::NoReceiveSync,
};
