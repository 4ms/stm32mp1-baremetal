#pragma once
#include "drivers/leds.hh"
#include "lib/mdrivlib/drivers/debounced_switch.hh"

namespace STM32MP1Disco
{

using OrangeLED = Led<GPIOH_BASE, 7, LedActive::High>;
using LD7 = OrangeLED;

using BlueLED = Led<GPIOD_BASE, 11, LedActive::High>;
using LD8 = BlueLED;

// Note: Green and Red LEDs share a pin with User1Button and User2Button.
using GreenLED = Led<GPIOA_BASE, 14, LedActive::High>;
using LD5 = GreenLED;

using RedLED = Led<GPIOA_BASE, 13, LedActive::High>;
using LD6 = RedLED;

using User1Button = mdrivlib::DebouncedPin<mdrivlib::GPIO::A, 14, mdrivlib::PinPolarity::Inverted>;
using User2Button = mdrivlib::DebouncedPin<mdrivlib::GPIO::A, 13, mdrivlib::PinPolarity::Inverted>;

} // namespace STM32MP1Disco
