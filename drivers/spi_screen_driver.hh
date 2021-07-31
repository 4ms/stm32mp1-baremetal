#pragma once
#include "dma_config_struct.hh"
#include "drivers/memory_transfer.hh"
#include "interrupt.hh"
#include "spi.hh"
#include "util/math.hh"

namespace mdrivlib
{

template<typename ConfT, typename DmaTransferT>
struct DmaSpiScreenDriver {
	enum PacketType { Cmd, Data };

	DmaSpiScreenDriver() = default;

	void init() {
		spi.disable();
		spi.configure();
		spi.set_tx_message_size(1); // not needed?
		spi.enable();
	}

	template<PacketType MessageType>
	void transmit_blocking(uint8_t byte) {
		spi.disable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		spi.set_tx_message_size(1);
		spi.set_fifo_threshold(1);
		spi.disable_end_of_xfer_interrupt();
		spi.enable();
		if constexpr (MessageType == Cmd)
			dcpin.low();
		if constexpr (MessageType == Data)
			dcpin.high();
		spi.start_transfer();
		spi.load_tx_data(byte);
		while (!spi.is_tx_complete())
			;
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
	}

	template<PacketType MessageType>
	void transmit_blocking(uint16_t halfword1, uint16_t halfword2) {
		spi.disable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		spi.set_data_size(8);
		spi.set_tx_message_size(4);
		spi.enable();
		if constexpr (MessageType == Cmd)
			dcpin.low();
		if constexpr (MessageType == Data)
			dcpin.high();
		spi.load_tx_data(MathTools::swap_bytes_and_combine(halfword2, halfword1));
		spi.start_transfer();
		while (!spi.is_tx_complete())
			;
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
	}

	void config_dma_transfer(uint32_t src, uint32_t sz) {
		uint32_t dst = spi.get_tx_datareg_addr();

		if constexpr (ConfT::DMAConf::transfer_size_periph == ConfT::DMAConf::HalfWord) {
			spi.disable();
			spi.set_data_size(16);
			spi.enable();
		}
		if constexpr (ConfT::DMAConf::transfer_size_periph == ConfT::DMAConf::Word) {
			spi.disable();
			spi.set_data_size(32);
			spi.enable();
		}

		dma.config_transfer(dst, src, sz);
	}

	void start_dma_transfer(std::function<void(void)> &&cb) {
		callback = std::move(cb);
		dma.register_callback(callback);

		start_dma_transfer();
	}

	void start_dma_transfer() {
		spi.disable();
		dma.start_transfer();

		uint32_t sz = dma.get_transfer_size();
		if (sz <= 0xFFFF) {
			spi.set_tx_message_size(sz);
			spi.set_tx_message_reload_size(0);
		} else if (sz <= (0xFFFF * 2)) {
			spi.set_tx_message_size(0xFFFF);
			spi.set_tx_message_reload_size(sz - 0xFFFF);
		} else
			return; // Todo: handle transfers > 128k packets by setting up an ISR on TSERF that sets the reload size

		dcpin.high();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		spi.enable_tx_dma();
		spi.enable();
		spi.start_transfer();
	}

private:
	SpiPeriph<typename ConfT::ScreenSpiConf> spi;
	typename ConfT::DCPin dcpin;
	std::function<void(void)> callback;

	DmaTransferT dma;

	volatile uint32_t *dma_ifcr_reg;
	volatile uint32_t *dma_isr_reg;
	uint32_t dma_tc_flag_index;
	uint32_t dma_te_flag_index;
};
} // namespace mdrivlib
