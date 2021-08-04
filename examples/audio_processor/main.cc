#include "audio_stream.hh"
#include "daisysp.h"
#include "drivers/stm32xx.h"
#include "shared/drivers/delay.hh"
#include "shared/drivers/leds.hh"
#include "shared/drivers/uart.hh"
#include "shared/stm32disco_conf.hh"
#include "util/math.hh"
#include "util/oscs.hh"
#include "util/zip.hh"
#include <cstdint>

// Synths:
#include "daisy_harm_osc.hh"
#include "daisy_reverb_osc.hh"
#include "dual_fm_osc.hh"

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

	// Synths
	enum class Synths { Passthrough, DualFMOsc, MonoTriOsc, HarmonicOsc, ReverbOsc };
	Synths current_synth = Synths::Passthrough;

	auto passthrough = [](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
		for (auto [in, out] : zip(in_buffer, out_buffer)) {
			out.chan[0] = in.chan[0];
			out.chan[1] = in.chan[1];
		}
	};

	// Dual FM Osc
	DualFMOsc<AudioStreamConf> dual_fm_osc;
	auto dual_fm_osc_process = [&dual_fm_osc](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
		dual_fm_osc.process(in_buffer, out_buffer);
	};

	// Tri + Sine osc
	TriangleOscillator<AudioStreamConf::SampleRate> tri_osc{400}; // 400Hz
	SineOscillator<AudioStreamConf::SampleRate> sine_osc{600};	  // 600Hz
	auto simple_osc_process = [&tri_osc, &sine_osc](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
		for (auto &out : out_buffer) {
			out.chan[0] = tri_osc.process() >> 8;
			out.chan[1] = sine_osc.process() >> 8;
		}
	};

	// Daisy Harmonic Osc + Env + Seqeuncer
	DaisyHarmonicExample<AudioStreamConf> harmonic_sequencer;
	auto harm_osc_process = [&harmonic_sequencer](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
		harmonic_sequencer.process(in_buffer, out_buffer);
	};

	// Daisy Harmonic Osc + Env + Seqeuncer
	DaisyReverbExample<AudioStreamConf> reverb_example;
	auto reverb_process = [&reverb_example](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
		reverb_example.process(in_buffer, out_buffer);
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

			switch (current_synth) {
				case Synths::Passthrough:
					current_synth = Synths::DualFMOsc;
					audio.set_process_function(dual_fm_osc_process);
					uart.write("Using DualFMOsc\r\n");
					blue_led.on();
					orange_led.off();
					break;

				case Synths::DualFMOsc:
					current_synth = Synths::MonoTriOsc;
					audio.set_process_function(simple_osc_process);
					uart.write("Using Dual Osc\r\n");
					blue_led.off();
					orange_led.on();
					break;

				case Synths::MonoTriOsc:
					current_synth = Synths::HarmonicOsc;
					audio.set_process_function(harm_osc_process);
					uart.write("Using Harmonic Osc\r\n");
					blue_led.on();
					orange_led.off();
					break;

				case Synths::HarmonicOsc:
					current_synth = Synths::ReverbOsc;
					audio.set_process_function(reverb_process);
					uart.write("Using Reverb\r\n");
					blue_led.off();
					orange_led.on();
					break;

				case Synths::ReverbOsc:
					current_synth = Synths::Passthrough;
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
