#pragma once
#include "i2c.hh"
#include "interrupt.hh"
#include "pin.hh"
#include <cstdint>

namespace mdrivlib
{
struct GPIO_expander_conf {
	uint8_t addr;
	// Pin int_pin;
	// IRQType IRQn;
	uint16_t config;

	enum PinDirection { Output = 0, Input = 1 };
	static constexpr uint16_t AllInputs() {
		return 0xFFFF;
	}
	static constexpr uint16_t AllOutputs() {
		return 0x0000;
	}
	static constexpr uint16_t Config(PinDirection pindirs[16]) {
		uint16_t c = 0;
		for (unsigned i = 0; i < 16; i++)
			c |= (pindirs[i] == Input) ? (1 << i) : 0;
		return c;
	}
};

struct GPIOExpander {
	static constexpr uint8_t NumChans = 16;
	enum Error {
		None,
		WriteConfigFailed,
		InvalidChannelID,
		ReadFailed,
		WriteFailed,
	};

	GPIOExpander(I2CPeriph &i2c, const GPIO_expander_conf &conf)
		: _conf{conf}
		, _device_addr(conf.addr << 1)
		, _i2c{i2c} {
	}

	auto start() {
		_data[0] = ConfigPort0;
		_data[1] = static_cast<uint8_t>(_conf.config & 0x00FF);
		_data[2] = static_cast<uint8_t>(_conf.config >> 8);
		auto err = _i2c.write(_device_addr, _data, 3);
		if (err != I2CPeriph::I2C_NO_ERR)
			return Error::WriteConfigFailed;
		return Error::None;
	}

	// Todo:
	// Create an EXTI interrupt to respond to chip telling us there's a pin change
	// void start_ISR(std::function<void(void)> &&cb) {
	// 	callback = std::move(cb);
	// 	InterruptManager::registerISR(_conf.IRQn, _conf.pri1, _conf.pri2, [&]() { callback(); });
	// }

	auto prepare_read() {
		_data[0] = InputPort0;
		auto err = _i2c.write_IT(_device_addr, _data, 1);
		return err == I2CPeriph::I2C_NO_ERR ? Error::None : Error::WriteFailed;
	}

	auto read_pins() {
		auto err = _i2c.read_IT(_device_addr, _data, 2);
		return err == I2CPeriph::I2C_NO_ERR ? Error::None : Error::ReadFailed;
	}

	auto set_output_values(uint16_t mask) {
		// Todo: test this on hardware
		_data[0] = OutputPort0;
		_data[1] = mask & 0x00FF;
		_data[2] = mask >> 8;
		auto err = _i2c.write_IT(_device_addr, _data, 3);
		return err == I2CPeriph::I2C_NO_ERR ? Error::None : Error::WriteFailed;
	}

	uint16_t collect_last_reading() {
		last_reading = _data[1] << 8 | _data[0];
		return last_reading;
	}

	bool get_pin_state(unsigned pinnum) {
		if (pinnum >= NumChans)
			return false;
		return last_reading & (1 << pinnum) ? true : false;
	}

private:
	const GPIO_expander_conf &_conf;
	const uint8_t _device_addr;
	I2CPeriph &_i2c;
	uint16_t last_reading;
	uint8_t _data[3];
	uint16_t _data16;
	unsigned _cur_port = 0;

	enum Registers : uint8_t {
		InputPort0 = 0x00,
		InputPort1 = 0x01,
		OutputPort0 = 0x02,
		OutputPort1 = 0x03,
		PolarityPort0 = 0x04,
		PolarityPort1 = 0x05,
		ConfigPort0 = 0x06,
		ConfigPort1 = 0x07,
	};
	static constexpr uint8_t Output = 0;
	static constexpr uint8_t Input = 1;
};

} // namespace mdrivlib
