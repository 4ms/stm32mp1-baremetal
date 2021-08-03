#include "pca9685_led_driver.hh"

namespace mdrivlib
{

PCA9685Driver::PCA9685Driver(I2CPeriph &i2c, uint32_t num_chips, uint32_t *frame_buf)
	: i2c_periph_(i2c)
	, num_chips_(num_chips)
	, frame_buf_{frame_buf} {
}

// Reset chip registers
LEDDriverError PCA9685Driver::start() {
	uint8_t driverAddr;
	LEDDriverError err;

	for (driverAddr = 0; driverAddr < num_chips_; driverAddr++) {
		err = reset_chip(driverAddr);
		if (err != LEDDriverError::None)
			return (err);
		// Todo return failed driver#: | ((driverAddr+1)<<4));
	}

	return LEDDriverError::None;
}

// Enables I2C transfers in background using interrupts
// (not implemented in I2CPeriph yet)
void PCA9685Driver::start_it_mode() {
	start();
	i2c_periph_.enable_IT(2, 2);
}

LEDDriverError PCA9685Driver::write_partial_chip(uint16_t chip_num, uint8_t num_leds) {
	uint16_t driverAddr = I2C_BASE_ADDRESS | (chip_num << 1);
	auto err = i2c_periph_.mem_write_IT(
		driverAddr, REG_LED0, REGISTER_ADDR_SIZE, reinterpret_cast<uint8_t *>(frame_buf_), num_leds * 4); 
	if (err != I2CPeriph::I2C_NO_ERR)
		return LEDDriverError::I2C_XMIT_TIMEOUT;

	return LEDDriverError::None;
}

LEDDriverError PCA9685Driver::write_chip(uint16_t chip_num) {

	uint16_t driverAddr = I2C_BASE_ADDRESS | (chip_num << 1);
	auto err = i2c_periph_.mem_write_IT(
		driverAddr, REG_LED0, REGISTER_ADDR_SIZE, reinterpret_cast<uint8_t *>(frame_buf_), kNumLedsPerChip * 4);
	if (err != I2CPeriph::I2C_NO_ERR)
		return LEDDriverError::I2C_XMIT_TIMEOUT;

	return LEDDriverError::None;
}

// Sets the brightness value of a single LED
// led_element_number==0 is PWM pin 0 of PCA9685 chip with address 0
// led_element_number==1 is PWM pin 1 of PCA9685 chip with address 0
// ...
// led_element_number==15 is PWM pin 15 of PCA9685 chip with address 0
// led_element_number==16 is PWM pin 0 of PCA9685 chip with address 1
// ...
LEDDriverError PCA9685Driver::set_single_led(uint8_t led_element_number, uint16_t brightness) {
	uint8_t driver_addr;
	uint8_t data[5];

	if (led_element_number >= (num_chips_ * 16))
		return LEDDriverError::BAD_LED_PARAM;

	driver_addr = (led_element_number / 16);
	led_element_number = led_element_number - (driver_addr * 16);

	driver_addr = I2C_BASE_ADDRESS | (driver_addr << 1);

	uint8_t registers_per_led = 4;
	data[0] = REG_LED0 + (led_element_number * registers_per_led);
	data[1] = 0; // on-time = 0
	data[2] = 0;
	data[3] = brightness & 0xFF; // off-time = brightness
	data[4] = brightness >> 8;

	// Todo: write_IT()
	auto err = i2c_periph_.write(driver_addr, data, 5);

	return (err == I2CPeriph::Error::I2C_NO_ERR) ? LEDDriverError::None : LEDDriverError::SET_LED_ERR;
}

// Sets color of an RGB LED
// Assumes 5 RGB LEDs per driver chip, connected to pins 0-14
// Example:
// led_number==0 refers to PWM pins 0,1,2 of PCA9685 chip with address 0
// led_number==1 refers to PWM pins 3,4,5 of PCA9685 chip with address 0
// ...
// led_number==5 refers to PWM pins 0,1,2 of PCA9685 chip with address 1
// ..
// led_number==9 refers to PWM pins 12,13,14 of PCA9685 chip with address 1
//
// Note: This function cannot access pin 15
LEDDriverError PCA9685Driver::set_rgb_led(uint8_t led_number, uint16_t c_red, uint16_t c_green, uint16_t c_blue) {
	uint8_t driverAddr;
	uint8_t data[13];

	if (led_number >= (num_chips_ * 5))
		return LEDDriverError::BAD_LED_PARAM;

	driverAddr = get_chip_num(led_number);
	led_number = led_number - (driverAddr * 5);

	// 12 registers per LED (4 registers per LED element) = 12*16 registers per
	// driver
	data[0] = REG_LED0 + (led_number * 12);
	data[1] = 0; // on-time = 0
	data[2] = 0;
	data[3] = c_red & 0xFF; // off-time = brightness
	data[4] = c_red >> 8;

	data[5] = 0; // on-time = 0
	data[6] = 0;
	data[7] = c_green & 0xFF; // off-time = brightness
	data[8] = c_green >> 8;

	data[9] = 0; // on-time = 0
	data[10] = 0;
	data[11] = c_blue & 0xFF; // off-time = brightness
	data[12] = c_blue >> 8;

	driverAddr = I2C_BASE_ADDRESS | (driverAddr << 1);

	// Todo: write_IT()
	auto err = i2c_periph_.write(driverAddr, data, 13);

	return (err == I2CPeriph::Error::I2C_NO_ERR) ? LEDDriverError::None : LEDDriverError::I2C_XMIT_TIMEOUT;
}

LEDDriverError PCA9685Driver::reset_chip(uint8_t driverAddr) {
	LEDDriverError err;

	err = write_register(driverAddr, REG_MODE1, 0b00000000); // clear sleep mode
	if (err != LEDDriverError::None)
		return err;
	HAL_Delay(1);

	err = write_register(driverAddr, REG_MODE1, 0b10000000); // start reset mode
	if (err != LEDDriverError::None)
		return err;
	HAL_Delay(1);

	err = write_register(driverAddr, REG_MODE1, 0b00100000); // auto increment
	if (err != LEDDriverError::None)
		return err;
	err = write_register(driverAddr, REG_MODE2, 0b00010001); // INVERT=1, OUTDRV=0, OUTNE=01
	if (err != LEDDriverError::None)
		return err;

	return LEDDriverError::None;
}

LEDDriverError PCA9685Driver::write_register(uint8_t driverAddr, uint8_t registerAddr, uint8_t registerValue) {
	uint8_t data[2] = {registerAddr, registerValue};

	driverAddr = I2C_BASE_ADDRESS | (driverAddr << 1);

	auto err = i2c_periph_.write(driverAddr, data, 2);

	return (err == I2CPeriph::Error::I2C_NO_ERR) ? LEDDriverError::None : LEDDriverError::I2C_XMIT_TIMEOUT;
}

// returns led element number of the red element of the given RGB LED id (green
// is red + 1, blue = red + 2)
uint8_t PCA9685Driver::get_red_led_element_id(uint8_t rgb_led_id) {
	return (rgb_led_id * 3) + (rgb_led_id / kNumRGBLEDsPerChip);
}

uint8_t PCA9685Driver::get_chip_num(uint8_t rgb_led_id) {
	return (rgb_led_id / kNumRGBLEDsPerChip);
}

} // namespace mdrivlib
