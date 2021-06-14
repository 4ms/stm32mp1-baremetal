#include "sdram.hh"
#include "arch.hh"
#include "interrupt.hh"
#include "pin.hh"
#include "rcc.hh"
#include "stm32xx.h"

#define SDRAM_DO_TEST

SDRAMPeriph::SDRAMPeriph(const SDRAMConfig &sdram_defs) noexcept
	: status(HAL_ERROR)
	, defs(sdram_defs) {

	init_gpio();
	__HAL_RCC_FMC_CLK_ENABLE();
	config_timing();
	start_refresh();

#ifdef SDRAM_DO_TEST
	do_test();
#endif
	status = HAL_OK;
}

static SDRAM_HandleTypeDef hsdram2;
void SDRAMPeriph::config_timing() {

	// Todo: pass the FMC clock in the conf, or use System::FMC::get_clock_speed()
	const uint32_t fmc_clock = defs.fmc_kernel_clock_MHz * 1000000UL;
	const uint32_t clk_divider = SDRAMPeriphMath::freq_to_clockdiv(fmc_clock, defs.timing.max_freq_MHz);
	sdram_clock_ = fmc_clock / clk_divider;

	FMC_SDRAM_TimingTypeDef SdramTiming = {
		.LoadToActiveDelay = SDRAMPeriphMath::ns_to_hclks(sdram_clock_, defs.timing.tMRD_ns),
		.ExitSelfRefreshDelay = SDRAMPeriphMath::ns_to_hclks(sdram_clock_, defs.timing.tXSR_ns),
		.SelfRefreshTime = SDRAMPeriphMath::ns_to_hclks(sdram_clock_, defs.timing.tRAS_ns),
		.RowCycleDelay = SDRAMPeriphMath::ns_to_hclks(sdram_clock_, defs.timing.tRC_ns),
		.WriteRecoveryTime = SDRAMPeriphMath::ns_to_hclks(sdram_clock_, defs.timing.tWR_ns),
		.RPDelay = SDRAMPeriphMath::ns_to_hclks(sdram_clock_, defs.timing.tRP_ns),
		.RCDDelay = SDRAMPeriphMath::ns_to_hclks(sdram_clock_, defs.timing.tRCD_ns),
	};

	FMC_SDRAM_InitTypeDef init = {
		.SDBank = defs.connected_bank == 1 ? FMC_SDRAM_BANK1 : FMC_SDRAM_BANK2,

		.ColumnBitsNumber = defs.arch.column_bits == 11 ? FMC_SDRAM_COLUMN_BITS_NUM_11
						  : defs.arch.column_bits == 10 ? FMC_SDRAM_COLUMN_BITS_NUM_10
						  : defs.arch.column_bits == 9	? FMC_SDRAM_COLUMN_BITS_NUM_9
														: FMC_SDRAM_COLUMN_BITS_NUM_8,

		.RowBitsNumber = defs.arch.row_bits == 13 ? FMC_SDRAM_ROW_BITS_NUM_13
					   : defs.arch.row_bits == 12 ? FMC_SDRAM_ROW_BITS_NUM_12
												  : FMC_SDRAM_ROW_BITS_NUM_11,

		.MemoryDataWidth = defs.arch.bus_width_bits == 32 ? FMC_SDRAM_MEM_BUS_WIDTH_32
						 : defs.arch.bus_width_bits == 16 ? FMC_SDRAM_MEM_BUS_WIDTH_16
														  : FMC_SDRAM_MEM_BUS_WIDTH_8,
		.InternalBankNumber =
			defs.arch.num_internal_banks == 4 ? FMC_SDRAM_INTERN_BANKS_NUM_4 : FMC_SDRAM_INTERN_BANKS_NUM_2,

		.CASLatency = defs.timing.CAS_latency == 2 ? FMC_SDRAM_CAS_LATENCY_2
					: defs.timing.CAS_latency == 1 ? FMC_SDRAM_CAS_LATENCY_1
												   : FMC_SDRAM_CAS_LATENCY_3,

		.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE,

		.SDClockPeriod = clk_divider == 2 ? FMC_SDRAM_CLOCK_PERIOD_2 : FMC_SDRAM_CLOCK_PERIOD_3,

		.ReadBurst = defs.timing.read_burst_enabled ? FMC_SDRAM_RBURST_ENABLE : FMC_SDRAM_RBURST_DISABLE,

		.ReadPipeDelay = defs.timing.read_pipe_delay == 0 ? FMC_SDRAM_RPIPE_DELAY_0
					   : defs.timing.read_pipe_delay == 1 ? FMC_SDRAM_RPIPE_DELAY_1
														  : FMC_SDRAM_RPIPE_DELAY_2,
	};

	FMC_SDRAM_Init(FMC_SDRAM_DEVICE, &init);
	FMC_SDRAM_Timing_Init(FMC_SDRAM_DEVICE, &SdramTiming, init.SDBank);

	__FMC_ENABLE();
	// FMC_Bank1_R->BTCR[0] |= FMC_BCR1_FMCEN;

	// Debugging:
	InterruptManager::registerISR(FMC_IRQn, []() { __BKPT(); });
	FMC_SDRAM_DEVICE->SDRTR |= FMC_SDRTR_REIE;
}

