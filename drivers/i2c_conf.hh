#pragma once
#include "drivers/pinconf.hh"

enum class I2C_Periph {
	I2C1_ = I2C1_BASE,
	I2C2_ = I2C2_BASE,
	I2C3_ = I2C3_BASE,
	I2C4_ = I2C4_BASE,
	I2C5_ = I2C5_BASE,
	I2C6_ = I2C6_BASE,
};

struct I2C_Config {
	I2C_Periph periph;
	PinConf sda_pin;
	PinConf scl_pin;
};
