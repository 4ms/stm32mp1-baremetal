#include "drivers/rcc.hh"
#include "stm32mp1xx_hal_rcc.h"
#include <stdint.h>

struct SystemClocks {
	static unsigned init_core_clocks(uint32_t HSE_Clock = 24000000, uint32_t MPU_MHz = 800)
	{
		constexpr uint32_t pll1m = 3;
		const uint32_t pll1n = (MPU_MHz == 650 ? 81U : 100U);
		constexpr uint32_t pll1p = 1;
		constexpr uint32_t pll1q = 2;
		constexpr uint32_t pll1r = 2;
		constexpr uint32_t pll1frac = 0;

		constexpr uint32_t pll2m = 3;
		constexpr uint32_t pll2n = 66;
		constexpr uint32_t pll2p = 2;
		constexpr uint32_t pll2q = 1;
		constexpr uint32_t pll2r = 1;
		constexpr uint32_t pll2frac = 5120;

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
			PLL1::DIVM1::write(pll1m - 1);
			PLL1::DIVN::write(pll1n - 1);
			PLL1::DIVP::write(pll1p - 1);
			PLL1::DIVQ::write(pll1q - 1);
			PLL1::DIVR::write(pll1r - 1);
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

			PLL2::DIVM2::write(pll2m - 1);
			PLL2::DIVN::write(pll2n - 1);
			PLL2::DIVP::write(pll2p - 1);
			PLL2::DIVQ::write(pll2q - 1);
			PLL2::DIVR::write(pll2r - 1);
			PLL2::FRACLatch::clear();
			PLL2::FRACValue::write(pll2frac);
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
		while (!MPUClockSrcReady::read())
			;

		AXISClockSrc::write(AXISClockSrcPLL2);
		while (!AXISClockSrcReady::read())
			;

		AXIDiv::write(AXIDivOff);
		while (!AXIDivReady::read())
			;

		float coreclock = (float)HSE_Clock * ((float)pll1n + (float)pll1frac / 8191.f) / ((float)pll1m * (float)pll1p);
		SystemCoreClock = static_cast<uint32_t>(coreclock);
		return SystemCoreClock;
	}
};
