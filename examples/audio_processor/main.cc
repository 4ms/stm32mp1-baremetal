#include "audio_codec_conf.hh"
#include "drivers/codec_CS42L51.hh"
#include "drivers/i2c.hh"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
// #include "interrupt.hh"
#include "stm32xx.h"
#include <cstdint>

FPin<GPIO::G, 9> CodecResetPin;

void main()
{
	// UART
	Uart<UART4_BASE> uart;
	uart.write("\r\n\r\nStarting Audio Processor\r\n");

	// LEDs
	Led<GPIOI_BASE, 8, LedActive::Low> red_led1;
	Led<GPIOI_BASE, 9, LedActive::Low> green_led1;

	red_led1.on();
	green_led1.on();

	mdrivlib::I2CPeriph codec_i2c{i2c_conf};
	mdrivlib::CodecCS42L51 codec{codec_i2c, sai_conf};

	codec.init();
}
extern "C" void aux_core_main() {}
