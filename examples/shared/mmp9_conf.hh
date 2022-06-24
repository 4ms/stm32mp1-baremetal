#pragma once
#include "drivers/leds.hh"

namespace MMp9
{
using RedLED = Led<GPIO::E, 2, LedActive::High>;
using GreenLED = Led<GPIO::D, 3, LedActive::High>;
using RedLED2 = Led<GPIO::B, 14, LedActive::High>;
using GreenLED2 = Led<GPIO::E, 1, LedActive::High>;

using OrangeLED = RedLED2;
using BlueLED = Led<GPIO::A, 14, LedActive::Low>;

constexpr uint32_t ConsoleUART = USART6_BASE;
constexpr PinConf UartRX{GPIO::C, 7, PinAF::AF_7};
constexpr PinConf UartTX{GPIO::C, 6, PinAF::AF_7};

} // namespace MMp9
