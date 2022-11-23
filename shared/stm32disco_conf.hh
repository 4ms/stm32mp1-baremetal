#pragma once
#include "drivers/i2c_conf.hh"
#include "drivers/leds.hh"
#include "stm32mp1xx.h"

namespace STM32MP1Disco
{
using OrangeLED = Led<GPIO::H, PinNum::_7, LedActive::High>;
using LD7 = OrangeLED;

using BlueLED = Led<GPIO::D, PinNum::_11, LedActive::High>;
using LD8 = BlueLED;

// Note: Green and Red LEDs share a pin with User1Button and User2Button.
using GreenLED = Led<GPIO::A, PinNum::_14, LedActive::Low>;
using LD5 = GreenLED;

using RedLED = Led<GPIO::A, PinNum::_13, LedActive::Low>;
using LD6 = RedLED;

using RedLED2 = OrangeLED; // For compatibility with OSD32BRK board
using GreenLED2 = BlueLED; // For compatibility with OSD32BRK board

constexpr uint32_t ConsoleUART = UART4_BASE;
constexpr PinConf UartRX{GPIO::B, PinNum::_2, PinAF::AF_8};
constexpr PinConf UartTX{GPIO::G, PinNum::_11, PinAF::AF_6};

// Specify whether the board has a PMIC power management IC, and what I2C bus it's on
namespace PMIC
{
constexpr bool HasSTPMIC = true;
constexpr I2C_Config I2C_config{
	.periph = I2C_Periph::I2C4_,
	.sda_pin = {GPIO::Z, PinNum::_5, PinAF::AF_6},
	.scl_pin = {GPIO::Z, PinNum::_4, PinAF::AF_6},
};
} // namespace PMIC

// Specify whether the board has a USBC Interface IC, and what I2C bus it's on
namespace USBC_Interface
{
constexpr bool HasSTUSB1600 = true;
constexpr I2C_Config I2C_config = PMIC::I2C_config;
}; // namespace USBC_Interface

constexpr uint32_t HSE_Clock_Hz = 24000000;
constexpr uint32_t MPU_MHz = 650;
} // namespace STM32MP1Disco
