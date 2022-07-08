#pragma once
#include "drivers/leds.hh"

namespace STM32MP1Disco
{
using OrangeLED = Led<GPIO::H, PinNum::_7, LedActive::High>;
using LD7 = OrangeLED;

using BlueLED = Led<GPIO::D, PinNum::_11, LedActive::High>;
using LD8 = BlueLED;

// Note: Green and Red LEDs share a pin with User1Button and User2Button.
using GreenLED = Led<GPIO::A, PinNum::_14, LedActive::High>;
using LD5 = GreenLED;

using RedLED = Led<GPIO::A, PinNum::_13, LedActive::High>;
using LD6 = RedLED;

using RedLED2 = OrangeLED; // For compatibility with OSD32BRK board
using GreenLED2 = BlueLED; // For compatibility with OSD32BRK board

constexpr uint32_t ConsoleUART = UART4_BASE;
constexpr PinConf UartRX{GPIO::B, PinNum::_2, PinAF::AF_8};
constexpr PinConf UartTX{GPIO::G, PinNum::_11, PinAF::AF_6};

} // namespace STM32MP1Disco
