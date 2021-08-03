#pragma once
#include "drivers/secondary_core_control.hh"
#include "stm32xx.h"
#include <cstdint>
#include <functional>

namespace mdrivlib
{

struct SMPControl {
	static inline __attribute__((section(".noncachable"))) uint32_t regs[4] = {0, 0, 0, 0};

	template<uint32_t channel>
	static void notify() {
		if constexpr (channel == 1)
			SecondaryCore::send_sgi(SGI1_IRQn);
		else if (channel == 2)
			SecondaryCore::send_sgi(SGI2_IRQn);
		else if (channel == 3)
			SecondaryCore::send_sgi(SGI3_IRQn);
		else if (channel == 4)
			SecondaryCore::send_sgi(SGI4_IRQn);
	}

	static void notify(uint32_t channel) {
		SecondaryCore::send_sgi(static_cast<IRQn_Type>(channel));
	}

	template<uint32_t reg_num = 0>
	static void write(uint32_t value) {
		if constexpr (reg_num == 0)
			regs[0] = value;
		else if (reg_num == 1)
			regs[1] = value;
		else if (reg_num == 2)
			regs[2] = value;
		else if (reg_num == 3)
			regs[3] = value;
	}

	static void write(uint32_t reg_num, uint32_t value) {
		if (reg_num >= 4)
			return;
		regs[reg_num] = value;
	}

	template<uint32_t reg_num = 0>
	static uint32_t read() {
		if constexpr (reg_num == 0)
			return regs[0];
		else if (reg_num == 1)
			return regs[1];
		else if (reg_num == 2)
			return regs[2];
		else if (reg_num == 3)
			return regs[3];
		return 0;
	}

	static uint32_t read(uint32_t reg_num) {
		return reg_num < 4 ? regs[reg_num] : 0;
	}
};

struct SMPThread {
	static constexpr uint32_t DataReg = 0;

	enum : uint32_t { CustomFunc0 = 0, CustomFunc1 = 0, CustomFunc2 = 2, CallFunction = 3 };

	static inline std::function<void()> thread_func;

	// Moves a function to static storage and notifies the secondary core that it should execute it
	// Aux Core must respond to the SGI#3 by calling SMPThread::execute();
	static void launch(std::function<void()> &&entry) {
		thread_func = std::move(entry);
		SMPControl::write<DataReg>(1);
		SMPControl::notify<CallFunction>();
	}

	template<uint32_t command_id, uint32_t data_reg = DataReg>
	static void launch_command(uint32_t data) {
		SMPControl::write<data_reg>(data);
		SMPControl::notify<command_id>();
	}

	// Called by the aux core to respond to receiving an SGI3
	static void execute() {
		thread_func();
		signal_done();
	}

	// Waits until thread completes
	static void join() {
		while (SMPControl::read<DataReg>() != 0)
			;
	}

	// Aux Core must call this after processing a custom command (any command besides CallFunction)
	static void signal_done() {
		SMPControl::write<DataReg>(0);
	}

	// Returns true if thread is completed
	static bool is_running() {
		return (SMPControl::read<DataReg>() != 0);
	}

	static void init() {
		SMPControl::write<DataReg>(0);
	}
};
} // namespace mdrivlib
