#pragma once
#include "interrupt.hh"
#include "pin.hh"
#include "sai_config_struct.hh"
#include <cstdint>

namespace mdrivlib
{
class SaiTdmPeriph {
public:
	enum Error {
		SAI_NO_ERR,
		SAI_INIT_ERR,
		SAI_I2C_ERR,
		SAI_XMIT_ERR,
	};

	SaiTdmPeriph(const SaiConfig &def)
		: saidef_(def) {
	}

	~SaiTdmPeriph() = default;

	Error init();
	void set_rx_buffers(uint8_t *rx_buf_ptr, uint32_t block_size);
	void set_tx_buffers(uint8_t *tx_buf_ptr, uint32_t block_size);
	void set_callbacks(std::function<void()> &&tx_complete_cb, std::function<void()> &&tx_half_complete_cb);

	void start();
	void stop();

private:
	const SaiConfig &saidef_;
	DMA_HandleTypeDef hdma_tx;
	DMA_HandleTypeDef hdma_rx;
	SAI_HandleTypeDef hsai_tx;
	SAI_HandleTypeDef hsai_rx;
	IRQn_Type tx_irqn;
	IRQn_Type rx_irqn;
	uint8_t *tx_buf_ptr_;
	uint8_t *rx_buf_ptr_;
	uint32_t tx_block_size_;
	uint32_t rx_block_size_;

	void _init_pins();
	Error _config_rx_sai();
	Error _config_tx_sai();
	void _config_rx_dma();
	void _config_tx_dma();
	Error _init_sai_dma();
	void _sai_enable(SAI_HandleTypeDef *hsai);
	void _sai_disable(SAI_HandleTypeDef *hsai);
	void _start_irq(IRQn_Type irqn);

	std::function<void()> tx_tc_cb;
	std::function<void()> tx_ht_cb;

	// Todo: can we use RegisterBits<>, if SaiConfig is a constexpr?
	// or: Saiconfig contains a RegisterBits object or type alias?
	uint32_t dma_tc_flag_index;
	uint32_t dma_ht_flag_index;
	uint32_t dma_te_flag_index;
	volatile uint32_t *dma_isr_reg;
	volatile uint32_t *dma_ifcr_reg;
};
} // namespace mdrivlib
