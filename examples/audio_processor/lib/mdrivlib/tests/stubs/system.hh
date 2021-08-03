#pragma once
#include <cstdint>

namespace TestProject
{
using IRQn_Type = uint32_t;
struct InterruptControl {
	static void set_irq_priority(IRQn_Type irqn, uint32_t pri1, uint32_t pri2) {
	}

	static void disable_irq(IRQn_Type irqn) {
	}

	static void enable_irq(IRQn_Type irqn) {
	}
};
} // namespace TestProject

// class System {
// public:
// 	System() {
// 	}

// 	static uint32_t encode_nvic_priority(uint32_t priority1, uint32_t priority2) {
// 		return 0;
// 	}
// };
