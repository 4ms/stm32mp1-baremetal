#pragma once
#include "drivers/leds.hh"

namespace STM32MP1Disco {

using GreenLED = Led<GPIOA_BASE, 14, LedActive::Low>;
using LD4 = GreenLED;

using RedLED = Led<GPIOA_BASE, 13, LedActive::Low>;
using LD6 = RedLED;

using OrangeLED = Led<GPIOH_BASE, 7, LedActive::Low>;
using LD7 = OrangeLED;

using BlueLED = Led<GPIOD_BASE, 11, LedActive::Low>;
using LD8 = BlueLED;

}
