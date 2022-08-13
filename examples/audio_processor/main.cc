#include "audio_stream.hh"
#include "drivers/uart.hh"
#include "drv/pmic.hh"
#include "register_access.hh"
#include "stm32disco_buttons_conf.hh"
#include "stm32mp1xx.h"

#include "synth_list.hh"

using namespace STM32MP1Disco;

namespace mdrivlib {
    /**
     * Independent watchdog (IWDG) key register.
     * 0xAAAA: refreshes the watchdog (reloads the RL[11:0] value into the IWDCNT down-counter).
     * This value must be written by software at regular intervals, or the watchdog will generate
     * a reset when the counter reaches 0.
     */
    using IWDG2_KR = RegisterBits<ReadWrite,
                                IWDG2_BASE + offsetof(IWDG_TypeDef, KR),
                                0xAAAA>;
} // namespace mdrivlib

void main()
{
	// Codec is powered by LDO1 on the Disco board
	STPMIC1 pmic{I2C4};
	pmic.setup_ldo1();

	// UI
	Uart<UART4_BASE> uart;
	uart.writeln("\r\n\r\nStarting Audio Processor");
	uart.writeln("Press User1 button to select a synth");

	User1Button button1;
	User2Button button2;

	SynthList synths(SynthList::Synths::HarmonicOsc);

	uart.write("Using Synth: ");
	uart.writeln(synths.current_synth_name());

	AudioStream audio;
	audio.start(synths.get_current_process());

	constexpr uint32_t LoadTimerStartingValue = 5000000;
	uint32_t display_load_timer = LoadTimerStartingValue;

	while (true) {
		// IWDG2 is set to timeout every 32 seconds by u-boot.
		mdrivlib::IWDG2_KR::set();

		button1.update();
		button2.update();

		// Select synth
		if (button1.is_just_pressed()) {
			// Loop through our synths.
			synths.next_synth();

			audio.set_process_function(synths.get_current_process());

			uart.write("Using Synth: ");
			uart.writeln(synths.current_synth_name());

			// Let the new synth run for a bit, so we get an accurate load measurement
			display_load_timer = LoadTimerStartingValue;
		}

		if (button2.is_just_pressed()) {
			// we can do something here... send a signal to a synth, or adjust a parameter?
		}

		if (display_load_timer == 1) {
			uart.write("Current load: ");
			uart.write(audio.get_load_measurement());
			uart.writeln("%\r\n");
		}
		if (display_load_timer != 0) {
			display_load_timer--;
		}
	}
}

extern "C" void aux_core_main() {}
