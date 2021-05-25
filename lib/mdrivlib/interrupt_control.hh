#pragma once
#include "irq_ctrl.h"
#include "stm32mp1xx.h"

namespace mdrivlib
{
struct InterruptControl {

	// Sets the priority and sub-priority for an IRQ
	//
	// Note on priority:
	// Binary Point is set to 4 in IRQ_Initialize().
	// From Table 3-2 in arm_gic_architecture_specifiation, the group and sub-group are therefore:
	//     ggg.sssss
	// ARM CA7 uses 5-bit priority, and is left-aligned, so bottom 3 sss bits are ignored.
	// So we have:
	//     ggg.ssXXX
	//
	// So, pri1 is upper 3 bits (value can be 0-7), pri2 is lower 2 bits (value can be 0-3)
	static void set_irq_priority(IRQn_Type irqn, uint32_t pri1, uint32_t pri2)
	{
		if (pri1 > 7UL)
			pri1 = 7UL;
		if (pri2 > 3UL)
			pri2 = 3UL;
		auto pri = (pri1 << 5) | (pri2 << 3);
		GIC_SetPriority(irqn, pri);
	}

	static void disable_irq(IRQn_Type irqn)
	{
		GIC_DisableIRQ(irqn);
	}

	static void enable_irq(IRQn_Type irqn)
	{
		// IRQ_SetMode(irqn, IRQ_MODE_TRIG_EDGE_RISING | IRQ_MODE_CPU_0 | IRQ_MODE_TYPE_IRQ);
		GIC_SetConfiguration(irqn, 0b10); // Edge Triggered
		GIC_SetTarget(irqn, 0b10);		  // CPU0
		GIC_SetGroup(irqn, 0);			  // Group 0
		GIC_EnableIRQ(irqn);
	}
};
} // namespace mdrivlib
