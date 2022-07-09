#include "audio_stream.hh"
#include "drivers/stm32xx.h"
#include "stm32disco_buttons_conf.hh"
#include "uart.hh"

#include "synth_list.hh"

void main()
{
	// UI
	Uart<UART4_BASE> uart;
	uart.write("\r\n\r\nStarting Audio Processor\r\n");
	uart.write("Press User1 button to select a synth\r\n");

	// BlueLED blue_led;
	STM32MP1Disco::User1Button button1;
	STM32MP1Disco::User2Button button2;

	SynthList synths;
	int current_synth = SynthList::Synths::DualFMOscillators;

	uart.write("Using Synth: ");
	uart.write(synths.name[current_synth]);
	uart.write("\r\n");

	AudioStream audio;
	audio.start(synths.process_func[current_synth]);

	constexpr uint32_t LoadTimerStartingValue = 5000000;
	uint32_t display_load_timer = LoadTimerStartingValue;

	while (true) {
		button1.update();
		button2.update();

		// Select synth
		if (button1.is_just_pressed()) {
			current_synth++;
			if (current_synth == SynthList::NumSynths)
				current_synth = 0;

			audio.set_process_function(synths.process_func[current_synth]);

			uart.write("Using Synth: ");
			uart.write(synths.name[current_synth]);
			uart.write("\r\n");

			// Let the new synth run for a bit, so we get an accurate load measurement
			display_load_timer = LoadTimerStartingValue;
		}

		if (button2.is_just_pressed()) {
			// we can do something here... send a signal to a synth, or adjust a parameter?
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
