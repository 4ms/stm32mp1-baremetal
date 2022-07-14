#pragma once

#include "i2c_simple.hh"

class STPMIC1 {

public:
	STPMIC1(I2C_TypeDef *i2cperiph)
		: i2c{i2cperiph, PMIC_I2C_Address}
	{}

	bool setup_vddcore_pwr()
	{
		BUCKx_CR vddcore{.bits = {
							 .enable = 1,
							 .low_power = 0,
							 .vout = BUCKx_1200mV,
						 }};
		return i2c.write_register_byte(RegisterMain::BUCK1_CR, vddcore.val);
	}

	bool setup_ddr3_pwr()
	{
		LDOx_CR vtt{.bits = {
						.enable = 1,
						.vout = LDO3_VOUT2DIV2,
						.bypass = 0,
					}};

		BUCKx_CR vddr{.bits = {
						  .enable = 1,
						  .low_power = 0,
						  .vout = BUCK2_1350mV,
					  }};

		REFDDRx_CR ref{.bits = {
						   .enable = 1,
					   }};

		if (!i2c.write_register_byte(RegisterMain::LDO3_CR, vtt.val))
			return false;

		if (!i2c.write_register_byte(RegisterMain::BUCK2_CR, vddr.val))
			return false;

		udelay(1000);

		if (!i2c.write_register_byte(RegisterMain::REFDDR_CR, ref.val))
			return false;

		udelay(1000);

		// debug("Wrote PMIC reg ", RegisterMain::LDO3_CR, " (LDO3_CR) = 0x", Hex{vtt.val}, "\n");
		// debug("Wrote PMIC reg ", RegisterMain::BUCK2_CR, " (BUCK2_CR) = 0x", Hex{vddr.val}, "\n");
		// debug("Wrote PMIC reg ", RegisterMain::REFDDR_CR, " (REFDDR_CR) = 0x", Hex{ref.val}, "\n");
		return true;
	}

	bool setup_ldo1()
	{
		LDOx_CR ldo1{.bits = {
						 .enable = 1,
						 .vout = LDO1_VOUT1800mV,
						 .bypass = 0,
					 }};
		if (!i2c.write_register_byte(RegisterMain::LDO1_CR, ldo1.val))
			return false;
		udelay(10);
		return true;
	}

	bool verify_chip_id()
	{
		auto id = i2c.read_register_byte(RegisterMain::ID);
		if (!id) {
			// pr_err("Failed to read from PMIC\n");
			return false;
		}
		// log("Read ID from PMIC: ", Hex{id.value()}, "\n");
		return true;
	}

	void print_reg(uint8_t reg)
	{
		auto val = i2c.read_register_byte(reg);
		// if (val.has_value())
		// 	log("Read reg 0x", Hex{reg}, " = 0x", Hex{val.value()}, "\n");
		// else
		// 	log("Failed to read register ", Hex{reg}, "\n");
	}

private:
	I2C_Controller i2c;

	constexpr static uint32_t PMIC_I2C_Address = 0x33;
	enum RegisterMain : uint8_t {
		ID = 0x06,
		BUCK1_CR = 0x20,
		BUCK2_CR = 0x21,
		BUCK3_CR = 0x22,
		BUCK4_CR = 0x23,
		REFDDR_CR = 0x24,
		LDO1_CR = 0x25,
		LDO2_CR = 0x26,
		LDO3_CR = 0x27,
		LDO4_CR = 0x28,
	};

	union BUCKx_CR {
		struct {
			uint8_t enable : 1;
			uint8_t low_power : 1;
			uint8_t vout : 6;
		} bits;
		uint8_t val;
	};

	union LDOx_CR {
		struct {
			uint8_t enable : 1;
			uint8_t _resv : 1;
			uint8_t vout : 5;
			uint8_t bypass : 1;
		} bits;
		uint8_t val;
	};

	union REFDDRx_CR {
		struct {
			uint8_t enable : 1;
			uint8_t _resv : 7;
		} bits;
		uint8_t val;
	};

	constexpr static uint8_t LDO3_VOUT2DIV2 = 31; // See datasheet, Table 9
	constexpr static uint8_t LDO1_VOUT1800mV = 9; // See datasheet, Table 9
	constexpr static uint8_t BUCK2_1350mV = 30;	  // See datasheet, Table 10

	constexpr static uint8_t BUCKx_1200mV = 24; // See datasheet, Table 10

	static void udelay(uint32_t usec)
	{
		const uint32_t loops_per_us = 5;
		const uint32_t loops = usec * loops_per_us;
		volatile uint32_t t = loops;
		while (t)
			t = t - 1;
	}
};
