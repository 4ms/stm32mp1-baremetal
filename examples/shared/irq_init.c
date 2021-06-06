#include "irq_ctrl.h"
#include "stm32mp1xx.h"
#include <stddef.h>

void GIC_ClearActiveIRQ(IRQn_Type IRQn)
{
	if (IRQn >= 16U) {
		GICDistributor->ICACTIVER[IRQn / 32U] = 1U << (IRQn % 32U);
	}
}

/// Initialize interrupt controller.
int32_t IRQ_Initialize(void)
{
	GIC_Enable();

	unsigned num_irq = 32U * ((GIC_DistributorInfo() & 0x1FU) + 1U);
	int x;
	do {
		x = GIC_AcknowledgePending();
		__DSB();
		__ISB();
		if (x < num_irq)
			GIC_EndInterrupt(x);
		__DSB();
		__ISB();
	} while (x >= 0 && x < num_irq);

	for (unsigned i = 32; i < num_irq; i++) {
		int act_pend = GIC_GetIRQStatus(i);
		int active = (act_pend & 0b10) >> 1;
		int pending = act_pend & 0b01;

		if (active)
			GIC_ClearActiveIRQ(i);
		if (pending)
			GIC_ClearPendingIRQ(i);
	}

	// Reset the active priority register, in case we halted/reset during an ISR
	// FixMe: This doesn't always work! Sometimes if we halt during an ISR handler, and upload new code, we have to
	// power cycle for it to run properly.
	GICInterface->APR[0] = 0x00U;
	__DSB();
	__ISB();

	GIC_SetInterfacePriorityMask(0xFF); // gic arch v2.0 specification: section 3.3.2: Writing 255 to GICC_PMR always
										// sets it to the largest supported priority field value.
	GIC_SetBinaryPoint(4);

	return (0);
}
