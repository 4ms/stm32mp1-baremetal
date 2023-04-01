#pragma once
#include "drivers/pinconf.hh"

namespace mdrivlib
{
struct QSPIFlashConfig {
	PinConf io0{};
	PinConf io1{};
	PinConf io2{};
	PinConf io3{};
	PinConf clk{};
	PinConf cs{};

	uint32_t clock_division = 4;
	uint32_t IRQ_pri = 2;
	uint32_t IRQ_subpri = 2;

	uint32_t flash_size_bytes = 0x40000;

	enum AddressBits { AddrBits24 = 24, AddrBits32 = 32 };
	AddressBits flash_size_address_bits = AddrBits24;

	enum ChipID { IS25L, S25FLxxxL, W25Q128JV };
	ChipID chip_id = IS25L;

	enum IOMode { SingleSPI, DualSPI, QuadSPI };
	IOMode io_mode = QuadSPI;

	enum Bank { Bank1, Bank2 };
	Bank bank = Bank1;
};

// struct QSPIFlashConf {
// 	static constexpr PinDef io0{};
// 	static constexpr PinDef io1{};
// 	static constexpr PinDef io2{};
// 	static constexpr PinDef io3{};
// 	static constexpr PinDef clk{};
// 	static constexpr PinDef cs{};

// 	static constexpr uint32_t clock_division = 4;
// 	static constexpr uint32_t IRQ_pri = 2;
// 	static constexpr uint32_t IRQ_subpri = 2;

// 	static constexpr uint32_t flash_size_bytes = 0x40000;
// 	enum AddressBits { AddrBits24 = 24, AddrBits32 = 32 };
// 	static constexpr AddressBits flash_size_address_bits = AddrBits24;

// 	enum IOMode { SDR, DDR, QPI };
// 	static constexpr IOMode io_mode = QPI;
// };
} // namespace mdrivlib