void SDRAMPeriph::start_refresh() {
	FMC_SDRAM_CommandTypeDef cmd;

	HAL_Delay(1);
	wait_until_ready();
	auto bank = defs.connected_bank == 1 ? FMC_SDRAM_CMD_TARGET_BANK1 : FMC_SDRAM_CMD_TARGET_BANK2;

	// CLK ENABLE command
	cmd = {.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE,
		   .CommandTarget = bank,
		   .AutoRefreshNumber = 2,
		   .ModeRegisterDefinition = 0};
	FMC_SDRAM_SendCommand(FMC_SDRAM_DEVICE, &cmd, 0);

	HAL_Delay(10);
	wait_until_ready();

	// All Bank Precharge command
	cmd = {
		.CommandMode = FMC_SDRAM_CMD_PALL,
		.CommandTarget = bank,
		.AutoRefreshNumber = 2,
		.ModeRegisterDefinition = 0,
	};
	FMC_SDRAM_SendCommand(FMC_SDRAM_DEVICE, &cmd, 0);
	HAL_Delay(1);
	wait_until_ready();

	// Auto-refresh command
	cmd = {.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE,
		   .CommandTarget = bank,
		   .AutoRefreshNumber = 5,
		   .ModeRegisterDefinition = 0};
	FMC_SDRAM_SendCommand(FMC_SDRAM_DEVICE, &cmd, 0);
	HAL_Delay(1);
	wait_until_ready();

	// Load Mode Register
	// Todo: Calculate this constant based on defs (add new config fields if needed)
	// Mode Register = 0x231: burst length 2, burst type sequential, CAS latency 3 clocks, Write
	// burst mode single bit, normal operation mode.
	// Mode Register = 0x030: burst length 1, burst type sequential, CAS latency 3 clocks, Write
	// burst mode = single location access, normal operation mode
	cmd = {.CommandMode = FMC_SDRAM_CMD_LOAD_MODE,
		   .CommandTarget = bank,
		   .AutoRefreshNumber = 2,
		   .ModeRegisterDefinition = 0x231};
	FMC_SDRAM_SendCommand(FMC_SDRAM_DEVICE, &cmd, 0);
	HAL_Delay(1);
	wait_until_ready();

	// Refresh rate in number of SDCLK clock cycles between the refresh cycles
	auto refresh_ms_to_refcount = [sdram_clock = sdram_clock_](uint32_t refresh_ms, uint32_t num_rows) -> uint32_t {
		return 800;
		// uint32_t rate_us = (refresh_ms * 10000) / num_rows;
		// uint32_t count = (sdram_clock / 1000000U) * rate_us;
		// return (count / 10) - 20; // Safe margin, as per STM32F74x datasheet
	};
	FMC_SDRAM_ProgramRefreshRate(FMC_SDRAM_DEVICE, refresh_ms_to_refcount(defs.timing.refresh_ms, defs.arch.num_rows));
	HAL_Delay(1);
	wait_until_ready();
}

void SDRAMPeriph::init_gpio() {
	// Hack for H7x5: pins PC_2C and PC_3C must be configured. See RM0399 Rev 3, page 588
	target::RCC_Enable::SYSCFG_::set();
	SYSCFG->PMCR = SYSCFG->PMCR & ~(SYSCFG_PMCR_PC2SO_Msk | SYSCFG_PMCR_PC3SO_Msk);

	for (auto &pind : defs.pin_list.pin_array) {
		Pin{pind.gpio,
			pind.pin,
			PinMode::Alt,
			pind.af,
			PinPull::None,
			PinPolarity::Normal,
			PinSpeed::VeryHigh,
			PinOType::PushPull};
	}
}

bool SDRAMPeriph::is_busy() {
#ifdef FMC_SDSR_BUSY
	return __FMC_SDRAM_GET_FLAG(FMC_SDRAM_DEVICE, FMC_SDRAM_FLAG_BUSY);
#else
	return false;
#endif
}

void SDRAMPeriph::wait_until_ready() {
	while (is_busy())
		;
}

uint32_t SDRAMPeriph::test(const uint32_t ram_start, const uint32_t ram_size) {
	uint32_t num_fails = 0;

	auto countup_func = [](uint32_t x) { return x; };
	num_fails += do_sdram_test(countup_func, ram_start, ram_size);

	auto bitinvert_countdown_func = [](uint32_t x) { return 0xFFFFFFFFU - x; };
	num_fails += do_sdram_test(bitinvert_countdown_func, ram_start, ram_size);

	return num_fails;
}

uint32_t SDRAMPeriph::do_sdram_test(uint32_t (*mapfunc)(uint32_t), const uint32_t ram_start, const uint32_t ram_size) {
	uint32_t num_fails = 0;
	const size_t test_val_size = sizeof(uint32_t);

	uint32_t addr = ram_start;
	for (uint32_t i = 0; i < (ram_size / test_val_size); i++) {
		// wait_until_ready();
		uint32_t testval = mapfunc(i);
		*((uint32_t *)addr) = testval;

		addr += test_val_size;
	}

	addr = ram_start;
	for (uint32_t i = 0; i < (ram_size / test_val_size); i++) {
		uint32_t readval = *((uint32_t *)addr);

		uint32_t expectedval = mapfunc(i);
		if (readval != expectedval)
			num_fails++;

		addr += test_val_size;
	}
	return num_fails;
}

void SDRAMPeriph::do_test() {
	// 434ms
	// vs SRAM1/2: 128ms
	// vs DTCM: 111ms
	uint32_t start = HAL_GetTick();
	volatile uint32_t sdram_fails = SDRAMPeriph::test(0xC0000000, 0x00800000);
	volatile uint32_t elapsed = HAL_GetTick() - start;
	if (sdram_fails)
		__BKPT();
	if (elapsed > 540) {
		__BKPT();
	}
	// else
	// 	__BKPT();
}
