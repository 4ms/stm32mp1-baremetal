//
// stusb1600.hh
// Bare-bones driver for using STUSB1600 as a host (Src) only
//
// By Dan Green
// Copyright (c) 2022 Dan Green
// Licensed under MIT License, see LICENSE file
//

#pragma once

#include "drivers/i2c.hh"
#include "drivers/i2c_conf.hh"

class STUSB1600 {
private:
	I2C_Controller i2c;
	constexpr static uint32_t I2C_Address = 0x28;

public:
	enum Registers : uint8_t {
		CCconnectionStatus = 0x0E,
		MonitoringStatus = 0x10,
		CCOperationStatus = 0x11,
		CCCaps = 0x18,
		ResetControl = 0x23,
		CCPowerMode = 0x28,
	};

	STUSB1600(const I2C_Config &conf)
		: i2c{I2C_Address, conf}
	{
		reset();
	}

	bool enable_vbus_source_mode()
	{
		return write_reg(Registers::CCPowerMode, 0b000); // source power role with acc support
	}

	bool reset()
	{
		auto ok = write_reg(Registers::ResetControl, 1);
		HAL_Delay(10);
		if (ok) {
			ok = write_reg(Registers::ResetControl, 0);
			HAL_Delay(10);
		}
		return ok;
	}

	void print_reg(uint8_t reg)
	{
		auto val = read_reg(reg);
		if (val.has_value())
			printf("Read reg 0x%02x = 0x%02x\n", reg, val.value());
		else
			printf("Failed to read register 0x%02x\n", reg);
	}

	bool write_reg(uint8_t reg, uint8_t val) { return i2c.write_register_byte(reg, val); }
	std::optional<uint8_t> read_reg(uint8_t reg) { return i2c.read_register_byte(reg); }
};
