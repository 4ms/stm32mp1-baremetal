/*
 * codec_PCM3168.cc
 *
 * Author: Dan Green (danngreen1@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * See http://creativecommons.org/licenses/MIT/ for more information.
 *
 * -----------------------------------------------------------------------------
 */

#include "codec_PCM3168.hh"
#include "codec_PCM3168_registers.hh"
#include "stm32xx.h"
#include "util/bitfield.hh"

namespace mdrivlib
{

static constexpr bool DISABLE_I2C = false;

using namespace CodecPCM3168Register;

struct RegisterData {
	uint8_t reg_num;
	uint8_t value;
};

static RegisterData default_codec_init[] = {
	{ResetControl::Address, bitfield8(ResetControl::NoReset, ResetControl::NoResync, ResetControl::Single)},
	{DacControl1::Address, bitfield8(DacControl1::I2S_TDM_24bit, DacControl1::SlaveMode)},
	{DacControl2::Address, bitfield8(DacControl2::DacAllEnable)},
	{DacSoftMute::Address, bitfield8(DacSoftMute::NoDacMuted)},
	{DacAllAtten::Address, bitfield8(DacAtten::ZeroDB)},
	{AdcSamplingMode::Address, bitfield8(AdcSamplingMode::Single)},
	{AdcControl1::Address, bitfield8(AdcControl1::SlaveMode, AdcControl1::I2S_TDM_24bit)},
	{AdcControl2::Address, bitfield8(AdcControl2::AdcAllHPFDisabled)},
	{AdcSoftMute::Address, bitfield8(AdcSoftMute::NoAdcMuted)},
	{AdcAllAtten::Address, bitfield8(AdcAtten::ZeroDB)},
	{AdcInputType::Address, bitfield8(AdcInputType::Adc1SingleEnded, AdcInputType::AllAdcDifferential)},

};

CodecPCM3168::CodecPCM3168(I2CPeriph &i2c, const SaiConfig &saidef)
	: CodecBase{saidef}
	, i2c_(i2c)
	, samplerate_{saidef.samplerate}
	, reset_pin_{saidef.reset_pin, PinMode::Output}
	, I2C_address{static_cast<uint8_t>((I2C_BASE_ADDR + ((saidef.bus_address & 0b11) << 1)))} {
	reset_pin_.low();
}

CodecPCM3168::Error CodecPCM3168::init() {
	auto err = sai_.init();
	if (err != SaiTdmPeriph::SAI_NO_ERR)
		return CodecPCM3168::I2S_INIT_ERR;

	reset_pin_.high();

	HAL_Delay(1); // 3846 SYSCLKI cycles = 0.313ms
	return _write_all_registers(samplerate_);
}

uint32_t CodecPCM3168::get_samplerate() {
	return samplerate_;
}

void CodecPCM3168::start() {
	sai_.start();
}

CodecPCM3168::Error CodecPCM3168::_write_all_registers(uint32_t sample_rate) {
	CodecPCM3168::Error err;

	for (auto packet : default_codec_init) {
		err = _write_register(packet.reg_num, packet.value);
		if (err != CODEC_NO_ERR)
			return err;
	}
	return err;
}

CodecPCM3168::Error CodecPCM3168::_write_register(uint8_t reg_address, uint8_t reg_value) {
	uint8_t data[2] = {reg_address, reg_value};

	if constexpr (DISABLE_I2C)
		return CODEC_NO_ERR;

	auto err = i2c_.mem_write(I2C_address, reg_address, REGISTER_ADDR_SIZE, &data[1], 1);
	return (err == I2CPeriph::I2C_NO_ERR) ? CODEC_NO_ERR : CODEC_I2C_ERR;
}

} // namespace mdrivlib
