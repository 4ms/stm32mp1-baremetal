#include "doctest.h"

#include "stubs/system.hh"
using namespace TestProject;

#include "drivers/interrupt.hh"
using namespace mdrivlib;

static int g_state = 0;

static void some_func(int some_arg) {
	g_state = some_arg;
}

static IRQType irqmap[7] = {
	ADC_IRQn,
	CEC_IRQn,
	COMP_IRQn,
	CRS_IRQn,
	ECC_IRQn,
	DCMI_IRQn,
	FPU_IRQn,
};

static void register_a_local_lamba(uint32_t testIRQnum, uint32_t state_value) {
	auto isrfunc = [state_value = state_value]() { some_func(state_value); };
	InterruptManager::registerISR(irqmap[testIRQnum], isrfunc);
}

TEST_CASE("InterruptManager Tests") {

	g_state = 0;

	SUBCASE("interrupt_tests, lambda_gets_called") {
		unsigned testIRQnum = 5;

		Interrupt myint{
			irqmap[testIRQnum],
			[]() { some_func(222222); },
		};

		CHECK_EQ(0, g_state);
		InterruptManager::callISR(irqmap[testIRQnum]);
		CHECK_EQ(222222, g_state);
	}

	SUBCASE("interrupt_tests, static_init") {
		unsigned testIRQnum = 5;
		InterruptManager::registerISR(irqmap[testIRQnum], []() { some_func(33333); });

		CHECK_EQ(0, g_state);
		InterruptManager::callISR(irqmap[testIRQnum]);
		CHECK_EQ(33333, g_state);
	}

	SUBCASE("interrupt_tests, lambda_scope_ends_but_still_gets_called") {
		unsigned testIRQnum = 5;

		register_a_local_lamba(testIRQnum, 4444);

		CHECK_EQ(0, g_state);
		InterruptManager::callISR(irqmap[testIRQnum]);
		CHECK_EQ(4444, g_state);
	}
}
