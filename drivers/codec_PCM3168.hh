/*
 * CodecPCM3168.cc
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
#include <cstdint>

namespace mdrivlib
{

class CodecPCM3168 : public CodecBase {
public:
	enum Error {
		CODEC_NO_ERR = 0,
		CODEC_I2C_ERR,
		I2C_INIT_ERR,
		I2S_INIT_ERR,
	};

	CodecPCM3168(I2CPeriph &i2c, const SaiConfig &saidef);

	Error init();
	uint32_t get_samplerate();
	void start();

private:
	I2CPeriph &i2c_;
	uint32_t samplerate_;
	Pin reset_pin_;

	Error _write_register(uint8_t RegisterAddr, uint8_t RegisterValue);
	Error _write_all_registers(uint32_t sample_rate);

	const uint8_t I2C_address;
	static inline constexpr auto REGISTER_ADDR_SIZE = I2C_MEMADD_SIZE_8BIT;
};
} // namespace mdrivlib
