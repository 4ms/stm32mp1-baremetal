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
	enum class Register : uint8_t {
		CCconnectionStatus = 0x0E,
		MonitoringStatus = 0x10,
		CCOperationStatus = 0x11,
		CCCap = 0x18,
		ResetControl = 0x23,
		VBusDischargeTime = 0x25,
		CCPowerMode = 0x28,
	};
	enum CCPowerModes : uint8_t {
		SourcePowerRole = 0b000,
		SinkPowerRole = 0b001,
		SinkPowerRoleNoAcc = 0b010,
		DualPowerRole = 0b011,
		DualPowerRoleTrySRC = 0b100,
		DualPowerRoleTrySNK = 0b101,
	};
	enum CCCaps : uint8_t {
		DefaultCurrent = 0b00000000,
	};
	enum VBusDischargeTimes : uint8_t {
		T504ms = 0b11010000,
	};

	STUSB1600(const I2C_Config &conf)
		: i2c{I2C_Address, conf}
	{
		reset();
	}

	bool enable_vbus_source_mode()
	{
		if (!write_reg(Register::CCCap, CCCaps::DefaultCurrent))
			return false;
		if (!write_reg(Register::VBusDischargeTime, VBusDischargeTimes::T504ms))
			return false;
		if (!write_reg(Register::CCPowerMode, CCPowerModes::SourcePowerRole))
			return false;
		return true;
	}

	bool reset()
	{
		if (!write_reg(Register::ResetControl, 1))
			return false;
		if (!write_reg(Register::ResetControl, 0))
			return false;
		return true;
	}

	void print_reg(Register reg)
	{
		auto val = read_reg(reg);
		if (val.has_value())
			printf("Read reg 0x%02x = 0x%02x\n", static_cast<uint8_t>(reg), val.value());
		else
			printf("Failed to read register 0x%02x\n", static_cast<uint8_t>(reg));
	}

	bool write_reg(Register reg, uint8_t val) { return i2c.write_register_byte(static_cast<uint8_t>(reg), val); }
	std::optional<uint8_t> read_reg(Register reg) { return i2c.read_register_byte(static_cast<uint8_t>(reg)); }
};
