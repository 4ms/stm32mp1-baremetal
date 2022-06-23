#pragma once
#include "drivers/leds.hh"

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

using RedLED2 = OrangeLED; // For compatibility with OSD32BRK board
using GreenLED2 = BlueLED; // For compatibility with OSD32BRK board

constexpr uint32_t ConsoleUART = UART4_BASE;
constexpr PinConf UartRX{GPIO::B, 2, PinAF::AF_8};
constexpr PinConf UartTX{GPIO::G, 11, PinAF::AF_6};

} // namespace STM32MP1Disco
