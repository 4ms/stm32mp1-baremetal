#include "sai.hh"
#include "clocks.hh"
#include "interrupt.hh"
#include "stm32xx.h"
#include "system.hh"

namespace mdrivlib
{

SaiPeriph::Error SaiPeriph::init() {
	_init_pins();

	Clocks::SAI::reset(saidef_.sai);
	Clocks::SAI::enable(saidef_.sai);

	Clocks::DMA::enable(saidef_.dma_init_rx.DMAx);
	Clocks::DMA::enable(saidef_.dma_init_tx.DMAx);
	Clocks::DMAMUX::enable();

	{
		// Todo: swap order: always init slave first
		_config_tx_sai();
		_config_rx_sai();
		auto err = _init_sai_protocol();
		if (err != SAI_NO_ERR)
			return err;
	}
	{
		// Todo: swap order: always init slave first
		_config_tx_dma();
		_config_rx_dma();
		auto err = _init_sai_dma();
		if (err != SAI_NO_ERR)
			return err;
	}

	__HAL_SAI_ENABLE(&hsai_rx);
	__HAL_SAI_ENABLE(&hsai_tx);

	tx_irqn = saidef_.dma_init_tx.IRQn;
	InterruptControl::set_irq_priority(tx_irqn, saidef_.dma_init_tx.pri, saidef_.dma_init_tx.subpri);
	InterruptControl::disable_irq(tx_irqn);

	rx_irqn = saidef_.dma_init_rx.IRQn;
	InterruptControl::set_irq_priority(rx_irqn, saidef_.dma_init_rx.pri, saidef_.dma_init_rx.subpri);
	InterruptControl::disable_irq(rx_irqn);

	return SAI_NO_ERR;
}

void SaiPeriph::_config_rx_sai() {
	__HAL_SAI_RESET_HANDLE_STATE(&hsai_rx);
	hsai_rx.Instance = saidef_.rx_block;

	__HAL_SAI_DISABLE(&hsai_rx);

	if (saidef_.mode == SaiConfig::RXMaster) {
		hsai_rx.Init.AudioMode = SAI_MODEMASTER_RX;
		hsai_rx.Init.Synchro = SAI_ASYNCHRONOUS;
#if defined(STM32H7x5) || defined(STM32MP1)
		hsai_rx.Init.MckOutput = SAI_MCK_OUTPUT_ENABLE;
#endif
	} else {
		hsai_rx.Init.AudioMode = SAI_MODESLAVE_RX;
		hsai_rx.Init.Synchro = SAI_SYNCHRONOUS;
#if defined(STM32H7x5) || defined(STM32MP1)
		hsai_rx.Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;
#endif
	}
	hsai_rx.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	hsai_rx.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	hsai_rx.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
	hsai_rx.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
	hsai_rx.Init.AudioFrequency = saidef_.samplerate;
	hsai_rx.Init.MonoStereoMode = SAI_STEREOMODE;
	hsai_rx.Init.CompandingMode = SAI_NOCOMPANDING;
	hsai_rx.Init.TriState = SAI_OUTPUT_NOTRELEASED;
#if defined(STM32H7x5) || defined(STM32MP1)
	hsai_rx.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
	hsai_rx.Init.PdmInit.Activation = DISABLE;
#endif

	HAL_SAI_DeInit(&hsai_rx);
}

void SaiPeriph::_config_tx_sai() {
	__HAL_SAI_RESET_HANDLE_STATE(&hsai_tx);
	hsai_tx.Instance = saidef_.tx_block;
	__HAL_SAI_DISABLE(&hsai_tx);

	if (saidef_.mode == SaiConfig::RXMaster) {
		hsai_tx.Init.AudioMode = SAI_MODESLAVE_TX;
		hsai_tx.Init.Synchro = SAI_SYNCHRONOUS;
#if defined(STM32H7x5) || defined(STM32MP1)
		hsai_tx.Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;
#endif
	} else {
		hsai_tx.Init.AudioMode = SAI_MODEMASTER_TX;
		hsai_tx.Init.Synchro = SAI_ASYNCHRONOUS;
#if defined(STM32H7x5) || defined(STM32MP1)
		hsai_tx.Init.MckOutput = SAI_MCK_OUTPUT_ENABLE;
#endif
	}
	hsai_tx.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	hsai_tx.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	hsai_tx.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
	hsai_tx.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
	hsai_tx.Init.AudioFrequency = saidef_.samplerate;
	hsai_tx.Init.MonoStereoMode = SAI_STEREOMODE;
	hsai_tx.Init.CompandingMode = SAI_NOCOMPANDING;
	hsai_tx.Init.TriState = SAI_OUTPUT_NOTRELEASED;
#if defined(STM32H7x5) || defined(STM32MP1)
	hsai_tx.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
	hsai_tx.Init.PdmInit.Activation = DISABLE;
#endif
	HAL_SAI_DeInit(&hsai_tx);
}

SaiPeriph::Error SaiPeriph::_init_sai_protocol() {
	if (HAL_SAI_InitProtocol(&hsai_tx, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
		return SAI_INIT_ERR;

	if (HAL_SAI_InitProtocol(&hsai_rx, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
		return SAI_INIT_ERR;

	return SAI_NO_ERR;
}

void SaiPeriph::_config_rx_dma() {
#if defined(STM32H7x5) || defined(STM32MP1)
	hdma_rx.Init.Request = saidef_.dma_init_rx.channel;
#elif defined(STM32F7)
	hdma_rx.Init.Channel = saidef_.dma_init_rx.channel;
#endif

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
	// __HAL_LINKDMA(&hsai_rx, hdmatx, hdma_rx); //??!
	HAL_DMA_DeInit(&hdma_rx);
}

void SaiPeriph::_config_tx_dma() {
#if defined(STM32H7x5) || defined(STM32MP1)
	hdma_tx.Init.Request = saidef_.dma_init_tx.channel;
#elif defined(STM32F7)
	hdma_tx.Init.Channel = saidef_.dma_init_tx.channel;
#endif

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
	// __HAL_LINKDMA(&hsai_tx, hdmarx, hdma_tx); //??!
	HAL_DMA_DeInit(&hdma_tx);
}

SaiPeriph::Error SaiPeriph::_init_sai_dma() {
	// Todo always init slave first

	if (HAL_DMA_Init(&hdma_tx) != HAL_OK)
		return SAI_INIT_ERR;

	// __HAL_LINKDMA(&hsai_tx, hdmatx, hdma_tx);
	// __HAL_LINKDMA(&hsai_tx, hdmarx, hdma_tx); //??!

	if (HAL_DMA_Init(&hdma_rx) != HAL_OK)
		return SAI_INIT_ERR;

	// __HAL_LINKDMA(&hsai_rx, hdmarx, hdma_rx);
	// __HAL_LINKDMA(&hsai_rx, hdmatx, hdma_rx); //??!

	return SAI_NO_ERR;
}

void SaiPeriph::_init_pins() {
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

void SaiPeriph::set_txrx_buffers(uint8_t *tx_buf_ptr, uint8_t *rx_buf_ptr, uint32_t block_size) {
	rx_buf_ptr_ = rx_buf_ptr;
	tx_buf_ptr_ = tx_buf_ptr;
	block_size_ = block_size;
}
void SaiPeriph::set_callbacks(std::function<void()> &&tx_complete_cb, std::function<void()> &&tx_half_complete_cb) {
	tx_tc_cb = std::move(tx_complete_cb);
	tx_ht_cb = std::move(tx_half_complete_cb);
}

void SaiPeriph::start() {
	IRQn_Type _irqn;
	if (saidef_.mode == SaiConfig::RXMaster) {
		dma_tc_flag_index = dma_get_TC_flag_index(hdma_rx.Instance);
		dma_ht_flag_index = dma_get_HT_flag_index(hdma_rx.Instance);
		dma_te_flag_index = dma_get_TE_flag_index(hdma_rx.Instance);
		dma_isr_reg = dma_get_ISR_reg(saidef_.dma_init_rx.stream);
		dma_ifcr_reg = dma_get_IFCR_reg(saidef_.dma_init_rx.stream);
		_irqn = rx_irqn;
	} else {
		dma_tc_flag_index = dma_get_TC_flag_index(hdma_tx.Instance);
		dma_ht_flag_index = dma_get_HT_flag_index(hdma_tx.Instance);
		dma_te_flag_index = dma_get_TE_flag_index(hdma_tx.Instance);
		dma_isr_reg = dma_get_ISR_reg(saidef_.dma_init_tx.stream);
		dma_ifcr_reg = dma_get_IFCR_reg(saidef_.dma_init_tx.stream);
		_irqn = tx_irqn;
	}

	InterruptManager::registerISR(_irqn, [this]() {
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

	InterruptControl::enable_irq(_irqn);

	HAL_SAI_Transmit_DMA(&hsai_tx, tx_buf_ptr_, block_size_);
	HAL_SAI_Receive_DMA(&hsai_rx, rx_buf_ptr_, block_size_);
}

void SaiPeriph::stop() {
	InterruptControl::disable_irq(tx_irqn);
	InterruptControl::disable_irq(rx_irqn);
}
} // namespace mdrivlib
