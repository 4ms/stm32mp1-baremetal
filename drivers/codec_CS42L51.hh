/*
 * CodecCS42L51.cc
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

#pragma once

#include "codec.hh"
#include "i2c.hh"
#include "sai.hh"
#include <stdint.h>

namespace mdrivlib
{

class CodecCS42L51 : public ICodec {
public:
	enum Error {
		CODEC_NO_ERR = 0,
		CODEC_I2C_ERR,
		I2C_INIT_ERR,
		I2S_CLK_INIT_ERR,
		I2STX_INIT_ERR,
		I2SRX_INIT_ERR,
		I2STX_DMA_INIT_ERR,
		I2SRX_DMA_INIT_ERR,
		I2STX_XMIT_DMA_ERR,
		I2SRX_XMIT_DMA_ERR,
		SAI_DMA_IT_FE,
		SAI_DMA_IT_TE,
		SAI_DMA_IT_DME,
		INVALID_PARAM
	};

	CodecCS42L51(I2CPeriph &i2c, const SaiConfig &saidef);

	virtual void init();
	virtual void start();
	virtual uint32_t get_samplerate();
	virtual void set_txrx_buffers(uint8_t *tx_buf_ptr, uint8_t *rx_buf_ptr, uint32_t block_size);
	virtual void set_callbacks(std::function<void()> &&tx_complete_cb, std::function<void()> &&tx_half_complete_cb);

	Error init_at_samplerate(uint32_t sample_rate);
	Error power_down();

	DMA_HandleTypeDef *get_rx_dmahandle();
	DMA_HandleTypeDef *get_tx_dmahandle();

private:
	I2CPeriph &i2c_;
	SaiPeriph sai_;
	uint32_t samplerate_;

	Error _write_register(uint8_t RegisterAddr, uint16_t RegisterValue);
	Error _write_all_registers(uint32_t sample_rate);
	Error _reset();
	uint16_t _calc_samplerate(uint32_t sample_rate);

	// The address of the audio codec is 0x4A = 0b1001010x: 0x94 to write and 0x95 to read
	const static inline auto REGISTER_ADDR_SIZE = I2C_MEMADD_SIZE_8BIT;
};
} // namespace mdrivlib
