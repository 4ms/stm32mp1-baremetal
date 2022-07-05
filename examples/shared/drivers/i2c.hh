#pragma once
#include "drivers/i2c_conf.hh"
#include "drivers/pin.hh"
#include "drivers/rcc.hh"
#include "print_messages.h"
#include <optional>
#include <span>

class I2C_Controller {
	I2C_TypeDef *i2c;
	uint32_t address;

public:
	I2C_Controller(uint32_t i2c_address, const I2C_Config &conf)
		: address{i2c_address}
	{
		conf.scl_pin.init(PinMode::Alt, PinOType::OpenDrain);
		conf.sda_pin.init(PinMode::Alt, PinOType::OpenDrain);

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

		// TODO: allow user to set CKSRC with I2C_Config
		RCC->I2C46CKSELR = RCC_I2C46CKSELR_I2C46SRC_1; // HSI

		i2c = reinterpret_cast<I2C_TypeDef *>(conf.periph);

		i2c->CR1 = i2c->CR1 & ~I2C_CR1_PE;
		i2c->TIMINGR = 0x6020070A;
		i2c->CR1 = i2c->CR1 | I2C_CR1_PE;

		if (!wait_on_flag_low_or_timeout(I2C_ISR_BUSY))
			pr_err("I2C could not be init\n");
	}

	// Returns contents of an 8-bit register at the given address, or nullopt if failed
	std::optional<uint8_t> read_register_byte(uint8_t mem_address)
	{
		if (request_register_read(mem_address)) {
			return read_byte();
		}
		return std::nullopt;
	}

	// Reads a register at mem_address into the data parameter
	// Returns false if failed
	bool read_register(uint8_t mem_address, std::span<uint8_t> data)
	{
		if (request_register_read(mem_address))
			if (read(data))
				return true;
		return false;
	}

	// General read a number of bytes sent by the other device
	bool read(std::span<uint8_t> data)
	{
		const uint32_t num_bytes = data.size_bytes();
		if (num_bytes == 0)
			return false;

		i2c->CR2 = I2C_CR2_START | address << 1 | num_bytes << 16 | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND;

		uint32_t bytes_left = num_bytes;
		auto d = data.begin();
		do {
			if (!wait_on_flag_high_or_timeout(I2C_ISR_RXNE))
				break;
			*d++ = i2c->RXDR;
			bytes_left--;
		} while (bytes_left);

		return bytes_left == 0 ? true : false;
	}

	// Read a single byte sent by the other device
	std::optional<uint8_t> read_byte()
	{
		const uint32_t num_bytes = 1;
		i2c->CR2 = I2C_CR2_START | address << 1 | num_bytes << 16 | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND;
		if (!wait_on_flag_high_or_timeout(I2C_ISR_RXNE)) {
			pr_err("Timed out on RXNE flag\n");
			return std::nullopt;
		}

		uint8_t ret = i2c->RXDR;
		return ret;
	}

	bool write_register_byte(uint8_t mem_address, uint8_t val)
	{
		const uint32_t one_byte = 1;

		i2c->CR2 = I2C_CR2_START | address << 1 | one_byte << 16 | I2C_CR2_RELOAD;
		if (!wait_on_flag_or_ack_or_timeout(I2C_ISR_TXIS))
			return false;

		i2c->TXDR = mem_address;
		if (!wait_on_flag_high_or_timeout(I2C_ISR_TCR))
			return false;

		// autoend, no startstop
		i2c->CR2 = address << 1 | one_byte << 16 | I2C_CR2_AUTOEND;
		if (!wait_on_flag_or_ack_or_timeout(I2C_ISR_TXIS))
			return false;

		i2c->TXDR = val;
		if (!wait_on_flag_or_ack_or_timeout(I2C_ISR_STOPF))
			return false;

		return true;
	}

	// General write, without setting any address, etc. Is this useful?
	bool write(const std::span<const uint8_t> data)
	{
		log("CR2: start | autoend | addr:%08x, | bytes:%zx\n", address << 1, data.size_bytes());
		i2c->CR2 = I2C_CR2_START | address << 1 | data.size_bytes() << 16 | I2C_CR2_AUTOEND;
		i2c->TXDR = *data.data();

		return wait_on_flag_high_or_timeout(I2C_ISR_TXE);
	}

private:
	// Writes the register address, without generating a stop
	bool request_register_read(uint8_t mem_address)
	{
		constexpr uint32_t num_bytes = 1;
		// printf_("CR2: start | addr:%08x, | bytes:%x\n", address << 1, num_bytes);

		i2c->CR2 = I2C_CR2_START | address << 1 | num_bytes << 16;
		if (!wait_on_flag_or_ack_or_timeout(I2C_ISR_TXIS))
			return false;

		i2c->TXDR = mem_address;

		if (!wait_on_flag_high_or_timeout(I2C_ISR_TC))
			printf_("Timed out on TC flag\n");

		return true;
	}

	bool wait_on_flag_high_or_timeout(uint32_t flag, uint32_t timeout = 0xFFFFFFFF)
	{
		while (!(i2c->ISR & flag) && timeout--)
			;
		return (timeout > 0) ? true : false;
	}

	bool wait_on_flag_low_or_timeout(uint32_t flag, uint32_t timeout = 0xFFFFFFFF)
	{
		while ((i2c->ISR & flag) && timeout--)
			;
		return (timeout > 0) ? true : false;
	}

	bool wait_on_flag_or_ack_or_timeout(uint32_t flag, uint32_t timeout = 0xFFFFFFFF)
	{
		while (timeout--) {
			uint32_t isr = i2c->ISR;
			if (isr & flag)
				return true;

			if (isr & I2C_ISR_NACKF) {
				printf_("NACKF\n");
				return false;
			}
		}
		printf_("Timed out waiting on flag\n");
		return false;
	}
};
