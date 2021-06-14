#pragma once
#include "interrupt.hh"
#include "register_access.hh"
#include "stm32xx.h"
#include <functional>

namespace mdrivlib
{
namespace stm32mp1
{
namespace corea7 // FixMe: This is actually for both cores...
{
struct HSEM_ {
	template<uint32_t Mask>
	using IER = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, IER), Mask>;

	template<uint32_t Mask>
	using ICR = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, ICR), Mask>;

	template<uint32_t Mask>
	using ISR = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, ISR), Mask>;

	template<uint32_t Mask>
	using MISR = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, MISR), Mask>;
};
} // namespace corea7
} // namespace stm32mp1
} // namespace mdrivlib

enum class HWSemaphoreFlag {
	LockFailed = 0,
	LockedOk = 1,
	// AlreadySet, SameCoreAlreadySet, OtherCoreAlreadySet
};
template<uint32_t SemaphoreID>
struct HWSemaphore {
	HWSemaphore() = delete;

	static HWSemaphoreFlag lock() {
		return (HSEM->RLR[SemaphoreID] == (HSEM_R_LOCK | HSEM_CR_COREID_CURRENT)) ? HWSemaphoreFlag::LockedOk
																				  : HWSemaphoreFlag::LockFailed;
	}

	static HWSemaphoreFlag lock(uint32_t processID) {
		// Two-step lock:
		HSEM->R[SemaphoreID] = HSEM_R_LOCK | HSEM_CR_COREID_CURRENT | processID;
		return (HSEM->R[SemaphoreID] == (HSEM_R_LOCK | HSEM_CR_COREID_CURRENT | processID))
				 ? HWSemaphoreFlag::LockedOk
				 : HWSemaphoreFlag::LockFailed;
	}

	static void unlock() {
		HSEM->R[SemaphoreID] = HSEM_CR_COREID_CURRENT;
	}

	static void unlock(uint32_t processID) {
		HSEM->R[SemaphoreID] = HSEM_CR_COREID_CURRENT | processID;
	}

	static bool is_locked() {
		return HSEM->R[SemaphoreID] & HSEM_R_LOCK;
	}

	static void enable_channel_ISR() {
		if constexpr (HSEM_CR_COREID_CURRENT == (HSEM_CPU1_COREID << HSEM_CR_COREID_Pos))
			HSEM->C1IER = HSEM->C1IER | (1 << SemaphoreID);
		if constexpr (HSEM_CR_COREID_CURRENT == (HSEM_CPU2_COREID << HSEM_CR_COREID_Pos))
			HSEM->C2IER = HSEM->C2IER | (1 << SemaphoreID);
	}

	static void disable_channel_ISR() {
		if constexpr (HSEM_CR_COREID_CURRENT == (HSEM_CPU1_COREID << HSEM_CR_COREID_Pos))
			HSEM->C1IER = HSEM->C1IER & ~(1 << SemaphoreID);
		if constexpr (HSEM_CR_COREID_CURRENT == (HSEM_CPU2_COREID << HSEM_CR_COREID_Pos))
			HSEM->C2IER = HSEM->C2IER & ~(1 << SemaphoreID);
	}

	static void clear_ISR() {
		if constexpr (HSEM_CR_COREID_CURRENT == (HSEM_CPU1_COREID << HSEM_CR_COREID_Pos))
			HSEM->C1ICR = 1 << SemaphoreID;
		if constexpr (HSEM_CR_COREID_CURRENT == (HSEM_CPU2_COREID << HSEM_CR_COREID_Pos))
			HSEM->C2ICR = 1 << SemaphoreID;
	}

	// aka: is_status_after_masking_pending()
	static bool is_ISR_triggered_and_enabled() {
		if constexpr (HSEM_CR_COREID_CURRENT == (HSEM_CPU1_COREID << HSEM_CR_COREID_Pos))
			return HSEM->C1MISR & (1 << SemaphoreID);
		if constexpr (HSEM_CR_COREID_CURRENT == (HSEM_CPU2_COREID << HSEM_CR_COREID_Pos))
			return HSEM->C2MISR & (1 << SemaphoreID);
		return false;
	}
};

struct HWSemaphoreGlobalBase {
	using CallbackT = std::function<void(void)>;

	template<unsigned SemaphoreID>
	static void register_channel_ISR(CallbackT &&func) {
		funcs[SemaphoreID] = std::move(func);
	}

	static void enable_all_handlers(IRQn_Type irqn) {
		InterruptManager::registerISR(irqn, 0, 0, [&]() {
			handle_isr<0>();
			handle_isr<1>();
			handle_isr<2>();
			handle_isr<3>();
			handle_isr<4>();
			handle_isr<5>();
			handle_isr<6>();
			handle_isr<7>();
			handle_isr<8>();
			handle_isr<9>();
			handle_isr<10>();
			handle_isr<11>();
			handle_isr<12>();
			handle_isr<13>();
			handle_isr<14>();
			handle_isr<15>();
			handle_isr<16>();
			handle_isr<17>();
			handle_isr<18>();
			handle_isr<19>();
			handle_isr<20>();
			handle_isr<21>();
			handle_isr<22>();
			handle_isr<23>();
			handle_isr<24>();
			handle_isr<25>();
			handle_isr<26>();
			handle_isr<27>();
			handle_isr<28>();
			handle_isr<29>();
			handle_isr<30>();
			handle_isr<31>();
		});
	}

	template<unsigned SemaphoreID>
	static void handle_isr() {
		if (HWSemaphore<SemaphoreID>::is_ISR_triggered_and_enabled()) {
			if (funcs[SemaphoreID])
				funcs[SemaphoreID]();
			HWSemaphore<SemaphoreID>::clear_ISR();
			return;
		}
	}

	static inline CallbackT funcs[32];
};
