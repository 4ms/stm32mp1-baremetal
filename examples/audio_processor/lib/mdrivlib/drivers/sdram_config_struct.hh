#pragma once
#include "pin.hh"
#include <array>
#include <cstdint>

namespace mdrivlib
{

struct SDRAMTimingConfig {
	uint8_t tRC_ns;	 // Row Cycle time (same bank)
	uint8_t tRP_ns;	 // Precharge to refresh/row activate
	uint8_t tMRD_ns; // Mode register set cycle time? == LoadToActiveDelay
	uint8_t tXSR_ns; // Exit Self Refresh time
	uint8_t tRAS_ns; // Row activate to precharge time == Self-refresh?
	uint8_t tWR_ns;	 // Write Recovery time
	uint8_t tRCD_ns; // RAS to CAS delay (same bank)

	uint8_t max_freq_MHz;
	uint8_t CAS_latency;
	uint8_t refresh_ms;
	bool read_burst_enabled;
	uint8_t read_pipe_delay;
};

struct SDRAMArchConfig {
	uint8_t num_internal_banks;
	uint8_t bus_width_bits;
	uint8_t column_bits;
	uint8_t row_bits;
	uint32_t num_rows;
};

struct SDRAMPinNames {
	PinNoInit A0;
	PinNoInit A1;
	PinNoInit A2;
	PinNoInit A3;
	PinNoInit A4;
	PinNoInit A5;
	PinNoInit A6;
	PinNoInit A7;
	PinNoInit A8;
	PinNoInit A9;
	PinNoInit A10;
	PinNoInit A11;
	PinNoInit A12;
	PinNoInit BA0;
	PinNoInit BA1;
	PinNoInit D0;
	PinNoInit D1;
	PinNoInit D2;
	PinNoInit D3;
	PinNoInit D4;
	PinNoInit D5;
	PinNoInit D6;
	PinNoInit D7;
	PinNoInit D8;
	PinNoInit D9;
	PinNoInit D10;
	PinNoInit D11;
	PinNoInit D12;
	PinNoInit D13;
	PinNoInit D14;
	PinNoInit D15;
	PinNoInit NBL0;
	PinNoInit NBL1;
	PinNoInit SDCKE0;
	PinNoInit SDCLK;
	PinNoInit SDNCAS;
	PinNoInit SDNE0;
	PinNoInit SDNRAS;
	PinNoInit SDNWE;
};
union SDRAMPinList {
	SDRAMPinNames pin_names;
	std::array<PinNoInit, 39> pin_array;
};

struct SDRAMConfig {
	SDRAMPinList pin_list;
	SDRAMTimingConfig timing;
	SDRAMArchConfig arch;
	uint8_t connected_bank;
	uint32_t fmc_kernel_clock_MHz;
};
} // namespace mdrivlib
