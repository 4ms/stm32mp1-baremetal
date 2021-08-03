#include "audio_stream.hh"
#include "daisy_harm_osc.hh"
#include "daisysp.h"
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

	STM32MP1Disco::BlueLED blue_led;
	STM32MP1Disco::OrangeLED orange_led;
	STM32MP1Disco::User1Button button1;
	STM32MP1Disco::User2Button button2;
	blue_led.on();
	orange_led.on();

	// FX
	enum class FX { Passthrough, DualFMOsc, MonoTriOsc, HarmonicOsc };
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

	TriangleOscillator<48000> tri_osc{400};
	SineOscillator<48000> sine_osc{600};
	auto simple_osc_process = [&tri_osc, &sine_osc](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
		for (auto &out : out_buffer) {
			out.chan[0] = tri_osc.process() >> 8;
			out.chan[1] = sine_osc.process() >> 8;
		}
	};

	DaisyHarmonicExample<AudioStreamConf> harmonic_sequencer;
	auto harm_osc_process = [&harmonic_sequencer](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
		harmonic_sequencer.process(in_buffer, out_buffer);
	};

	// AudioStream
	AudioStream audio;
	audio.set_process_function(passthrough);
	uart.write("Using Passthrough FX\r\n");
	audio.start();

	uint32_t display_load_timer = 1;

	while (true) {
		button1.update();
		button2.update();

		// Change FX
		if (button1.is_just_pressed()) {
			switch (current_fx) {
				case FX::Passthrough:
					current_fx = FX::DualFMOsc;
					audio.set_process_function(dual_fm_osc_process);
					uart.write("Using DualFMOsc\r\n");
					blue_led.on();
					orange_led.off();
					break;

				case FX::DualFMOsc:
					current_fx = FX::MonoTriOsc;
					audio.set_process_function(simple_osc_process);
					uart.write("Using Dual Osc\r\n");
					blue_led.off();
					orange_led.on();
					break;

				case FX::MonoTriOsc:
					current_fx = FX::HarmonicOsc;
					audio.set_process_function(harm_osc_process);
					uart.write("Using Harmonic Osc\r\n");
					blue_led.on();
					orange_led.off();
					break;

				case FX::HarmonicOsc:
					current_fx = FX::Passthrough;
					audio.set_process_function(passthrough);
					uart.write("Using Passthrough\r\n");
					blue_led.on();
					orange_led.on();
					break;
			}
			display_load_timer = 1000000;
		}

		if (button2.is_just_pressed()) {
			// Do something!
		}

		if (display_load_timer == 1) {
			uart.write("Current load: ");
			uart.write(audio.get_load_measurement());
			uart.write("%\r\n\r\n");
		}
		if (display_load_timer)
			display_load_timer--;
	}
}

extern "C" void aux_core_main() {}
