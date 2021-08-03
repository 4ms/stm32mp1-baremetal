#pragma once
#include "drivers/i2c.hh"
#include "syscfg.hh"

namespace mdrivlib
{

struct I2C {
	static void init(const I2CConfig &defs) {
		// if (calc_clock_kHz(defs.timing) > 800000UL)
		// enable_fast_mode_plus(defs);
	}
	static void enable_fast_mode_plus(const I2CConfig &defs) {
		// H7x5 requires enabling FMP in SYSCFG, as well as specific pins, see RM0399 Rev 3, p. 587 (SYSCFG)
		RCC_Enable::SYSCFG_::set();

		auto tmp = SYSCFG_FMP::read();
		if (defs.I2Cx == I2C1)
			tmp |= SYSCFG_PMCR_I2C1_FMP;
		else if (defs.I2Cx == I2C2)
			tmp |= SYSCFG_PMCR_I2C2_FMP;
		else if (defs.I2Cx == I2C3)
			tmp |= SYSCFG_PMCR_I2C3_FMP;
		else if (defs.I2Cx == I2C4)
			tmp |= SYSCFG_PMCR_I2C4_FMP;

		if ((defs.SCL.gpio == GPIO::B && defs.SCL.pin == 6) || (defs.SDA.gpio == GPIO::B && defs.SDA.pin == 6))
			tmp |= SYSCFG_PMCR_I2C_PB6_FMP;
		if ((defs.SCL.gpio == GPIO::B && defs.SCL.pin == 7) || (defs.SDA.gpio == GPIO::B && defs.SDA.pin == 7))
			tmp |= SYSCFG_PMCR_I2C_PB7_FMP;
		if ((defs.SCL.gpio == GPIO::B && defs.SCL.pin == 8) || (defs.SDA.gpio == GPIO::B && defs.SDA.pin == 8))
			tmp |= SYSCFG_PMCR_I2C_PB8_FMP;
		if ((defs.SCL.gpio == GPIO::B && defs.SCL.pin == 9) || (defs.SDA.gpio == GPIO::B && defs.SDA.pin == 9))
			tmp |= SYSCFG_PMCR_I2C_PB7_FMP;

		SYSCFG_FMP::write(tmp);
	}
};
} // namespace mdrivlib
