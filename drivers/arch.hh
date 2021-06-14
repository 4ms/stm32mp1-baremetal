#pragma once

namespace mdrivlib
{
enum class SupportedTargets {
	stm32mp1_ca7,
	stm32mp1_cm4,
	stm32h7x5,
	stm32f7,
	stm32f4,
};
}

//
// STM32MP1
//
#if defined(STM32MP1) /*&& defined(CORE_CA7)*/
namespace mdrivlib::stm32mp1::core_a7
{
constexpr bool TARGET_FOUND = true;
constexpr auto TYPE = SupportedTargets::stm32mp1_ca7;
} // namespace mdrivlib::stm32mp1::core_a7
namespace target = mdrivlib::stm32mp1::core_a7;

// #elif defined(STM32MP1) && defined(CORE_CM4)
// namespace mdrivlib
// {
// namespace stm32mp1
// {
// namespace core_m4
// {
// constexpr bool TARGET_FOUND = true;
// constexpr auto TYPE = SupportedTargets::stm32mp1_cm4;
// } // namespace core_m4
// } // namespace stm32mp1
// } // namespace mdrivlib
// namespace target = mdrivlib::stm32mp1::core_m4;

//
// STM32H745 and STM32H755
//
#elif defined(STM32H745xx) || defined(STM32H755xx)
#ifndef STM32H7x5
#define STM32H7x5
#endif
namespace mdrivlib
{
namespace stm32h7x5
{
constexpr bool TARGET_FOUND = true;
constexpr auto TYPE = SupportedTargets::stm32h7x5;
} // namespace stm32h7x5
} // namespace mdrivlib
namespace target = mdrivlib::stm32h7x5;

//
// STM32F746
//
#elif defined(STM32F746xx)
namespace mdrivlib
{
namespace stm32f7xx
{
#ifndef STM32F7xx
#define STM32F7xx
#endif
constexpr bool TARGET_FOUND = true;
constexpr auto TYPE = SupportedTargets::stm32mp1;
} // namespace stm32f7xx
} // namespace mdrivlib
namespace target = mdrivlib::stm32f7xx;
#endif

static_assert(target::TARGET_FOUND == true, "No target specified");
