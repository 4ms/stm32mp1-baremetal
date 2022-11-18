#pragma once
#include "stm32mp1xx.h"
#include <optional>
#include <span>

class I2C_Controller {
	I2C_TypeDef *i2c;
	uint32_t address;
	bool _is_init = false;

public:
	I2C_Controller(I2C_TypeDef *periph, uint32_t i2c_address)
		: i2c{periph}
		, address{i2c_address}
	{
		if (wait_not_busy())
			_is_init = true;
	}

	// Returns contents of an 8-bit register at the given address, or nullopt if failed
	// Returns false if failed
	std::optional<uint8_t> read_register_byte(uint8_t mem_address)
	{
		uint8_t data[1];
		if (read_register(mem_address, data))
			return data[0];
		else
			return std::nullopt;
	}

	// Reads a register at mem_address into the data parameter
	// Returns false if failed
	bool read_register(uint8_t mem_address, std::span<uint8_t> data)
	{
		if (!wait_not_busy())
			return false;

		if (request_register_read(mem_address))
			if (read(data))
				return true;
		return false;
	}

	// General read of any number of bytes sent by the other device
	// Returns false if failed
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

	// Write a byte to a register at mem_address
	// Returns false if failed
	bool write_register_byte(uint8_t mem_address, uint8_t val)
	{
		const uint32_t one_byte = 1;

		if (!wait_not_busy())
			return false;

		// Req Mem Write: xfer config reload, start, write
		i2c->CR2 = I2C_CR2_START | address << 1 | one_byte << 16 | I2C_CR2_RELOAD;

		if (!wait_on_flag_or_ack_or_timeout(I2C_ISR_TXIS))
			return false;

		i2c->TXDR = mem_address;

		if (!wait_on_flag_high_or_timeout(I2C_ISR_TCR))
			return false;

		// xfer config: size 1 autoend, no startstop
		i2c->CR2 = address << 1 | one_byte << 16 | I2C_CR2_AUTOEND;

		if (!wait_on_flag_or_ack_or_timeout(I2C_ISR_TXIS))
			return false;

		i2c->TXDR = val;

		if (!wait_on_flag_or_ack_or_timeout(I2C_ISR_STOPF))
			return false;

		i2c->ICR = I2C_ICR_STOPCF;

		return true;
	}

	bool is_init()
	{
		return _is_init;
	}

private:
	// Writes the register address, without generating a stop
	bool request_register_read(uint8_t mem_address)
	{
		constexpr uint32_t num_bytes = 1;

		i2c->CR2 = I2C_CR2_START | address << 1 | num_bytes << 16;
		if (!wait_on_flag_or_ack_or_timeout(I2C_ISR_TXIS))
			return false;

		i2c->TXDR = mem_address;

		if (!wait_on_flag_high_or_timeout(I2C_ISR_TC)) {
			return false;
			// pr_err("Timed out on TC flag\n");
		}

		return true;
	}

	bool wait_on_flag_high_or_timeout(uint32_t flag, uint32_t timeout = 0xFFFFFF)
	{
		while (!(i2c->ISR & flag) && timeout--)
			;
		return (timeout > 0) ? true : false;
	}

	bool wait_not_busy(uint32_t timeout = 0xFFFFFF)
	{
		while ((i2c->ISR & I2C_ISR_BUSY) && timeout--)
			;
		return (timeout > 0) ? true : false;
	}

	bool wait_on_flag_or_ack_or_timeout(uint32_t flag, uint32_t timeout = 0xFFFFFF)
	{
		while (timeout--) {
			uint32_t isr = i2c->ISR;
			if (isr & flag)
				return true;

			if (isr & I2C_ISR_NACKF)
				return false;
		}

		return false;
	}
};
