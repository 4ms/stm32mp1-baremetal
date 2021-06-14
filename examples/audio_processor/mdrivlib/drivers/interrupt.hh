#pragma once

#include "stm32xx.h"
using IRQType = IRQn_Type;

// FixMe: how to get around this ugly PP stuff?
#ifdef TESTPROJECT
#include "stubs/system.hh"
#else
#include "arch.hh"
#include "system.hh"
#endif

#include <functional>

// Interrupt Manager class
class Interrupt {
public:
	using ISRType = std::function<void(void)>;
	static inline const uint32_t NumISRs = 256;

	Interrupt() = default;
	Interrupt(IRQType irqnum, ISRType &&func) {
		ISRs[irqnum] = std::move(func);
	}

	// Register a callable object (e.g. lambda) to respond to an IRQ
	static void registerISR(IRQType irqnum, ISRType &&func) {
		ISRs[irqnum] = std::move(func);
	}

	// Register a callable object (e.g. lambda) to respond to an IRQ
	// Sets the priority and enables the IRQ immediately
	static void registerISR(IRQType irqnum, unsigned priority1, unsigned priority2, ISRType &&func) {
		target::System::disable_irq(irqnum);
		target::System::set_irq_priority(irqnum, priority1, priority2);
		ISRs[irqnum] = std::move(func);
		target::System::enable_irq(irqnum);
	}

	static inline void callISR(uint32_t irqnum) {
		ISRs[irqnum]();
	}

	// Sets a default handler for all ISRs.
	// This could be done for debug builds, to point to debug breakpoint
	// Copies the provided func, so a function pointer is recommended
	static inline void SetDefaultISR(ISRType func) {
		for (uint32_t i = 0; i < NumISRs; i++)
			ISRs[i] = func;
	}

private:
	static inline ISRType ISRs[NumISRs];
};

using InterruptManager = Interrupt;

// Todo:
// 		rename file to interrupt_handler.hh
// 		rename InterruptManager to InterruptHandler
// 		registerISR(IRQType, ISRType) ==> register_isr
// 		registerISR(IRQType, unsigned, unsigned, ISRType) ==> register_and_start_isr
//
// 		Test SetDefaultISR()
// 		try alternatives to std::function
// 			* inplace_function<void(void)>
//			  https://github.com/WG21-SG14/SG14/blob/master/Docs/Proposals/NonAllocatingStandardFunction.pdf
//			* function_view
//			  https://vittorioromeo.info/index/blog/passing_functions_to_functions.html#fn_view_impl
//
//
