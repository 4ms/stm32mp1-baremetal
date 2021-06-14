#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib {
namespace stm32h7x5 {
using RegisterDataT = uint32_t;

// Usage:
// target::SPI<1>::CR1<SPI_CR1_SPE>.read()

template <unsigned PeriphNum> struct SPI {
  static_assert(PeriphNum <= 6, "stm32h7x5 has only SPI1 - SPI6");
  static constexpr RegisterDataT BASE = PeriphNum == 1 ? SPI1_BASE : PeriphNum == 2 ? SPI2_BASE : PeriphNum == 3 ? SPI3_BASE : PeriphNum == 4 ? SPI4_BASE : PeriphNum == 5 ? SPI5_BASE : SPI6_BASE;
  template <RegisterDataT Mask = 0xFFFFFFFF> using CR1 = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, CR1), Mask>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using CR2 = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, CR2), Mask>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using CFG1 = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, CFG1), Mask>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using CFG2 = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, CFG2), Mask>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using IER = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, IER), Mask>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using SR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, SR), Mask>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using IFCR = RegisterBits<WriteOnly, BASE + offsetof(SPI_TypeDef, IFCR), Mask>;
  using TXDR = RegisterBits<WriteOnly, BASE + offsetof(SPI_TypeDef, TXDR), 0xFFFFFFFF>;
  using RXDR = RegisterBits<ReadOnly, BASE + offsetof(SPI_TypeDef, RXDR), 0xFFFFFFFF>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using CRCPOLY = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, CRCPOLY), Mask>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using TXCRC = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, TXCRC), Mask>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using RXCRC = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, RXCRC), Mask>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using UDRDR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, UDRDR), Mask>;
  template <RegisterDataT Mask = 0xFFFFFFFF> using I2SCFGR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, I2SCFGR), Mask>;
};
} // namespace stm32h7x5
} // namespace mdrivlib
