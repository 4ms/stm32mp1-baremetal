#pragma once

#include "conf/led_driver_conf.hh"
#include "hal_callback.hh"
#include "i2c.hh"
#include "led_driver.hh"
#include "stm32xx.h"

namespace mdrivlib
{

// Todo: finish this and use it
class PCA9685Driver {
public:
	constexpr static int kNumLedsPerChip = 16;
	constexpr static int kNumRGBLEDsPerChip = 5;

	// returns the address within the frame buffer of the requested led and chip
	static constexpr uint32_t *get_buf_addr(uint32_t *frame_buf, uint32_t chip_num, uint32_t led_num) {
		return &(frame_buf[chip_num * kNumLedsPerChip + led_num]);
	}

	PCA9685Driver(I2CPeriph &i2c, uint32_t num_chips, uint32_t *frame_buf);

	LEDDriverError start();
	void start_it_mode();
	LEDDriverError start_dma_mode();
	LEDDriverError set_rgb_led(uint8_t led_number, uint16_t c_red, uint16_t c_green, uint16_t c_blue);
	LEDDriverError set_single_led(uint8_t led_element_number, uint16_t brightness);
	uint8_t get_red_led_element_id(uint8_t rgb_led_id);
	uint8_t get_chip_num(uint8_t rgb_led_id);
	LEDDriverError write_chip(uint16_t chip_num);
	LEDDriverError write_partial_chip(uint16_t chip_num, uint8_t num_bits);

protected:
	I2CPeriph &i2c_periph_;
	const uint32_t num_chips_;
	LEDDriverError led_error_;
	uint32_t *frame_buf_;

	LEDDriverError write_register(uint8_t driverAddr, uint8_t registerAddr, uint8_t registerValue);
	LEDDriverError reset_chip(uint8_t driverAddr);

	constexpr static inline uint32_t REG_MODE1 = 0x00;
	constexpr static inline uint32_t REG_MODE2 = 0x01;
	constexpr static inline uint32_t REG_LED0 = 0x06;
	constexpr static inline uint32_t I2C_BASE_ADDRESS = 0b10000000; // 0x40 unshifted, 0x80/1 shifted
	constexpr static inline auto REGISTER_ADDR_SIZE = I2C_MEMADD_SIZE_8BIT;
};

} // namespace mdrivlib
