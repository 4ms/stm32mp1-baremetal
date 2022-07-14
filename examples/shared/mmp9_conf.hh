#pragma once
#include "drivers/i2c_conf.hh"
#include "drivers/leds.hh"

namespace MMp9
{
using RedLED = Led<GPIO::E, PinNum::_2, LedActive::High>;
using GreenLED = Led<GPIO::D, PinNum::_3, LedActive::High>;
using RedLED2 = Led<GPIO::B, PinNum::_14, LedActive::High>;
using GreenLED2 = Led<GPIO::E, PinNum::_1, LedActive::High>;

using OrangeLED = RedLED2;
using BlueLED = Led<GPIO::A, PinNum::_14, LedActive::Low>;

constexpr uint32_t ConsoleUART = USART6_BASE;
constexpr PinConf UartRX{GPIO::C, PinNum::_7, PinAF::AF_7};
constexpr PinConf UartTX{GPIO::C, PinNum::_6, PinAF::AF_7};

namespace PMIC
{
constexpr bool HasSTPMIC = false;
constexpr I2C_Config I2C_config{};
} // namespace PMIC

constexpr uint32_t HSE_Clock_Hz = 24000000;
constexpr uint32_t MPU_MHz = 800;

} // namespace MMp9
