#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{
using RegisterDataT = uint32_t;
// Usage:
// SPI_<1>::CR1<SPI_CR1_SPE>.read()

template<unsigned PeriphNum>
struct SPI_ {
	static_assert(PeriphNum <= 6, "stm32mp1 has only SPI1 - SPI6");

	static constexpr RegisterDataT BASE = PeriphNum == 1 ? SPI1_BASE
										: PeriphNum == 2 ? SPI2_BASE
										: PeriphNum == 3 ? SPI3_BASE
										: PeriphNum == 4 ? SPI4_BASE
										: PeriphNum == 5 ? SPI5_BASE
														 : SPI6_BASE;

	// clang-format off
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
	template <RegisterDataT Mask = 0xFFFFFFFF> using HWCFGR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, HWCFGR), Mask>;
	template <RegisterDataT Mask = 0xFFFFFFFF> using VERR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, VERR), Mask>;
	template <RegisterDataT Mask = 0xFFFFFFFF> using IPIDR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, IPIDR), Mask>;
	template <RegisterDataT Mask = 0xFFFFFFFF> using SIDR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, SIDR), Mask>;
	// clang-format on
};
} // namespace mdrivlib
