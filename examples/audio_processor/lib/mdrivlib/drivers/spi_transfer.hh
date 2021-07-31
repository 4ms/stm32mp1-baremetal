#pragma once
#include "spi.hh"
#include "spi_transfer_config_struct.hh"
#include "util/math.hh"

// Wrapper around a chip's SPI peripheral, handling multi-chip buses.
// Note: to use the enable_fixed_size_interrupt() method you must enable the ISR outside of this driver.
// Note: The parent object of this is responsible for controlling any aux gpio pins: (data/cmd, dac latch, etc):
namespace mdrivlib
{

template<typename SpiConfT>
struct SpiTransferDriver {

	SpiTransferDriver() {
		unselect_all_chips();
		spi.disable();
		spi.configure();

		// Default to disable fixed-size transfers using End Of Transfer ISR
		spi.set_tx_message_size(0);
		spi.enable();
	}

private:
	SpiPeriph<SpiConfT> spi;
	uint32_t _cur_chip = 0;

public:
	void init() {
		spi.disable();
		spi.clear_TXTF_flag(); // needed?
		spi.enable();
	}

	void transmit_blocking(uint32_t word) {
		transmit_when_ready(word);
		begin_open_transmission();
		wait_until_tx_complete();
	}

	//
	// Open transmission: tx without specifing a number of packets
	//

	void begin_open_transmission() {
		spi.start_transfer();
	}
	void end_open_data_transmission() {
	}

	//
	// Fixed size transmission: tx with a known number of packets
	//

	void begin_fixed_size_transmission(uint32_t num_packets) {
		spi.disable();
		spi.set_tx_message_size(num_packets);
		spi.enable();
		spi.start_transfer();
	}
	bool is_fixed_size_tx_complete() {
		return spi.is_end_of_transfer();
	}
	void enable_fixed_size_interrupt() {
		spi.enable_end_of_xfer_interrupt();
	}
	void disable_fixed_size_interrupt() {
		spi.disable_end_of_xfer_interrupt();
	}
	void end_fixed_size_transmission() {
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
	}

	//
	// Transmit
	//
	void transmit_when_ready(uint32_t word) {
		wait_until_ready_to_tx();
		spi.load_tx_data(word);
	}
	void transmit(uint32_t word) {
		spi.load_tx_data(word);
	}
	bool is_tx_complete() {
		return spi.is_tx_complete();
	}
	void wait_until_tx_complete() {
		while (!spi.is_tx_complete())
			;
	}
	bool is_ready_to_tx() {
		return spi.tx_space_available();
	}
	void wait_until_ready_to_tx() {
		while (!spi.tx_space_available())
			;
	}

	//
	// Receive
	//
	uint32_t received_data() {
		return spi.received_data();
	}
	bool is_rx_packet_available() {
		return spi.rx_packet_available();
	}

	//
	// Chip selection
	//

	uint32_t cur_chip() const {
		return _cur_chip;
	}
	void select_chip(uint32_t chip) {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.select(chip);
	}
	void unselect_chip(uint32_t chip) {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.unselect(chip);
	}
	template<uint32_t chip>
	void select_chip() {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.template select<chip>();
	}
	template<uint32_t chip>
	void unselect_chip() {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.template unselect<chip>();
	}
	void select_cur_chip() {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.select(_cur_chip);
	}
	void unselect_cur_chip() {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.unselect(_cur_chip);
	}
	void unselect_all_chips() {
		auto tmp = _cur_chip;
		do {
			spi.unselect(_cur_chip);
			advance_chip();
		} while (_cur_chip != tmp);
	}

	// Select next chip. Steps through sequentially: CS0 -> CS1 -> CS2 -> CS3 -> CS0
	// Loops back to CS0 when it hits SpiConfT::NumChips.
	// It's safe to call even if there only one chip (will be optimized out)
	void advance_chip() {
		if constexpr (SpiConfT::NumChips == 1)
			return;
		if constexpr (SpiConfT::NumChips == 2) {
			_cur_chip = 1 - _cur_chip;
			return;
		}
		if constexpr (SpiConfT::NumChips > 2) {
			if (++_cur_chip >= SpiConfT::NumChips)
				_cur_chip = 0;
		}
	}
};
} // namespace mdrivlib
