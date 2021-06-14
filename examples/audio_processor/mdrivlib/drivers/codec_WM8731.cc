/*
 * codec_i2c.c
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

#include "codec_WM8731.hh"
#include "codec_WM8731_registers.h"

// #ifdef STM32H7
static constexpr bool DISABLE_I2C = false;
// #else
// static constexpr bool DISABLE_I2C = true;
// #endif

namespace mdrivlib
{
using namespace _CodecWM8731;

uint16_t default_codec_init_data[] = {
	VOL_0dB,   // Reg 00: Left Line In
	VOL_0dB,   // Reg 01: Right Line In
	HPVOL_0dB, // Reg 02: Left Headphone out
	HPVOL_0dB, // Reg 03: Right Headphone out
	(MUTEMIC   // Reg 04: Analog Audio Path Control (maximum attenuation on sidetone, sidetone disabled, DAC selected,
			 // Mute Mic, no bypass)
	 | INSEL_line | DACSEL | SIDEATT_neg6dB),
	(DEEMPH_disable // Reg 05: Digital Audio Path Control: HPF, De-emp at 48kHz on DAC, do not soft mute dac
	 | ADCHPFEnable),
	(PD_MIC | PD_OSC | PD_CLKOUT), // Reg 06: Power Down Control (Clkout, Osc, Mic Off) 0x062
	(format_24b					   // Reg 07: Digital Audio Interface Format (24-bit, slave)
	 | format_I2S),
	0x000, // Reg 08: Sampling Control (USB_NORM=Normal, BOSR=256x, MCLK=12.288MHz, SR=48k)
	0x001  // Reg 09: Active Control
};

CodecWM8731::CodecWM8731(I2CPeriph &i2c, const SaiConfig &saidef)
	: i2c_(i2c)
	, sai_{saidef}
	, samplerate_{saidef.samplerate} {
}

void CodecWM8731::init() {
	init_at_samplerate(samplerate_);
	sai_.init();
}

void CodecWM8731::set_txrx_buffers(uint8_t *tx_buf_ptr, uint8_t *rx_buf_ptr, uint32_t block_size) {
	sai_.set_txrx_buffers(tx_buf_ptr, rx_buf_ptr, block_size);
}

void CodecWM8731::set_callbacks(std::function<void()> &&tx_complete_cb, std::function<void()> &&tx_half_complete_cb) {
	sai_.set_callbacks(std::move(tx_complete_cb), std::move(tx_half_complete_cb));
}

uint32_t CodecWM8731::get_samplerate() {
	return samplerate_;
}
void CodecWM8731::start() {
	sai_.start();
}

CodecWM8731::Error CodecWM8731::init_at_samplerate(uint32_t new_sample_rate) {
	HAL_Delay(10);
	auto err = _reset();
	if (err != CODEC_NO_ERR)
		return err;
	return _write_all_registers(new_sample_rate);
}

CodecWM8731::Error CodecWM8731::_reset() {
	return _write_register(WM8731_REG_RESET, 0);
}

CodecWM8731::Error CodecWM8731::_write_all_registers(uint32_t sample_rate) {
	CodecWM8731::Error err;

	for (uint8_t i = 0; i < WM8731_NUM_REGS; i++) {
		if (i != WM8731_REG_SAMPLE_CTRL)
			err = _write_register(i, default_codec_init_data[i]);
		else
			err = _write_register(i, default_codec_init_data[i] | _calc_samplerate(sample_rate));

		if (err != CODEC_NO_ERR)
			return err;
	}
	return err;
}

uint16_t CodecWM8731::_calc_samplerate(uint32_t sample_rate) {
	if (sample_rate == 48000)
		return SR_NORM_48K;
	else if (sample_rate == 44100)
		return SR_NORM_44K;
	else if (sample_rate == 32000)
		return SR_NORM_32K;
	else if (sample_rate == 88200)
		return SR_NORM_88K;
	else if (sample_rate == 96000)
		return SR_NORM_96K;
	else if (sample_rate == 8000)
		return SR_NORM_8K;
	else
		return 0;
}

CodecWM8731::Error CodecWM8731::_write_register(uint8_t reg_address, uint16_t reg_value) {
	uint8_t Byte1 = ((reg_address << 1) & 0xFE) | ((reg_value >> 8) & 0x01);
	uint8_t Byte2 = reg_value & 0xFF;
	uint8_t data[2] = {Byte1, Byte2};

	if constexpr (DISABLE_I2C)
		return CODEC_NO_ERR;

	auto err = i2c_.write(CODEC_ADDRESS, data, 2);
	// auto err = i2c_.mem_write(CODEC_ADDRESS, Byte1, REGISTER_ADDR_SIZE, &Byte2, 1);
	return (err == I2CPeriph::I2C_NO_ERR) ? CODEC_NO_ERR : CODEC_I2C_ERR;
}

CodecWM8731::Error CodecWM8731::power_down(void) {
	return _write_register(WM8731_REG_POWERDOWN, 0xFF); // Power Down enable all
}

// FixMe: Why is this exposed?
DMA_HandleTypeDef *CodecWM8731::get_rx_dmahandle() {
	return sai_.get_rx_dmahandle();
}
DMA_HandleTypeDef *CodecWM8731::get_tx_dmahandle() {
	return sai_.get_tx_dmahandle();
}
} // namespace mdrivlib
