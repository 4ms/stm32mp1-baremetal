#pragma once
#include "sdram_config_struct.hh"
#include "stm32xx.h"

namespace SDRAMPeriphMath
{

static uint32_t freq_to_clockdiv(uint32_t FMC_clock, uint32_t freq) {
	// Requirement: FMC_clock / clockdiv >= freq
	// STM32H7 SDRAM controller: Only values of 2 and 3 are valid for clockdiv
	return ((freq * 2 * 1000000) >= FMC_clock) ? 2 : 3;
};

static constexpr uint32_t ns_to_hclks(const uint32_t sdram_clock, const uint32_t ns) {
	// return 16;
	uint32_t sdram_mhz = sdram_clock / 1000000U;
	uint32_t clks_x1000 = ns * sdram_mhz;
	uint32_t clks_x10 = clks_x1000 / 100;
	uint32_t round_up_clks_x10 = clks_x10 + 9;
	return round_up_clks_x10 / 10;
};

} // namespace SDRAMPeriphMath

class SDRAMPeriph {
public:
	SDRAMPeriph(const SDRAMConfig &sdram_defs) noexcept;
	static uint32_t test(const uint32_t ram_start, const uint32_t ram_size);
	static bool is_busy();
	static void wait_until_ready();

	uint32_t status;

private:
	const SDRAMConfig &defs;
	void config_timing();
	void start_refresh();
	void do_test();
	void init_gpio();
	static uint32_t do_sdram_test(uint32_t (*mapfunc)(uint32_t), const uint32_t ram_start, const uint32_t ram_size);

	uint32_t sdram_clock_;
};

