#include "audio.hh"
#include "drivers/stm32xx.h"
#include "dual_fm_osc.hh"
#include "shared/drivers/leds.hh"
#include "shared/drivers/uart.hh"
#include "util/math.hh"
#include "util/oscs.hh"
#include "util/zip.hh"
#include <cstdint>

using AudioInBuffer = AudioStreamConf::AudioInBuffer;
using AudioOutBuffer = AudioStreamConf::AudioOutBuffer;
using AudioInFrame = AudioStreamConf::AudioInFrame;
using AudioOutFrame = AudioStreamConf::AudioOutFrame;

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

	AudioStream audio;

	auto passthrough = [](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
		for (auto [in, out] : zip(in_buffer, out_buffer)) {
			out.chan[0] = in.chan[0];
			out.chan[1] = in.chan[1];
		}
	};

	// Select one of these:
	// audio.set_process_function(passthrough);
	audio.set_process_function(DualFMOsc<AudioStreamConf>::process);

	audio.start();

	while (true) {
		// blink the lights
		//
		// switch Processor if button0 pressed
		//
		// print out the processor load if button1 pressed
		//
	}
}

extern "C" void aux_core_main() {}
