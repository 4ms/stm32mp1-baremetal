#include "audio.hh"
#include "drivers/stm32xx.h"
#include "dual_fm_osc.hh"
#include "shared/disco_conf.hh"
#include "shared/drivers/delay.hh"
#include "shared/drivers/leds.hh"
#include "shared/drivers/uart.hh"
#include "util/math.hh"
#include "util/oscs.hh"
#include "util/zip.hh"
#include <cstdint>

using AudioInBuffer = AudioStreamConf::AudioInBuffer;
using AudioOutBuffer = AudioStreamConf::AudioOutBuffer;

void main()
{
	// UART
	Uart<UART4_BASE> uart;
	uart.write("\r\n\r\nStarting Audio Processor\r\n");

	STM32MP1Disco::GreenLED green_led;
	STM32MP1Disco::RedLED red_led;
	red_led.on();
	green_led.on();

	AudioStream audio;

	auto passthrough = [](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
		for (auto [in, out] : zip(in_buffer, out_buffer)) {
			out.chan[0] = in.chan[0];
			out.chan[1] = in.chan[1];
		}
	};

	DualFMOsc<AudioStreamConf> dual_fm_osc;

	auto dual_fm_osc_process = [&dual_fm_osc](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
		dual_fm_osc.process(in_buffer, out_buffer);
	};

	// Select one of these:
	audio.set_process_function(passthrough);
	audio.set_process_function(dual_fm_osc_process);

	audio.start();

	while (true) {

		red_led.on();
		Delay::cycles(5000000);
		red_led.off();

		green_led.on();
		Delay::cycles(5000000);
		green_led.off();

		// blink the lights
		//
		// switch Processor if button0 pressed
		//
		// print out the processor load if button1 pressed
		//
	}
}

extern "C" void aux_core_main() {}
