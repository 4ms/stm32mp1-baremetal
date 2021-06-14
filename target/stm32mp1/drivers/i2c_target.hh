#pragma once
#include "drivers/i2c_config_struct.hh"

namespace mdrivlib {
namespace stm32mp1 {
namespace core_a7 {
struct I2C {
  static void init(const I2CConfig &defs) {}
};
} // namespace core_a7
} // namespace stm32mp1
} // namespace mdrivlib
