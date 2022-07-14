#pragma once
#include "drv/debounced_switch.hh"

namespace STM32MP1Disco
{
using User1Button = mdrivlib::DebouncedPin<mdrivlib::GPIO::A, 14, mdrivlib::PinPolarity::Inverted>;
using User2Button = mdrivlib::DebouncedPin<mdrivlib::GPIO::A, 13, mdrivlib::PinPolarity::Inverted>;
} // namespace STM32MP1Disco
