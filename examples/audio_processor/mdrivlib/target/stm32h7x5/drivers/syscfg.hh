#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
namespace stm32h7x5
{
using SYSCFG_FMP = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, PMCR), 0, 7>;
} // namespace stm32h7x5
} // namespace mdrivlib
