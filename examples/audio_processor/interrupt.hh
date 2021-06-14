#pragma once
#include <cstdint>
#include <functional>

using IRQType = uint32_t;

// Interrupt Handler Manager class
//
// Note: Uses std::function, which MAY use dynamic memory if you capture too much in your lambda.
// In practice, it's easy to avoid using dynamic memory (if that's your goal).
// consider just capturing [this], which is a 4-byte pointer. That's often enough.
class Interrupt {
public:
	using ISRType = std::function<void(void)>;
	static inline const uint32_t NumISRs = 256;

	Interrupt() = default;

	Interrupt(IRQType irqnum, ISRType &&func)
	{
		ISRs[irqnum] = std::move(func);
	}

	// Register a callable object (e.g. lambda) to respond to an IRQ
	static void registerISR(IRQType irqnum, ISRType &&func)
	{
		ISRs[irqnum] = std::move(func);
	}

	static void callISR(uint32_t irqnum)
	{
		ISRs[irqnum]();
	}

	// Sets a default handler for all ISRs.
	// This could be done for debug builds, to point to debug breakpoint
	// Copies the provided func, so a function pointer is recommended
	static void SetDefaultISR(ISRType func)
	{
		for (uint32_t i = 0; i < NumISRs; i++)
			ISRs[i] = func;
	}

private:
	static inline ISRType ISRs[NumISRs];
};

using InterruptManager = Interrupt;
