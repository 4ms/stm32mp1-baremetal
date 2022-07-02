#pragma once
#include "drivers/i2c_conf.hh"
#include "drivers/leds.hh"

namespace OSD32BRK
{

using RedLED = Led<GPIO::Z, 6, LedActive::Low>;
using GreenLED = Led<GPIO::Z, 7, LedActive::Low>;
using RedLED2 = Led<GPIO::I, 8, LedActive::Low>;
using GreenLED2 = Led<GPIO::I, 9, LedActive::Low>;

using OrangeLED = GreenLED; // For compatibility with STM32MP1 Discovery board
using BlueLED = RedLED2;	// For compatibility with STM32MP1 Discovery board

constexpr uint32_t ConsoleUART = UART4_BASE;
constexpr PinConf UartRX{GPIO::B, 2, PinAF::AF_8};
constexpr PinConf UartTX{GPIO::G, 11, PinAF::AF_6};

namespace PMIC
{
constexpr bool HasSTPMIC = true;
constexpr I2C_Config I2C_config{
	.periph = I2C_Periph::I2C4_,
	.sda_pin = {GPIO::Z, 5, PinAF::AF_6},
	.scl_pin = {GPIO::Z, 4, PinAF::AF_6},
};
} // namespace PMIC

} // namespace OSD32BRK
