#pragma once
#include "irq_ctrl.h"

namespace mdrivlib
{
struct System {
	// Binary Point is set to 4 in IRQ_Initialize().
	// From Table 3-2 in arm_gic_architecture_specifiation, the group and sub-group are:
	//     ggg.sssss
	// ARM CA7 uses 5-bit priority, and is left-aligned, so bottom 3 sss bits are ignored.
	// So we have:
	//     ggg.ssXXX
	//
	// So, pri1 is upper 3 bits (value can be 0-7), pri2 is lower 2 bits (value can be 0-3)
	//
	static void set_irq_priority(IRQn_Type irqn, uint32_t pri1, uint32_t pri2) {
		auto pri = ((pri1 & 0b111) << 5) | (pri2 & 0b11 << 3);
		GIC_SetPriority(irqn, pri);
	}

	static void disable_irq(IRQn_Type irqn) {
		IRQ_Disable(irqn);
	}

	static void enable_irq(IRQn_Type irqn) {
		auto status =
			IRQ_SetMode((IRQn_ID_t)irqn, IRQ_MODE_TRIG_EDGE_RISING | IRQ_MODE_CPU_0 /*ALL*/ | IRQ_MODE_TYPE_IRQ);
		IRQ_Enable(irqn);
	}
};
} // namespace mdrivlib
