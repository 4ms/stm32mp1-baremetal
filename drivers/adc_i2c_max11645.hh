#pragma once
#include "adc_i2c_config_struct.hh"
#include "i2c.hh"
#include "pin.hh"
#include "stm32xx.h"
#include "util/bitfield.hh"

// tACQ >= 95 * (Rsource + Rin) * Cin
// 58 uS >= 95 * (25k + 2.5k) * 22pF, where 25k = max source impedance (100k pot with wiper in center)
// tACQ takes 12 clock cycles ==> 206kHz fSCL

// To enter HS mode (>400kHz), master must send 0000 1XXX  on bus to all devices
namespace mdrivlib
{
class ADC_I2C_MAX11645 {
public:
	ADC_I2C_MAX11645(I2CPeriph &i2c, const ADC_I2C_Config &conf)
		: _conf(conf)
		, _i2c{i2c} {
		//
	}

	enum Error {
		None,
		WriteConfigFailed,
		InvalidChannelID,
		ReadChannelFailed,
	};

	auto send_config() {
		// Todo: put these config/setup values in ADC_I2C_Config
		_data[0] = Setup::SETUP | Setup::REF_USE_EXT | Setup::INT_CLK;
		_data[1] = Config::CONFIG | Config::SINGLE_ENDED | Config::SCAN_SEL_CHAN_ONCE;
		auto err = _i2c.write(DEVICE_ADDRESS, _data, 2);

		return err == I2CPeriph::I2C_NO_ERR ? Error::None : Error::WriteConfigFailed;
	}

	auto select_channel(unsigned chan) {
		if (chan > 1)
			return Error::InvalidChannelID;

		if (chan != _cur_chan) {
			_cur_chan = chan;

			_data[0] = (_cur_chan ? Config::SEL_CHAN_1 : Config::SEL_CHAN_0) | Config::CONFIG | Config::SINGLE_ENDED |
					   Config::SCAN_SEL_CHAN_ONCE;
			auto err = _i2c.write_IT(DEVICE_ADDRESS, _data, 1);
			if (err != I2CPeriph::I2C_NO_ERR)
				return Error::ReadChannelFailed;
		}
		return Error::None;
	}

	auto read_blocking() {
		auto err = _i2c.read(DEVICE_ADDRESS, _data, 2);
		collect_last_reading();
		return err == I2CPeriph::I2C_NO_ERR ? Error::None : Error::ReadChannelFailed;
	}

	auto read() {
		auto err = _i2c.read_IT(DEVICE_ADDRESS, _data, 2);
		// Idea: could be useful to do: read_IT_with_cb(ADDR, _data, 2, [=](){ collect_last_reading(); });

		return err == I2CPeriph::I2C_NO_ERR ? Error::None : Error::ReadChannelFailed;
	}

	uint32_t collect_last_reading() {
		last_reading[_cur_chan] = ((_data[0] & 0x0F) << 8) | _data[1];
		return last_reading[_cur_chan];
	}

	uint32_t get_last_val() {
		return last_reading[_cur_chan];
	}

	uint32_t get_val(unsigned chan) {
		if (chan > 1)
			return 0;
		return last_reading[chan];
	}

private:
	const ADC_I2C_Config &_conf;
	I2CPeriph &_i2c;
	uint32_t last_reading[2]{0};
	uint8_t _data[2];
	unsigned _cur_chan = 0;

	static constexpr uint8_t DEVICE_ADDRESS_RAW = 0b0110110; // 0x36 unshifted, 0x6C/D shifted
	static constexpr uint8_t DEVICE_ADDRESS = DEVICE_ADDRESS_RAW << 1;
	static constexpr uint8_t HS_MASTER_CODE = 0b00001111;
	enum Setup : uint8_t {
		SETUP = (1 << 7),
		REF_USE_VDD = (0b00 << 5),
		REF_USE_EXT = (0b01 << 5),
		REF_USE_INT_DISCONNECT_REFPIN = (0b10 << 5),
		REF_USE_INT_AND_OUTPUT_ON_REFPIN = (0b11 << 5),
		REF_POWERUP_INT = (1 << 4),
		REF_POWERDOWN_INT = (0 << 4),
		EXT_CLK = (1 << 3),
		INT_CLK = (0 << 3),
		BIPOLAR = (1 << 2),
		UNIPOLAR = (0 << 2),
		RESET_CONFIG = (1 << 1),
		_dont_care = (1 << 0),
	};
	enum Config : uint8_t {
		CONFIG = (0 << 7),
		SCAN_UP_TO_SEL_CHAN = (0b00 << 5),
		SCAN_SEL_CHAN_8_TIMES = (0b01 << 5),
		SCAN_SEL_CHAN_ONCE = (0b11 << 5),
		_unused_scan = (0b10 << 5),
		_unused = (0b111 << 2),
		SEL_CHAN_0 = (0 << 1),
		SEL_CHAN_1 = (1 << 1),
		SINGLE_ENDED = (1 << 0),
		DIFFERENTIAL = (0 << 0)
	};
};
} // namespace mdrivlib
