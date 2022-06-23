#pragma once
#include "drivers/leds.hh"

namespace OSD32BRK
{

using RedLED = Led<GPIO::Z, 6, LedActive::Low>;
using GreenLED = Led<GPIO::Z, 7, LedActive::Low>;
using RedLED2 = Led<GPIO::I, 8, LedActive::Low>;
using GreenLED2 = Led<GPIO::I, 9, LedActive::Low>;

using OrangeLED = RedLED2; // For compatibility with STM32MP1 Discovery board
using BlueLED = GreenLED2; // For compatibility with STM32MP1 Discovery board
} // namespace OSD32BRK
