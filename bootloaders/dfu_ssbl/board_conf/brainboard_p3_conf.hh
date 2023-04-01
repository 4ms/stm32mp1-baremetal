#pragma once
#include "drivers/i2c_conf.hh"
#include "drivers/leds.hh"

namespace Board
{
using RedLED = Led<GPIO::G, PinNum::_12, LedActive::High>;	// DEBUG2
using GreenLED = Led<GPIO::A, PinNum::_9, LedActive::High>; // D0
using RedLED2 = RedLED;
using GreenLED2 = GreenLED;
using OrangeLED = RedLED2;
using BlueLED = GreenLED2;

constexpr uint32_t ConsoleUART = USART6_BASE;
constexpr PinConf UartRX{GPIO::C, PinNum::_7, PinAF::AF_7};
constexpr PinConf UartTX{GPIO::C, PinNum::_6, PinAF::AF_7};

constexpr PinConf DFUBootPin{GPIO::E, PinNum::_11}; // DEBUG3

namespace NORFlash
{
constexpr PinConf d2{GPIO::F, PinNum::_7, PinAF::AF_9};
constexpr PinConf d3{GPIO::F, PinNum::_6, PinAF::AF_9};
} // namespace NORFlash

namespace PMIC
{
constexpr bool HasSTPMIC = false;
constexpr I2C_Config I2C_config{};
} // namespace PMIC

constexpr uint32_t HSE_Clock_Hz = 24000000;
constexpr uint32_t MPU_MHz = 800;

} // namespace Board
