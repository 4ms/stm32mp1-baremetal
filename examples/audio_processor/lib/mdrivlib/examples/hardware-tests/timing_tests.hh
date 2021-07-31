#pragma once
#include "debug.hh"
#include "drivers/pin.hh"

static inline void timing_test(uint32_t addr) {
	auto baseaddr = reinterpret_cast<uint32_t *>(addr);
	for (uint32_t i = 0; i < 1000; i++) {
		*baseaddr++ = i;
	}
}

// Tests memory store/recall speed in various memory locations
// As a base-line, it tests the time to run a loop of NOPs.
// You must change the addresses to valid address in your system
static inline void run_all_timing_tests() {
	// 4006 inst (MOVW + MOVW + SUBS + BNE, *1000): 4.8us: 834MIPS
	// 3006 inst: (NOP + SUBS + BNE, *1000) 2.48us: 1212
	// 4006 inst (NOP + SUBS + BNE, *1333): 3.58us: 1118
	// 6007 inst (NOP + SUBS + BNE, *2000): 5.58us: 1076
	// 24007 inst (NOP + SUBS + BNE, *8000): 24us: 1000
	//
	Debug::Pin2::low();
	Debug::Pin1::high();
	for (uint32_t i = 0; i < 8000; i++) {
		asm volatile("NOP\n");
	}
	Debug::Pin2::high();
	Debug::Pin1::low();

	Debug::Pin2::high();
	Debug::Pin1::high();
	// w/o MMU: 14us for 4006 instructions: 286MIPS
	// with MMU: 4.99us --> 802MIPS
	// STR + ADD + CMP +BNE
	timing_test(0xD8000000);
	Debug::Pin1::low();

	// w/o MMU: 48us for 4007 instructions: 83MIPS (each test)
	// with MMU: 4.9us--> 830MIPS
	timing_test(0x10000000);
	Debug::Pin1::high();
	timing_test(0x10020000);
	Debug::Pin1::low();
	timing_test(0x10040000);
	Debug::Pin1::high();
	timing_test(0x10050000);
	Debug::Pin1::low();

	Debug::Pin2::low();
}
