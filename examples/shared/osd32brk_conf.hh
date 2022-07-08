#pragma once
#include "drivers/i2c_conf.hh"
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

namespace PMIC
{
constexpr bool HasSTPMIC = true;
constexpr I2C_Config I2C_config{
	.periph = I2C_Periph::I2C4_,
	.sda_pin = {GPIO::Z, PinNum::_5, PinAF::AF_6},
	.scl_pin = {GPIO::Z, PinNum::_4, PinAF::AF_6},
};
} // namespace PMIC

constexpr uint32_t HSE_Clock_Hz = 24000000;
constexpr uint32_t MPU_MHz = 650;

} // namespace OSD32BRK
