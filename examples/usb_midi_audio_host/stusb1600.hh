#pragma once

#include "drivers/i2c.hh"
#include "drivers/i2c_conf.hh"

class STUSB1600 {

public:
	STUSB1600(const I2C_Config &conf)
		: i2c{I2C_Address, conf}
	{}

	bool write_reg(uint8_t reg, uint8_t val) { return i2c.write_register_byte(reg, val); }

	void print_reg(uint8_t reg)
	{
		auto val = i2c.read_register_byte(reg);
		if (val.has_value())
			printf("Read reg 0x%02x = 0x%02x\n", reg, val.value());
		else
			printf("Failed to read register 0x%02x\n", reg);
	}

	enum Registers : uint8_t {
		CCconnectionStatus = 0x0E,
		MonitoringStatus = 0x10,
		CCOperationStatus = 0x11,
		CCCaps = 0x18,
		ResetControl = 0x23,
		CCPowerMode = 0x28,
	};

private:
	I2C_Controller i2c;

	constexpr static uint32_t I2C_Address = 0x28;
};
