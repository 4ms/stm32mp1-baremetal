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

constexpr uint32_t ConsoleUART = UART4_BASE;
constexpr PinConf UartRX{GPIO::B, 2, PinAF::AF_8};
constexpr PinConf UartTX{GPIO::G, 11, PinAF::AF_6};

} // namespace OSD32BRK
