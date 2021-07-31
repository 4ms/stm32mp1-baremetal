#pragma once
#include "arch.hh"
#include "rcc.hh"
#include "spi.hh"
#include "spi_transfer.hh"
#include "spi_transfer_config_struct.hh"

namespace mdrivlib
{
template<typename ConfT>
struct DacSpi_MCP48FVBxx {
	using SpiConf = typename ConfT::SpiConf;
	static constexpr unsigned NumChannelsPerChip = 2;

	DacSpi_MCP48FVBxx() {
		latch.high();
	}

	void init() {
		driver.init();
		configure_dac_registers();
		driver.begin_open_transmission();
		latch.low();
	}
	void configure_dac_registers() {
		for (int i = 0; i < ConfT::SpiConf::NumChips; i++) {
			driver.select_chip(i);
			driver.begin_open_transmission();
			driver.transmit(make_packet(VREF, WRITE, VrefB_PinUnbuffered | VrefA_PinUnbuffered));
			driver.transmit(make_packet(POWER, WRITE, PowerA_On | PowerB_On));
			driver.transmit(make_packet(GAINSTATUS, WRITE, GainB_1x | GainA_1x));
			// could be: queue_transmission(...); start_transmission()
			driver.wait_until_tx_complete();
			driver.unselect_chip(i);
		}
	}

public:
	void set_output_blocking(uint32_t chip, uint32_t val) {
		latch.high();
		driver.select_chip(chip);
		driver.begin_open_transmission();
		driver.transmit(make_packet(DACVALUE0, WRITE, (val >> 12) & 0xFFF));
		driver.transmit(make_packet(DACVALUE1, WRITE, val & 0xFFF));
		driver.wait_until_tx_complete();
		latch.low();
		driver.unselect_chip(chip);
	}

	void prepare_xfer(uint32_t chip, uint32_t val) {
		driver.select_chip(chip);
		driver.transmit(make_packet(DACVALUE0, WRITE, (val >> 12) & 0xFFF));
		driver.transmit(make_packet(DACVALUE1, WRITE, val & 0xFFF));
	}

	void do_xfer() {
		driver.begin_open_transmission();
	}

	void end_xfer(uint32_t chip) {
		driver.unselect_chip(chip);
	}

	SpiTransferDriver<typename ConfT::SpiConf> driver;
	typename ConfT::AuxPin latch;

private:
	enum Register : uint8_t {
		DACVALUE0 = 0x00 << 3,
		DACVALUE1 = 0x01 << 3,
		VREF = 0x08 << 3,
		POWER = 0x09 << 3,
		GAINSTATUS = 0x0A << 3,
	};
	enum Command : uint8_t {
		WRITE = 0b000,
		READ = 0b110,
	};
	enum Vref : uint16_t {
		VrefA_Disabled = 0x0000,
		VrefA_Internal = 0x0001,
		VrefA_PinUnbuffered = 0x0002,
		VrefA_PinBuffered = 0x0003,
		VrefB_Disabled = 0x0000,
		VrefB_Internal = 0x0004,
		VrefB_PinUnbuffered = 0x0008,
		VrefB_PinBuffered = 0x000C,
	};
	enum Power : uint16_t {
		PowerA_On = 0x0000,
		PowerA_OffPullDown1k = 0x0001,
		PowerA_OffPullDown100k = 0x0002,
		PowerA_OffFloating = 0x0003,
		PowerB_On = 0x0000,
		PowerB_OffPullDown1k = 0x0004,
		PowerB_OffPullDown100k = 0x0008,
		PowerB_OffFloating = 0x000C,
	};
	enum Gain : uint16_t {
		GainA_2x = (1 << 9),
		GainA_1x = 0x0000,
		GainB_2x = (1 << 8),
		GainB_1x = 0x0000,
		// Todo: R/O BOR bit
	};

	constexpr uint8_t make_command(Register r, Command c) {
		return r | c;
	}
	constexpr uint32_t make_packet(Register r, Command c, uint16_t data) {
		return (make_command(r, c) << 16) | data;
	}
	void _configure_dac_registers() {
		for (int i = 0; i < ConfT::SpiConf::NumChips; i++) {
			driver.select_chip(i);
			driver.begin_open_transmission();
			driver.transmit(make_packet(VREF, WRITE, VrefB_PinUnbuffered | VrefA_PinUnbuffered));
			driver.transmit(make_packet(POWER, WRITE, PowerA_On | PowerB_On));
			driver.transmit(make_packet(GAINSTATUS, WRITE, GainB_1x | GainA_1x));
			driver.wait_until_xfer_complete();
			driver.unselect_chip(i);
		}
	}
};
} // namespace mdrivlib
