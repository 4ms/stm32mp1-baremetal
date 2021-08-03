#pragma once
#include "interrupt.hh"
#include "spi.hh"
#include "spi_config_struct.hh"
#include "util/math.hh"

// WIP!!!
template<typename ConfT>
struct SpiDataCmdDriver {

	SpiDataCmdDriver() {
		spi.disable();
		spi.configure();
		spi.set_tx_message_size(0);
		spi.enable();
	}

private:
	SpiPeriph<typename ConfT::SpiConf> spi;
	typename ConfT::AuxPin auxpin;

	void wait_until_ready() {
		while (!spi.tx_space_available())
			;
	}

protected:
	// is this needed?
	volatile bool _ready;
	void wait_until_EOT() {
		while (!_ready) {
		}
		_ready = false;
	}
	void enable_EOT_ISR() {
		InterruptManager::registerISR(ConfT::SpiConf::IRQn, [this]() {
			if (spi.is_end_of_transfer()) {
				spi.clear_EOT_flag();
				spi.clear_TXTF_flag();
				_ready = true;
			}
		});
		_ready = true;
		target::System::set_irq_priority(ConfT::SpiConf::IRQn, ConfT::SpiConf::priority1, ConfT::SpiConf::priority2);
		target::System::enable_irq(ConfT::SpiConf::IRQn);
		spi.enable_end_of_xfer_interrupt();
	}

	// in screen driver:
	// enum PacketType { Cmd, Data };
	// template<PacketType MessageType>
	// void transmit(uint8_t byte) {
	// 	if constexpr (MessageType == Cmd)
	// 		set_auxpin_and_transmit_blocking<Low>(byte);
	// 	if constexpr (MessageType == Data)
	// 		set_auxpin_and_transmit_blocking<High>(byte);
	// }
	//
	// void transmit_data(uint16_t halfword1, uint16_t halfword2) {
	// 	set_auxpin_and_transmit_blocking<Data>(MathTools::swap_bytes_and_combine(halfword2, halfword1));
	// }
	// void transmit_data(uint32_t word) {
	// 	set_auxpin_and_transmit_blocking<Data>(MathTools::MathTools::swap_bytes32(word));
	// }

	void screen_driver_start_xfer(uint32_t num_packets) {
		wait_until_ready();
		// can we just set fifo to 1 all the time?
		spi.disable();
		num_packets = MathTools::min(spi.get_max_fifo_packets(), num_packets);
		spi.set_fifo_threshold(num_packets);
		spi.enable();
		//??? needed?
		// spi.clear_EOT_flag();
		// spi.clear_TXTF_flag();
		spi.start_transfer();
	}
	// then call auxpin.low/high()
	// then call spi.load_tx_data() as many times as needed
	// Blocking
	enum AuxPinState { Low, High };
	template<AuxPinState MessageType>
	void set_auxpin_and_transmit_blocking(uint8_t byte) {
		wait_until_ready();
		spi.disable();
		// spi.set_tx_message_size(1);
		spi.set_fifo_threshold(1);
		spi.enable();
		if constexpr (MessageType == Low)
			auxpin.low();
		if constexpr (MessageType == High)
			auxpin.high();
		spi.load_tx_data(byte);
		spi.start_transfer();
	}

	template<AuxPinState MessageType>
	void set_auxpin_and_transmit_blocking(uint16_t halfword) {
		wait_until_ready();
		spi.disable();
		// spi.set_tx_message_size(2);
		spi.set_fifo_threshold(2); //??
		spi.enable();
		if constexpr (MessageType == Low)
			auxpin.low();
		if constexpr (MessageType == High)
			auxpin.high();
		spi.load_tx_data(halfword);
		spi.start_transfer();
	}

	template<AuxPinState MessageType>
	void set_auxpin_and_transmit_blocking(uint32_t word) {
		wait_until_ready();
		spi.disable();
		// spi.set_tx_message_size(4);
		spi.set_fifo_threshold(4);
		spi.enable();
		if constexpr (MessageType == Low)
			auxpin.low();
		if constexpr (MessageType == High)
			auxpin.high();
		spi.load_tx_data(word);
		spi.start_transfer();
	}

	// do we need to set fifo size or can it always be 1?
	void begin_open_data_transmission(uint8_t fifo_size) {
		wait_until_ready();
		spi.disable();
		spi.set_fifo_threshold(fifo_size);
		spi.clear_TXTF_flag(); // needed?
		spi.enable();
		spi.start_transfer();
	}
	void transmit_open_data16(uint16_t halfword) {
		wait_until_ready();
		spi.load_tx_data(halfword);
	}
	void transmit_open_data32(uint32_t word) {
		wait_until_ready();
		spi.load_tx_data(word);
	}
	void end_open_data_transmission() {
		// needed?
		spi.disable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		spi.enable();
	}
};
