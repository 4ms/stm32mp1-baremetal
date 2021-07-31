#include "sai_tdm.hh"
#include "clocks.hh"
#include "interrupt.hh"
#include "interrupt_control.hh"
#include "stm32xx.h"
#include "system.hh"

namespace mdrivlib
{

SaiTdmPeriph::Error SaiTdmPeriph::init() {
	_init_pins();

	Clocks::SAI::reset(saidef_.sai);
	Clocks::SAI::enable(saidef_.sai);

	Clocks::DMA::enable(saidef_.dma_init_rx.DMAx);
	Clocks::DMA::enable(saidef_.dma_init_tx.DMAx);
	Clocks::DMAMUX::enable();

	Error err = SAI_NO_ERR;

	// Todo: swap order: always init slave first
	err = _config_tx_sai();
	if (err != SAI_NO_ERR)
		return err;

	err = _config_rx_sai();
	if (err != SAI_NO_ERR)
		return err;

	// Todo: swap order: always init slave first
	_config_tx_dma();
	_config_rx_dma();
	err = _init_sai_dma();
	if (err != SAI_NO_ERR)
		return err;

	_sai_enable(&hsai_rx);
	_sai_enable(&hsai_tx);

	tx_irqn = saidef_.dma_init_tx.IRQn;
	InterruptControl::set_irq_priority(tx_irqn, saidef_.dma_init_tx.pri, saidef_.dma_init_tx.subpri);
	InterruptControl::disable_irq(tx_irqn);

	rx_irqn = saidef_.dma_init_rx.IRQn;
	InterruptControl::set_irq_priority(rx_irqn, saidef_.dma_init_rx.pri, saidef_.dma_init_rx.subpri);
	InterruptControl::disable_irq(rx_irqn);

	return SAI_NO_ERR;
}

SaiTdmPeriph::Error SaiTdmPeriph::_config_rx_sai() {
	Error err = SAI_NO_ERR;

	__HAL_SAI_RESET_HANDLE_STATE(&hsai_rx);
	hsai_rx.Instance = saidef_.rx_block;

	_sai_disable(&hsai_rx);

	if (saidef_.mode == SaiConfig::RXMaster) {
		hsai_rx.Init.AudioMode = SAI_MODEMASTER_RX;
		hsai_rx.Init.Synchro = SAI_ASYNCHRONOUS;
		hsai_rx.Init.MckOutput = SAI_MCK_OUTPUT_ENABLE;
		hsai_rx.Init.SynchroExt = saidef_.sync_send == SaiConfig::BlockASendsSync ? SAI_SYNCEXT_OUTBLOCKA_ENABLE
								: saidef_.sync_send == SaiConfig::BlockBSendsSync ? SAI_SYNCEXT_OUTBLOCKB_ENABLE
																				  : SAI_SYNCEXT_DISABLE;
	}
	if (saidef_.mode == SaiConfig::TXMaster) {
		hsai_rx.Init.AudioMode = SAI_MODESLAVE_RX;
		hsai_rx.Init.Synchro = SAI_SYNCHRONOUS;
		hsai_rx.Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;
		hsai_rx.Init.SynchroExt = saidef_.sync_send == SaiConfig::BlockASendsSync ? SAI_SYNCEXT_OUTBLOCKA_ENABLE
								: saidef_.sync_send == SaiConfig::BlockBSendsSync ? SAI_SYNCEXT_OUTBLOCKB_ENABLE
																				  : SAI_SYNCEXT_DISABLE;
	}
	if (saidef_.mode == SaiConfig::ExtSynced) {
		hsai_rx.Init.AudioMode = SAI_MODESLAVE_RX;
		hsai_rx.Init.Synchro = saidef_.sync_receive_from == SaiConfig::SyncToSAI1 ? SAI_SYNCHRONOUS_EXT_SAI1
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI2 ? SAI_SYNCHRONOUS_EXT_SAI2
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI3 ? SAI_SYNCHRONOUS_EXT_SAI3
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI4 ? SAI_SYNCHRONOUS_EXT_SAI4
																				  : SAI_SYNCHRONOUS;

		hsai_rx.Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;
		hsai_rx.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	}

	hsai_rx.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	hsai_rx.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
	hsai_rx.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
	hsai_rx.Init.AudioFrequency = saidef_.samplerate;
	hsai_rx.Init.MonoStereoMode = SAI_STEREOMODE;
	hsai_rx.Init.CompandingMode = SAI_NOCOMPANDING;
	hsai_rx.Init.TriState = SAI_OUTPUT_NOTRELEASED;
	hsai_rx.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
	hsai_rx.Init.PdmInit.Activation = DISABLE;

	if (saidef_.num_tdm_ins > 2) {
		// Todo: use conf to set slot size
		// Todo: allow conf to set FS pulse mode (ActiveFrameLength)
		hsai_rx.Init.Protocol = SAI_FREE_PROTOCOL;
		hsai_rx.Init.DataSize = saidef_.datasize;
		hsai_rx.Init.FirstBit = SAI_FIRSTBIT_MSB;
		hsai_rx.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
		hsai_rx.FrameInit.FrameLength = saidef_.framesize;
		hsai_rx.FrameInit.ActiveFrameLength = 1; // FS pulses at start of frame
		hsai_rx.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
		hsai_rx.FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
		hsai_rx.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;
		hsai_rx.SlotInit.FirstBitOffset = 0;
		hsai_rx.SlotInit.SlotSize = SAI_SLOTSIZE_32B;
		hsai_rx.SlotInit.SlotNumber = saidef_.num_tdm_ins;
		hsai_rx.SlotInit.SlotActive = 0x0000FFFF;
		HAL_SAI_DeInit(&hsai_rx);
		if (HAL_SAI_Init(&hsai_rx) != HAL_OK)
			err = SAI_INIT_ERR;
	} else {
		HAL_SAI_DeInit(&hsai_rx);
		if (HAL_SAI_InitProtocol(&hsai_rx, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
			err = SAI_INIT_ERR;
	}

	return err;
}

SaiTdmPeriph::Error SaiTdmPeriph::_config_tx_sai() {
	Error err = SAI_NO_ERR;

	__HAL_SAI_RESET_HANDLE_STATE(&hsai_tx);
	hsai_tx.Instance = saidef_.tx_block;
	_sai_disable(&hsai_tx);

	if (saidef_.mode == SaiConfig::RXMaster) {
		hsai_tx.Init.AudioMode = SAI_MODESLAVE_TX;
		hsai_tx.Init.Synchro = SAI_SYNCHRONOUS;
		hsai_tx.Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;
		hsai_tx.Init.SynchroExt = saidef_.sync_send == SaiConfig::BlockASendsSync ? SAI_SYNCEXT_OUTBLOCKA_ENABLE
								: saidef_.sync_send == SaiConfig::BlockBSendsSync ? SAI_SYNCEXT_OUTBLOCKB_ENABLE
																				  : SAI_SYNCEXT_DISABLE;
	}
	if (saidef_.mode == SaiConfig::TXMaster) {
		hsai_tx.Init.AudioMode = SAI_MODEMASTER_TX;
		hsai_tx.Init.Synchro = SAI_ASYNCHRONOUS;
		hsai_tx.Init.MckOutput = SAI_MCK_OUTPUT_ENABLE;
		hsai_tx.Init.SynchroExt = saidef_.sync_send == SaiConfig::BlockASendsSync ? SAI_SYNCEXT_OUTBLOCKA_ENABLE
								: saidef_.sync_send == SaiConfig::BlockBSendsSync ? SAI_SYNCEXT_OUTBLOCKB_ENABLE
																				  : SAI_SYNCEXT_DISABLE;
	}
	if (saidef_.mode == SaiConfig::ExtSynced) {
		hsai_tx.Init.AudioMode = SAI_MODESLAVE_TX;
		hsai_tx.Init.Synchro = saidef_.sync_receive_from == SaiConfig::SyncToSAI1 ? SAI_SYNCHRONOUS_EXT_SAI1
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI2 ? SAI_SYNCHRONOUS_EXT_SAI2
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI3 ? SAI_SYNCHRONOUS_EXT_SAI3
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI4 ? SAI_SYNCHRONOUS_EXT_SAI4
																				  : SAI_SYNCHRONOUS;

		hsai_tx.Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;
		hsai_tx.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	}

	hsai_tx.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	hsai_tx.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
	hsai_tx.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
	hsai_tx.Init.AudioFrequency = saidef_.samplerate;
	hsai_tx.Init.MonoStereoMode = SAI_STEREOMODE;
	hsai_tx.Init.CompandingMode = SAI_NOCOMPANDING;
	hsai_tx.Init.TriState = SAI_OUTPUT_NOTRELEASED;
	hsai_tx.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
	hsai_tx.Init.PdmInit.Activation = DISABLE;

	if (saidef_.num_tdm_outs > 2) {
		hsai_tx.Init.Protocol = SAI_FREE_PROTOCOL;
		hsai_tx.Init.DataSize = saidef_.datasize;
		hsai_tx.Init.FirstBit = SAI_FIRSTBIT_MSB;
		hsai_tx.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
		hsai_tx.FrameInit.FrameLength = saidef_.framesize;
		hsai_tx.FrameInit.ActiveFrameLength = 1; // FS pulses at start of frame
		hsai_tx.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
		hsai_tx.FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
		hsai_tx.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;
		hsai_tx.SlotInit.FirstBitOffset = 0;
		hsai_tx.SlotInit.SlotSize = SAI_SLOTSIZE_32B;
		hsai_tx.SlotInit.SlotNumber = saidef_.num_tdm_outs;
		hsai_tx.SlotInit.SlotActive = 0x0000FFFF;
		HAL_SAI_DeInit(&hsai_tx);
		if (HAL_SAI_Init(&hsai_tx) != HAL_OK)
			err = SAI_INIT_ERR;
	} else {
		HAL_SAI_DeInit(&hsai_tx);
		if (HAL_SAI_InitProtocol(&hsai_tx, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
			err = SAI_INIT_ERR;
	}

	return err;
}

void SaiTdmPeriph::_config_rx_dma() {
	hdma_rx.Init.Request = saidef_.dma_init_rx.channel;

	hdma_rx.Instance = saidef_.dma_init_rx.stream;
	hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_rx.Init.Mode = DMA_CIRCULAR;
	hdma_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
	hdma_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hdma_rx.Init.MemBurst = DMA_MBURST_SINGLE;
	hdma_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;

	__HAL_LINKDMA(&hsai_rx, hdmarx, hdma_rx);
	HAL_DMA_DeInit(&hdma_rx);
}

void SaiTdmPeriph::_config_tx_dma() {
	hdma_tx.Init.Request = saidef_.dma_init_tx.channel;

	hdma_tx.Instance = saidef_.dma_init_tx.stream;
	hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_tx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_tx.Init.Mode = DMA_CIRCULAR;
	hdma_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
	hdma_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hdma_tx.Init.MemBurst = DMA_MBURST_SINGLE;
	hdma_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;

	__HAL_LINKDMA(&hsai_tx, hdmatx, hdma_tx);
	HAL_DMA_DeInit(&hdma_tx);
}

SaiTdmPeriph::Error SaiTdmPeriph::_init_sai_dma() {
	// Todo always init slave first
	if (HAL_DMA_Init(&hdma_tx) != HAL_OK)
		return SAI_INIT_ERR;

	if (HAL_DMA_Init(&hdma_rx) != HAL_OK)
		return SAI_INIT_ERR;

	return SAI_NO_ERR;
}

void SaiTdmPeriph::_init_pins() {
	if (saidef_.mode == SaiConfig::TXMaster || saidef_.mode == SaiConfig::RXMaster) {
		Pin sai_mclk{saidef_.MCLK.gpio,
					 saidef_.MCLK.pin,
					 PinMode::Alt,
					 saidef_.MCLK.af,
					 PinPull::None,
					 PinPolarity::Normal,
					 PinSpeed::High,
					 PinOType::PushPull};
		Pin sai_sclk{saidef_.SCLK.gpio,
					 saidef_.SCLK.pin,
					 PinMode::Alt,
					 saidef_.SCLK.af,
					 PinPull::None,
					 PinPolarity::Normal,
					 PinSpeed::High,
					 PinOType::PushPull};
		Pin sai_lrclk{saidef_.LRCLK.gpio,
					  saidef_.LRCLK.pin,
					  PinMode::Alt,
					  saidef_.LRCLK.af,
					  PinPull::None,
					  PinPolarity::Normal,
					  PinSpeed::High,
					  PinOType::PushPull};
	}
	Pin sai_mrx_adc{saidef_.SD_ADC.gpio,
					saidef_.SD_ADC.pin,
					PinMode::Alt,
					saidef_.SD_ADC.af,
					PinPull::None,
					PinPolarity::Normal,
					PinSpeed::High,
					PinOType::PushPull};
	Pin sai_mtx_dac{saidef_.SD_DAC.gpio,
					saidef_.SD_DAC.pin,
					PinMode::Alt,
					saidef_.SD_DAC.af,
					PinPull::None,
					PinPolarity::Normal,
					PinSpeed::High,
					PinOType::PushPull};
}

void SaiTdmPeriph::set_tx_buffers(uint8_t *tx_buf_ptr, uint32_t block_size) {
	tx_buf_ptr_ = tx_buf_ptr;
	tx_block_size_ = block_size;
}

void SaiTdmPeriph::set_rx_buffers(uint8_t *rx_buf_ptr, uint32_t block_size) {
	rx_buf_ptr_ = rx_buf_ptr;
	rx_block_size_ = block_size;
}

void SaiTdmPeriph::set_callbacks(std::function<void()> &&tx_complete_cb, std::function<void()> &&tx_half_complete_cb) {
	tx_tc_cb = std::move(tx_complete_cb);
	tx_ht_cb = std::move(tx_half_complete_cb);
}

void SaiTdmPeriph::start() {
	if (saidef_.mode == SaiConfig::RXMaster) {
		dma_tc_flag_index = dma_get_TC_flag_index(hdma_rx.Instance);
		dma_ht_flag_index = dma_get_HT_flag_index(hdma_rx.Instance);
		dma_te_flag_index = dma_get_TE_flag_index(hdma_rx.Instance);
		dma_isr_reg = dma_get_ISR_reg(saidef_.dma_init_rx.stream);
		dma_ifcr_reg = dma_get_IFCR_reg(saidef_.dma_init_rx.stream);
		_start_irq(rx_irqn);
	}
	if (saidef_.mode == SaiConfig::TXMaster) {
		dma_tc_flag_index = dma_get_TC_flag_index(hdma_tx.Instance);
		dma_ht_flag_index = dma_get_HT_flag_index(hdma_tx.Instance);
		dma_te_flag_index = dma_get_TE_flag_index(hdma_tx.Instance);
		dma_isr_reg = dma_get_ISR_reg(saidef_.dma_init_tx.stream);
		dma_ifcr_reg = dma_get_IFCR_reg(saidef_.dma_init_tx.stream);
		_start_irq(tx_irqn);
	}

	HAL_SAI_Transmit_DMA(&hsai_tx, tx_buf_ptr_, tx_block_size_);
	HAL_SAI_Receive_DMA(&hsai_rx, rx_buf_ptr_, rx_block_size_);
}

void SaiTdmPeriph::_start_irq(IRQn_Type irqn) {
	InterruptManager::registerISR(irqn, [this]() {
		if ((*dma_isr_reg & dma_tc_flag_index) /*&& (saidef_.dma_init_tx.stream->CR & DMA_IT_TC)*/) {
			*dma_ifcr_reg = dma_tc_flag_index;
			tx_tc_cb();
		}

		if ((*dma_isr_reg & dma_ht_flag_index) /*&& (saidef_.dma_init_tx.stream->CR & DMA_IT_HT)*/) {
			*dma_ifcr_reg = dma_ht_flag_index;
			tx_ht_cb();
		}

		if ((*dma_isr_reg & dma_te_flag_index) /*&& (saidef_.dma_init_tx.stream->CR & DMA_IT_HT)*/) {
			*dma_ifcr_reg = dma_te_flag_index;
			// Error: debug breakpoint or logging here
		}
	});
	InterruptControl::enable_irq(irqn);
}

void SaiTdmPeriph::stop() {
	InterruptControl::disable_irq(tx_irqn);
	InterruptControl::disable_irq(rx_irqn);
}

void SaiTdmPeriph::_sai_enable(SAI_HandleTypeDef *hsai) {
	hsai->Instance->CR1 = hsai->Instance->CR1 | (0x1UL << 16U);
}

void SaiTdmPeriph::_sai_disable(SAI_HandleTypeDef *hsai) {
	hsai->Instance->CR1 = hsai->Instance->CR1 & ~(0x1UL << 16U);
}

} // namespace mdrivlib
