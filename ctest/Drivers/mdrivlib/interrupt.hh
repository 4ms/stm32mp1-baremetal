#pragma once
#include <cstdint>
#include <functional>

// using IRQType = IRQn_Type;
using IRQType = unsigned;

// Interrupt Manager class
class Interrupt {
public:
	// Todo: Try inplace_function<void(void)> instead of std::function
	// https://github.com/WG21-SG14/SG14/blob/master/Docs/Proposals/NonAllocatingStandardFunction.pdf
	using ISRType = std::function<void(void)>;
	static inline const uint32_t NumISRs = 256;

	Interrupt() {
	}
	Interrupt(IRQType irqnum, ISRType &&func) {
		ISRs[irqnum] = std::move(func);
	}

	// Register a callable object (e.g. lambda) to respond to an IRQ
	static void registerISR(IRQType irqnum, ISRType &&func) {
		ISRs[irqnum] = std::move(func);
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
