#pragma once

#include "drivers/i2c.hh"
#include "drivers/i2c_conf.hh"
#include "print_messages.h"

class STPMIC1 {
	I2C_Controller i2c;
	constexpr static uint32_t PMIC_I2C_Address = 0x33;

public:
	STPMIC1(const I2C_Config &conf)
		: i2c{PMIC_I2C_Address, conf}
	{

		printf_("init i2c %08x\n", conf.periph);

		uint8_t data[1] = {0x06};
		i2c.write(data);

		uint8_t id = i2c.read_byte();
		printf_("Read: %d\n", id);
		__BKPT();
	}
};
