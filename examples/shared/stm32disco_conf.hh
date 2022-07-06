#pragma once
#include "drivers/i2c_conf.hh"
#include "drivers/leds.hh"

namespace STM32MP1Disco
{
using OrangeLED = Led<GPIO::H, 7, LedActive::High>;
using LD7 = OrangeLED;

using BlueLED = Led<GPIO::D, 11, LedActive::High>;
using LD8 = BlueLED;

// Note: Green and Red LEDs share a pin with User1Button and User2Button.
using GreenLED = Led<GPIO::A, 14, LedActive::High>;
using LD5 = GreenLED;

using RedLED = Led<GPIO::A, 13, LedActive::High>;
using LD6 = RedLED;

using RedLED2 = OrangeLED; // For compatibility with OSD32BRK board
using GreenLED2 = BlueLED; // For compatibility with OSD32BRK board

constexpr uint32_t ConsoleUART = UART4_BASE;
constexpr PinConf UartRX{GPIO::B, PinNum::_2, PinAF::AF_8};
constexpr PinConf UartTX{GPIO::G, PinNum::_11, PinAF::AF_6};

namespace PMIC
{
constexpr bool HasSTPMIC = true;
constexpr I2C_Config I2C_config{
	.periph = I2C_Periph::I2C4_,
	.sda_pin = {GPIO::Z, PinNum::_5, PinAF::AF_6},
	.scl_pin = {GPIO::Z, PinNum::_4, PinAF::AF_6},
};
} // namespace PMIC
} // namespace STM32MP1Disco
