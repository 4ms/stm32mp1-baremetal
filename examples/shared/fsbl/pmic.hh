#pragma once

#include "drivers/i2c_conf.hh"
#include "drivers/rcc.hh"
#include "print_messages.h"

class STPMIC1 {
	I2C_TypeDef *i2c;

public:
	STPMIC1(const I2C_Config &conf)
	{
		conf.scl_pin.init(PinMode::Alt);
		conf.sda_pin.init(PinMode::Alt);

		if (conf.periph == I2C_Periph::I2C1_)
			mdrivlib::RCC_Enable::I2C1_::set();

		if (conf.periph == I2C_Periph::I2C2_)
			mdrivlib::RCC_Enable::I2C2_::set();

		if (conf.periph == I2C_Periph::I2C3_)
			mdrivlib::RCC_Enable::I2C3_::set();

		if (conf.periph == I2C_Periph::I2C4_)
			mdrivlib::RCC_Enable::I2C4_::set();

		if (conf.periph == I2C_Periph::I2C5_)
			mdrivlib::RCC_Enable::I2C5_::set();

		if (conf.periph == I2C_Periph::I2C6_)
			mdrivlib::RCC_Enable::I2C6_::set();

		i2c = reinterpret_cast<I2C_TypeDef *>(conf.periph);

		printf_("init i2c %08x\n", conf.periph);

		// see Figure 555. I2C initialization flowchart
		i2c->CR1 = i2c->CR1 & ~I2C_CR1_PE; // SW reset
		i2c->TIMINGR = 0x40505874;
		i2c->CR1 = i2c->CR1 | I2C_CR1_PE; // CR = 0x1; enable
		while (i2c->ISR & I2C_ISR_BUSY)
			;

		uint32_t address = 0x33;
		uint32_t num_bytes = 1;
		uint8_t data = 0x06; // Version

		// write
		i2c->CR2 = I2C_CR2_START | address << 1 | num_bytes << 16 | I2C_CR2_AUTOEND;
		i2c->TXDR = data;
		while (!(i2c->ISR & I2C_ISR_TXE))
			;

		// read
		I2C5->CR2 = I2C_CR2_START | address << 1 | num_bytes << 16 | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND;
		while (!(i2c->ISR & I2C_ISR_RXNE))
			;

		volatile uint8_t id = i2c->RXDR;
		printf_("Read: %d\n", id);
		__BKPT();
	}
};
