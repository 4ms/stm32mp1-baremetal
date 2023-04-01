#pragma once
#include "board_conf.hh"
#include "norflash/qspi_flash_struct.hh"

constexpr mdrivlib::QSPIFlashConfig qspi_flash_conf = {
	.io0 = {GPIO::F, PinNum::_8, PinAF::AF_10},
	.io1 = {GPIO::F, PinNum::_9, PinAF::AF_10},
	.io2 = Board::NORFlash::d2,
	.io3 = Board::NORFlash::d3,
	.clk = {GPIO::F, PinNum::_10, PinAF::AF_9},
	.cs = {GPIO::B, PinNum::_6, PinAF::AF_10},

	/* QSPI freq = QUADSPICLK /(1 + ClockPrescaler) = 266 MHz/(2+1) = 88 Mhz */
	.clock_division = 3,
	.IRQ_pri = 2,
	.IRQ_subpri = 2,

	.flash_size_bytes = 0x0100'0000,
	.flash_size_address_bits = mdrivlib::QSPIFlashConfig::AddrBits24,

	.chip_id = mdrivlib::QSPIFlashConfig::W25Q128JV, // was S25FL

	.io_mode = mdrivlib::QSPIFlashConfig::QuadSPI,

	.bank = mdrivlib::QSPIFlashConfig::Bank1,
};
