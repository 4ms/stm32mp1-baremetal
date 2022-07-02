#pragma once
#include "drivers/i2c_conf.hh"
#include "drivers/pin.hh"
#include "drivers/rcc.hh"
#include "print_messages.h"
#include <span>

class I2C_Controller {
	I2C_TypeDef *i2c;
	uint32_t address;

public:
	I2C_Controller(uint32_t i2c_address, const I2C_Config &conf)
		: address{i2c_address}
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

		// see Figure 555. I2C initialization flowchart
		i2c->CR1 = i2c->CR1 & ~I2C_CR1_PE; // SW reset
		i2c->TIMINGR = 0x40505874;
		i2c->CR1 = i2c->CR1 | I2C_CR1_PE; // CR = 0x1; enable
		while (i2c->ISR & I2C_ISR_BUSY)
			;
	}

	bool write(const std::span<const uint8_t> data)
	{
		// write
		i2c->CR2 = I2C_CR2_START | address << 1 | data.size_bytes() << 16 | I2C_CR2_AUTOEND;
		i2c->TXDR = *data.data();
		while (!(i2c->ISR & I2C_ISR_TXE))
			;

		return true;
	}

	bool read(std::span<uint8_t> data)
	{
		// TODO
		const uint32_t num_bytes = data.size_bytes();
		auto d = data.begin();
		uint32_t bytes_left = num_bytes;

		I2C5->CR2 = I2C_CR2_START | address << 1 | num_bytes << 16 | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND;

		while (bytes_left--) {
			while (!(i2c->ISR & I2C_ISR_RXNE))
				;

			uint8_t ret = i2c->RXDR;
			*d++ = ret;
		}

		return true;
	}

	uint8_t read_byte()
	{
		const uint32_t num_bytes = 1;
		I2C5->CR2 = I2C_CR2_START | address << 1 | num_bytes << 16 | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND;
		while (!(i2c->ISR & I2C_ISR_RXNE))
			;

		uint8_t ret = i2c->RXDR;
		return ret;
	}
};
