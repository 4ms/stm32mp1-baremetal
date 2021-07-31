#pragma once
#include "drivers/arch.hh"
#include "drivers/hsem.hh"
#include "drivers/interrupt.hh"
#include "drivers/pinchange.hh"
#include "stm32xx.h"

// Set up your pins here:
#include "debug.hh" //This is where I define my debug pins, but you can include your own file
#include "drivers/pin.hh"
using TestPin0 = FPin<Debug::Pin0::Gpio_v, Debug::Pin0::PinNum_v, PinMode::Output>;
using TestPin1 = FPin<Debug::Pin1::Gpio_v, Debug::Pin1::PinNum_v, PinMode::Output>;
using TestPin2 = FPin<Debug::Pin2::Gpio_v, Debug::Pin2::PinNum_v, PinMode::Output>;
using TestPin3 = FPin<Debug::Pin3::Gpio_v, Debug::Pin3::PinNum_v, PinMode::Output>;

namespace mdrivlib
{

// Test nested interrupts.
// Requires you to choose some pins that you can view on a data analyzer or scope: Debug::Pin0 through Debug::Pin3
// They should be of type FPin<>
//
// Debug Pin 0 should emit a long high pulse
// While Debug Pin 0 is still high, Debug Pin 1 should go high for a short pulse
// Assert: Debug Pin 0 goes high, then low while Debug Pin 1 is still high
// Assert: Debug Pin 2 toggle for at least a few milliseconds immediately after Debug Pin 1 goes low
//
// Additional tests: Swap priority of two ISRs and verify Debug Pin 0 goes high and low before Debug Pin 1 goes high.
//
// Debug Pin 0: high indicates Semaphore ISR running, low = not running
// red_LED1 on = Semaphore ISR reached
// Debug Pin 1: high indicates Pinchange ISR is running, low = not running
// red LED2 on = pinchange isr reached
// Debug Pin 2: toggling indicates non-ISR level code is running
// Debug Pin 3: used to create a PinChange interrupt
static inline void test_nesting_isr() {
	// Reset debug pins to a known state
	TestPin3::low();
	TestPin2::low();
	TestPin1::low();
	TestPin0::low();

	constexpr uint32_t TestSemaphoreID = 1;
	constexpr IRQType TestSemaphoreIRQn = HSEM_IT1_IRQn;

	// Create callback for low-priority ISR (Semaphore unlocked)
	// which triggers the high-priority ISR from within it
	InterruptManager::registerISR(TestSemaphoreIRQn, 3, 3, [&]() {
		if (HWSemaphore<TestSemaphoreID>::is_ISR_triggered_and_enabled()) {
			for (int i = 0; i < 800; i++) {
				if (i == 50)
					TestPin3::high(); // Trigger higher priority ISR
				TestPin0::high();
			}
			TestPin0::low();
			HWSemaphore<TestSemaphoreID>::clear_ISR();
		}
	});
	// Clear pending interrupts and enable the channel
	HWSemaphore<TestSemaphoreID>::disable_channel_ISR();
	HWSemaphore<TestSemaphoreID>::clear_ISR();
	target::System::enable_irq(TestSemaphoreIRQn);
	HWSemaphore<TestSemaphoreID>::enable_channel_ISR();
	HWSemaphore<TestSemaphoreID>::lock();

	// Create callback for high-priority ISR (Pinchange ISR = Debug 3 pin goes high)
	PinChangeInterrupt pcint;
	const PinChangeConfig pcconf = {
		.pin = TestPin3::PinNum_v,
		.port = TestPin3::Gpio_v,
		.on_rising_edge = true,
		.on_falling_edge = false,
		.priority1 = 0,
		.priority2 = 0,
	};
	pcint.init(pcconf, []() {
		TestPin1::high();
		TestPin1::low();
	});
	// Enable interrupt
	pcint.start();

	// Trigger the low-priority ISR
	HWSemaphore<TestSemaphoreID>::unlock();

	// Loop for a while
	int i = 1000000;
	while (i--) {
		TestPin2::low();
		HAL_Delay(100);
		TestPin2::high();
		HAL_Delay(100);
	}

	target::System::disable_irq(TestSemaphoreIRQn);
	HWSemaphore<TestSemaphoreID>::disable_channel_ISR();
	HWSemaphore<TestSemaphoreID>::clear_ISR();
	pcint.stop();
}
} // namespace mdrivlib
