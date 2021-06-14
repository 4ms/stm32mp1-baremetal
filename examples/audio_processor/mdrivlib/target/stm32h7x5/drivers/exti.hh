#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
namespace stm32h7x5
{
namespace EXTI_
{
using Pin0 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[0]), 0, 3>;
using Pin1 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[0]), 4, 3>;
using Pin2 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[0]), 8, 3>;
using Pin3 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[0]), 12, 3>;
using Pin4 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[1]), 0, 3>;
using Pin5 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[1]), 4, 3>;
using Pin6 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[1]), 8, 3>;
using Pin7 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[1]), 12, 3>;
using Pin8 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[2]), 0, 3>;
using Pin9 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[2]), 4, 3>;
using Pin10 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[2]), 8, 3>;
using Pin11 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[2]), 12, 3>;
using Pin12 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[3]), 0, 3>;
using Pin13 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[3]), 4, 3>;
using Pin14 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[3]), 8, 3>;
using Pin15 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[3]), 12, 3>;

constexpr uint8_t PortA = 0;
constexpr uint8_t PortB = 1;
constexpr uint8_t PortC = 2;
constexpr uint8_t PortD = 3;
constexpr uint8_t PortE = 4;
constexpr uint8_t PortF = 5;
constexpr uint8_t PortG = 6;
constexpr uint8_t PortH = 7;
constexpr uint8_t PortI = 8;
constexpr uint8_t PortJ = 9;
constexpr uint8_t PortK = 10;

// using EXTI_IMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, IMR1), 0xFFFFFFFF>;
// using EXTI_EMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EMR1), 0xFFFFFFFF>;
// using EXTI_RTSR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RTSR1), 0xFFFFFFFF>;
// using EXTI_FTSR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FTSR1), 0xFFFFFFFF>;

// using EXTI_PR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, PR1), 0xFFFFFFFF>;

// Rising/Falling edge trigger selection:
using RTSR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RTSR1), 0x003FFFFF>;
using FTSR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FTSR1), 0x003FFFFF>;
// Todo:
// SWIER1
// SWIER2
// SWIER3
// RTSR2
// FTSR2
// RTSR3
// FTSR3
// D3PMR1
// D3PCR1L
// D3PCR1H
// D3PMR2
// D3PCR2L
// D3PCR2H
// D3PMR3
// D3PCR3L
// D3PCR3H
//

// Interrupt and Event enables for Core 1 and Core 2:
using IMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, IMR1), 0xFFFFFFFF>;
using IMR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, IMR2), 0xFFFFFFFF>;
using IMR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, IMR3), 0xFFFFFFFF>;

using EMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EMR1), 0xFFFFFFFF>;
using EMR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EMR2), 0xFFFFFFFF>;
using EMR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EMR3), 0xFFFFFFFF>;

using C2IMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2IMR1), 0xFFFFFFFF>;
using C2IMR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2IMR2), 0xFFFFFFFF>;
using C2IMR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2IMR3), 0xFFFFFFFF>;

using C2EMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2EMR1), 0xFFFFFFFF>;
using C2EMR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2EMR2), 0xFFFFFFFF>;
using C2EMR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2EMR3), 0xFFFFFFFF>;

// Rising/Falling trigger pending (read, then write 1 to clear)
using PR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, PR1), 0x003FFFFF>;
using C2PR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2PR1), 0x003FFFFF>;
// Todo:
// C1PR2
// C1PR3
// C2PR2
// C2PR3

// Rising edge and falling edge trigger selection for GPIO pins:
template<uint32_t PinNum>
using PinRisingTrig = RegisterBits<ReadWrite, RTSR1::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using PinFallingTrig = RegisterBits<ReadWrite, FTSR1::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using PinTrigPending = RegisterBits<ReadWrite, PR1::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using PinInterruptMask = RegisterBits<ReadWrite, IMR1::BaseAddress, (1 << PinNum)>;

} // namespace EXTI_
} // namespace stm32h7x5
} // namespace mdrivlib
