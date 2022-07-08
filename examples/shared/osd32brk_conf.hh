#pragma once
#include "drivers/leds.hh"

namespace OSD32BRK
{

using RedLED = Led<GPIO::Z, PinNum::_6, LedActive::Low>;
using GreenLED = Led<GPIO::Z, PinNum::_7, LedActive::Low>;
using RedLED2 = Led<GPIO::I, PinNum::_8, LedActive::Low>;
using GreenLED2 = Led<GPIO::I, PinNum::_9, LedActive::Low>;

using OrangeLED = GreenLED; // For compatibility with STM32MP1 Discovery board
using BlueLED = RedLED2;	// For compatibility with STM32MP1 Discovery board

constexpr uint32_t ConsoleUART = UART4_BASE;
constexpr PinConf UartRX{GPIO::B, PinNum::_2, PinAF::AF_8};
constexpr PinConf UartTX{GPIO::G, PinNum::_11, PinAF::AF_6};

} // namespace OSD32BRK
