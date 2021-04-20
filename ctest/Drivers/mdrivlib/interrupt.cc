#include "interrupt.hh"
extern "C" {
#include "irq_ctrl.h"
}

static void ISRHandler(unsigned irqnum) {
	InterruptManager::callISR(irqnum);
}

extern "C" {
void __attribute__((interrupt)) SVC_Handler() {
	while (1)
		; // Debug breakpoint until we have a SVC to respond to
}

void __attribute__((interrupt)) IRQ_Handler() {
	IRQn_ID_t irqn = IRQ_GetActiveIRQ();
	ISRHandler(irqn);
	IRQ_EndOfInterrupt(irqn);
}

void __attribute__((interrupt)) FIQ_Handler() {
	while (1)
		; // Debug breakpoint until we have an FIQ to respond to
}
}
