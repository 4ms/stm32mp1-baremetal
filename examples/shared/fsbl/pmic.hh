#pragma once

#include "delay.h"
#include "drivers/i2c.hh"
#include "drivers/i2c_conf.hh"
#include "print_messages.h"

class STPMIC1 {

public:
	STPMIC1(const I2C_Config &conf)
		: i2c{PMIC_I2C_Address, conf}
	{
		verify_chip_id();
	}

	bool setup_ddr3_pwr()
	{
		auto ldo3 = i2c.read_register_byte(Register::LDO3_CR);
		log("Initial: LDO3_CR = 0x%x\n", ldo3.value_or(0xDEADBEEF));

		auto buck2 = i2c.read_register_byte(Register::BUCK2_CR);
		log("Initial: BUCK2_CR = 0x%x\n", buck2.value_or(0xDEADBEEF));

		auto refddr = i2c.read_register_byte(Register::REFDDR_CR);
		log("Initial: REFDDR_CR = 0x%x\n", refddr.value_or(0xDEADBEEF));

		LDOx_CR vtt{.bits = {
						.enable = 1,
						.vout = LDO3_VOUT2DIV2,
						.bypass = 0,
					}};
		if (!i2c.write_register_byte(Register::LDO3_CR, vtt.val))
			return false;

		BUCKx_CR vddr{.bits = {
						  .enable = 0,
						  .low_power = 0,
						  .vout = BUCK2_1350mV,
					  }};
		if (!i2c.write_register_byte(Register::BUCK2_CR, vddr.val))
			return false;

		vddr.bits.enable = 1;
		if (!i2c.write_register_byte(Register::BUCK2_CR, vddr.val))
			return false;

		udelay(1000);

		REFDDRx_CR ref{.bits = {.enable = 1}};
		if (!i2c.write_register_byte(Register::REFDDR_CR, ref.val))
			return false;

		udelay(1000);

		log("Writing LDO3_CR = 0x%x\n", vtt.val);
		log("Writing BUCK2_CR = 0x%x\n", vddr.val);
		log("Writing REFDDR_CR = 0x%x\n", ref.val);

		ldo3 = i2c.read_register_byte(Register::LDO3_CR);
		log("Read back: LDO3_CR = 0x%x\n", ldo3.value_or(0xDEADBEEF));

		buck2 = i2c.read_register_byte(Register::BUCK2_CR);
		log("Read back: BUCK2_CR = 0x%x\n", buck2.value_or(0xDEADBEEF));

		refddr = i2c.read_register_byte(Register::REFDDR_CR);
		log("Read back: REFDDR_CR = 0x%x\n", refddr.value_or(0xDEADBEEF));

		return true;
	}

	bool verify_chip_id()
	{
		auto id = i2c.read_register_byte(Register::ID);
		if (!id) {
			pr_err("Failed to read from PMIC\n");
			return false;
		}
		log("Read ID from PMIC: %02x\n", id.value());
		return true;
	}

private:
	I2C_Controller i2c;

	constexpr static uint32_t PMIC_I2C_Address = 0x33;
	enum Register : uint8_t {
		ID = 0x06,
		BUCK1_CR = 0x20,
		BUCK2_CR = 0x21,
		BUCK3_CR = 0x22,
		BUCK4_CR = 0x23,
		LDO1_CR = 0x25,
		LDO2_CR = 0x26,
		LDO3_CR = 0x27,
		LDO4_CR = 0x28,
		REFDDR_CR = 0x34,
	};

	enum RegisterAlt : uint8_t {

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
	constexpr static uint8_t BUCK2_1350mV = 30;	  // See datasheet, Table 10
};
