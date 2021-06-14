#pragma once
#include "arch.hh"
#include "interrupt.hh"
#include "spi.hh"
#include "spi_config_struct.hh"
#include "spi_transfer.hh"
#include "system.hh"

namespace mdrivlib
{
// Todo: Try this: use hardware NSS, and use ch2sel pin to mux the NSS output to chip 1 or chip 2
// Then run SPI DMA in duplex mode for 16 packets: 8 with CH1 selected, 8 with CH2 selected.
// Only one ISR at the end of this 16-packet run (could use FIFO, instead of DMA)
// ISR toggles select pin and repeats

// 691ns with constexpr ConfT
// /256: at 81kHz: each channel = 20kHz, with OS=16 1.26kHz @ 16bit
// /128: at 145kHz: 13% load: each channel = 36kHz, OS=16 2.2kHz @ 16bit
// /64: at 250kHz: 22% load: each channel = 62kHz, OS=16 4kHz @ 16bit

// ConfT: Required to be derived from DefaultSpiConf
// PostFilterT: Requires void add_val(T) and T val()
template<typename ConfT, typename PostFilterT>
struct AdcSpi_MAX11666 {
	static constexpr unsigned NumChannelsPerChip = 2;

	AdcSpi_MAX11666() {
		_init();
	}

private:
	void _init() {
		cur_chan = 0;
		driver.init();
	}

public:
	void start_circular_mode() {
		driver.enable_fixed_size_interrupt();
		InterruptManager::registerISR(ConfT::IRQn, ConfT::priority1, ConfT::priority2, [this]() {
			read_and_switch_channels();
			// Todo: check for OVR flag, UDR flag, MODF, FRE
		});
	}

	void start() {
		driver.select_cur_chip();
		driver.transmit(cur_chan == 1 ? SWITCH_TO_CH1 : SWITCH_TO_CH2);
		driver.begin_open_transmission();
	}

	void read_and_switch_channels() {
		if (driver.is_rx_packet_available()) {
			driver.unselect_cur_chip();
			store_reading(driver.received_data());
			advance_chip_and_channel();
		}
		start();
	}

	void store_reading(uint16_t reading) {
		postfilter[buffer_idx()].add_val((reading >> 2) & 0xFFF);
	}

	uint16_t get_val(int chan) {
		return postfilter[chan].val();
	}

private:
	SpiTransferDriver<ConfT> driver;
	PostFilterT postfilter[ConfT::NumChips * NumChannelsPerChip];
	unsigned cur_chan = 0;

	void advance_chip_and_channel() {
		driver.advance_chip();
		if (driver.cur_chip() == 0)
			advance_channel();
	}

	void advance_channel() {
		cur_chan = 1 - cur_chan;
	}

	unsigned buffer_idx() const {
		return driver.cur_chip() * 2 + cur_chan;
	}

	enum Max11666Commands {
		SWITCH_TO_CH1 = 0x00FF,
		CONTINUE_READING_CH1 = 0x0000,
		SWITCH_TO_CH2 = 0xFF00,
		CONTINUE_READING_CH2 = 0xFFFF,
	};
};
} // namespace mdrivlib
