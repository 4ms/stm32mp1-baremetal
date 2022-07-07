#include "drivers/rcc.hh"
#include "stm32mp1xx_hal_rcc.h"

struct SystemClocks {
	static unsigned init_core_clocks(uint32_t MPU_MHz = 800)
	{
		RCC_OscInitTypeDef rcc_osc_conf = {
			.OscillatorType = RCC_OSCILLATORTYPE_HSE,
			.HSEState = RCC_HSE_BYPASS,
			.PLL =
				{
					.PLLState = RCC_PLL_ON,
					.PLLSource = RCC_PLL12SOURCE_HSE,
					.PLLM = 3,
					.PLLN = MPU_MHz == 650 ? 81U : 100U,
					.PLLP = 1,
					.PLLQ = 2,
					.PLLR = 2,
					.PLLFRACV = MPU_MHz == 650 ? 2048U : 0U,
					.PLLMODE = MPU_MHz == 650 ? RCC_PLL_FRACTIONAL : RCC_PLL_INTEGER,
				},
			.PLL2 =
				{
					.PLLState = RCC_PLL_ON,
					.PLLSource = RCC_PLL12SOURCE_HSE,
					.PLLM = 3,
					.PLLN = 66,
					.PLLP = 2,
					.PLLQ = 1,
					.PLLR = 1,
					.PLLFRACV = 5120,
					.PLLMODE = RCC_PLL_FRACTIONAL,
				},
		};

		using namespace mdrivlib;
		using namespace mdrivlib::RCC_Clocks;
		MPUClockSrc::write(RCC_Clocks::MPUClockSrcHSI);
		AXISClockSrc::write(RCC_Clocks::AXISClockSrcHSI);

		// HSE
		{
			OscEnableHSEON::clear();
			while (HSEClockReady::read())
				;

			// Set Bypass Mode = analog oscillator
			OscEnableDIGBYP::clear();
			OscEnableHSEBYP::set();

			OscEnableHSEON::set();
			while (!HSEClockReady::read())
				;
		}

		// PLL1
		{
			PLL1::DIVPEnable::clear();
			PLL1::DIVQEnable::clear();
			PLL1::DIVREnable::clear();
			PLL1::Enable::clear();
			while (PLL1::Ready::read())
				;

			PLL12Source::write(PLL12SourceHSE);
			while (!PLL12SourceReady::read())
				;
			PLL1::DIVM1::write(3 - 1);
			PLL1::DIVN::write((MPU_MHz == 650 ? 81U : 100U) - 1);
			PLL1::DIVP::write(1 - 1);
			PLL1::DIVQ::write(2 - 1);
			PLL1::DIVR::write(2 - 1);
			PLL1::FRACLatch::clear();
			PLL1::FRACValue::write(0);
			PLL1::FRACLatch::set();
			PLL1::SpreadSpectrumClockGen::Enable::clear();

			PLL1::Enable::set();
			while (!PLL1::Ready::read())
				;
			PLL1::DIVPEnable::set();
			PLL1::DIVQEnable::set();
			PLL1::DIVREnable::set();
		}

		// PLL2
		{
			PLL2::DIVPEnable::clear();
			PLL2::DIVQEnable::clear();
			PLL2::DIVREnable::clear();
			PLL2::Enable::clear();
			while (PLL2::Ready::read())
				;

			PLL2::DIVM2::write(3 - 1);
			PLL2::DIVN::write(66 - 1);
			PLL2::DIVP::write(2 - 1);
			PLL2::DIVQ::write(1 - 1);
			PLL2::DIVR::write(1 - 1);
			PLL2::FRACLatch::clear();
			PLL2::FRACValue::write(5120);
			PLL2::FRACLatch::set();
			PLL2::SpreadSpectrumClockGen::Enable::clear();

			// Enable it
			PLL2::Enable::set();
			while (!PLL2::Ready::read())
				;
			PLL2::DIVPEnable::set();
			PLL2::DIVQEnable::set();
			PLL2::DIVREnable::set();
		}

		MPUClockSrc::write(MPUClockSrcPLL1);
		while (!MPUClockReady::read())
			;

		RCC_ClkInitTypeDef rcc_mpuclk_conf = {
			.ClockType = RCC_CLOCKTYPE_MPU | RCC_CLOCKTYPE_ACLK,
			.MPUInit =
				{
					.MPU_Clock = RCC_MPUSOURCE_PLL1,
					.MPU_Div = RCC_MPU_DIV_OFF,
				},
			.AXISSInit =
				{
					.AXI_Clock = RCC_AXISSOURCE_PLL2,
					.AXI_Div = RCC_MPU_DIV_OFF,
				},
		};

		auto clk_err = HAL_RCC_ClockConfig(&rcc_mpuclk_conf);
		return static_cast<unsigned>(clk_err);
	}
};
