#pragma once
#include "pin.hh"

namespace mdrivlib
{
// Todo: add clock division and maybe other QSPI conf
struct QSPIFlashConfig {
	PinNoInit io0;
	PinNoInit io1;
	PinNoInit io2;
	PinNoInit io3;
	PinNoInit clk;
	PinNoInit cs;

	uint32_t clock_division = 4;
	uint32_t IRQ_pri = 2;
	uint32_t IRQ_subpri = 2;
};
} // namespace mdrivlib
