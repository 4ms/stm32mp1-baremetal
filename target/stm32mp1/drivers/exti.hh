#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib::EXTI_
{
using Pin0 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[0]), 0, 8>;
using Pin1 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[0]), 8, 8>;
using Pin2 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[0]), 16, 8>;
using Pin3 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[0]), 24, 8>;
using Pin4 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[1]), 0, 8>;
using Pin5 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[1]), 8, 8>;
using Pin6 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[1]), 16, 8>;
using Pin7 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[1]), 24, 8>;
using Pin8 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[2]), 0, 8>;
using Pin9 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[2]), 8, 8>;
using Pin10 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[2]), 16, 8>;
using Pin11 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[2]), 24, 8>;
using Pin12 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[3]), 0, 8>;
using Pin13 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[3]), 8, 8>;
using Pin14 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[3]), 16, 8>;
using Pin15 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[3]), 24, 8>;

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
constexpr uint8_t PortZ = 11;

// Rising/Falling edge trigger selection:
using RTSR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RTSR1), 0x0001FFFF>;
using RTSR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RTSR2), 0x0>; // not used in MP1
using RTSR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RTSR3), (0b11 << 9) | (1 << 4) | (0b11 << 1)>;
using FTSR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FTSR1), 0x0001FFFF>;
using FTSR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FTSR2), 0x0>; // not used in MP1
using FTSR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FTSR3), (0b11 << 9) | (1 << 4) | (0b11 << 1)>;

// Interrupt and Event enables for Core 1 and Core 2:
using C1IMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C1IMR1), 0xFFFFFFFF>;
using C1IMR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C1IMR2), 0xFFFFFFFF>;
using C1IMR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C1IMR3), 0xFFFFFFFF>;
using C2IMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2IMR1), 0xFFFFFFFF>;
using C2EMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2EMR1), 0xFFFFFFFF>;
using C2IMR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2IMR2), 0xFFFFFFFF>;
using C2EMR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2EMR2), 0xFFFFFFFF>;
using C2IMR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2IMR3), 0xFFFFFFFF>;
using C2EMR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2EMR3), 0xFFFFFFFF>;

// Rising/Falling trigger pending (read, then write 1 to clear)
using RPR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RPR1), 0x0001FFFF>;
using RPR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RPR2), 0x0>; // not used in MP1
using RPR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RPR3), (0b11 << 9) | (1 << 4) | (0b11 << 1)>;
using FPR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FPR1), 0x0001FFFF>;
using FPR2 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FPR2), 0x0>; // not used in MP1
using FPR3 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FPR3), (0b11 << 9) | (1 << 4) | (0b11 << 1)>;

// Rising edge and falling edge trigger selection for GPIO pins:
template<uint32_t PinNum>
using PinRisingTrig = RegisterBits<ReadWrite, RTSR1::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using PinFallingTrig = RegisterBits<ReadWrite, FTSR1::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using PinRisingTrigPending = RegisterBits<ReadWrite, RPR1::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using PinFallingTrigPending = RegisterBits<ReadWrite, FPR1::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using Core1PinInterruptMask = RegisterBits<ReadWrite, C1IMR1::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using Core2PinInterruptMask = RegisterBits<ReadWrite, C2IMR1::BaseAddress, (1 << PinNum)>;
} // namespace mdrivlib::EXTI_
