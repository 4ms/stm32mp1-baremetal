#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
using SYSCFG_FMP = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, PMCR), 0, 7>;
} // namespace mdrivlib
