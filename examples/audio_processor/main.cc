#include "audio.hh"
#include "drivers/stm32xx.h"
#include "dual_fm_osc.hh"
#include "shared/drivers/delay.hh"
#include "shared/drivers/leds.hh"
#include "shared/drivers/uart.hh"
#include "shared/stm32disco_conf.hh"
#include "util/math.hh"
#include "util/oscs.hh"
#include "util/zip.hh"
#include <cstdint>

using AudioInBuffer = AudioStreamConf::AudioInBuffer;
using AudioOutBuffer = AudioStreamConf::AudioOutBuffer;

void main()
{
	// UI
	Uart<UART4_BASE> uart;
	uart.write("\r\n\r\nStarting Audio Processor\r\n");

	STM32MP1Disco::GreenLED green_led;
	STM32MP1Disco::RedLED red_led;
	STM32MP1Disco::User1Button button1;
	STM32MP1Disco::User2Button button2;
	red_led.on();
	green_led.on();

	// FX
	enum class FX { Passthrough, DualFMOsc, EndOfList };
	FX current_fx = FX::Passthrough;

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

	// AudioStream
	AudioStream audio;
	audio.set_process_function(passthrough);
	audio.start();

	while (true) {

		if (button1.is_just_pressed()) {
			switch (current_fx) {
				case FX::Passthrough:
					current_fx = FX::DualFMOsc;
					audio.set_process_function(passthrough);
					uart.write("Using DualFMOsc FX\r\n");
					red_led.on();
					green_led.off();
					break;

				case FX::DualFMOsc:
					current_fx = FX::Passthrough;
					audio.set_process_function(dual_fm_osc_process);
					uart.write("Using Passthrough FX\r\n");
					red_led.off();
					green_led.on();
					break;

				default:
					break;
			}
		}

		if (button2.is_just_pressed()) {
			uart.write("Current load: ");
			uart.write(audio.get_load_measurement());
			uart.write("\r\n");
		}
	}
}

extern "C" void aux_core_main() {}
