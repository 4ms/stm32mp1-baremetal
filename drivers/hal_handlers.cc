#include "stm32xx.h"

extern void recover_from_task_fault(void);

extern "C" {
void SysTick_Handler(void) {
	HAL_IncTick();
}

#define HARDFAULT_HANDLING_ASM(_x)                                                                                     \
	__asm volatile("tst lr, #4 \n"                                                                                     \
				   "ite eq \n"                                                                                         \
				   "mrseq r0, msp \n"                                                                                  \
				   "mrsne r0, psp \n"                                                                                  \
				   "b my_fault_handler_c \n")

#define HALT_IF_DEBUGGING()                                                                                            \
	do {                                                                                                               \
		if ((*(volatile uint32_t *)0xE000EDF0) & (1 << 0)) {                                                           \
			__asm("bkpt 1");                                                                                           \
		}                                                                                                              \
	} while (0)

void HardFault_Handler() {
	HARDFAULT_HANDLING_ASM();
}

typedef struct __attribute__((packed)) ContextStateFrame {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t return_address;
	uint32_t xpsr;
} sContextStateFrame;

__attribute__((optimize("O0"))) void my_fault_handler_c(sContextStateFrame *frame) {
	// https://interrupt.memfault.com/blog/cortex-m-fault-debug
	// HALT_IF_DEBUGGING();
	volatile uint32_t mmfar = SCB->MMFAR;
	volatile uint32_t bfar = SCB->BFAR;
	volatile uint32_t hfsr = SCB->HFSR;
	volatile uint32_t afsr = SCB->AFSR;
	volatile uint32_t dfsr = SCB->DFSR;
	volatile uint32_t cfsr = SCB->CFSR;

	// UFSR - UsageFaultStatusReg
	volatile uint16_t UsageFaultStatusReg = cfsr >> 16;
	volatile bool ufsr_undef_instruction = UsageFaultStatusReg & (1 << 0);
	// e.g. thumb mode, interworking addresses:
	volatile bool ufsr_invstate_invalid_EPSR = UsageFaultStatusReg & (1 << 1);
	// failed integrety test on EXC_RETURN (exception return):
	volatile bool ufsr_invalid_PC = UsageFaultStatusReg & (1 << 2);
	// co-processor, e.g. FPU, not enabled:
	volatile bool ufsr_no_coproc = UsageFaultStatusReg & (1 << 3);
	volatile bool ufsr_unaligned = UsageFaultStatusReg & (1 << 8);
	volatile bool ufsr_divbyzero = UsageFaultStatusReg & (1 << 9);

	// BFSR - BusFaultStatusReg
	volatile uint8_t BusFaultStatusReg = (cfsr >> 8) & 0xFF;
	volatile bool bfsr_ibuserr = BusFaultStatusReg & (1 << 0);
	// instruction which was executing prior to exception entry triggered the fault:
	volatile bool bfsr_preciserr = BusFaultStatusReg & (1 << 1);
	// hardware was not able to determine exact location of fault:
	volatile bool bfsr_impreciserr = BusFaultStatusReg & (1 << 2);
	// error returning from exception:
	volatile bool bfsr_unstkerr = BusFaultStatusReg & (1 << 3);
	// fault occured during exception entry:
	volatile bool bfsr_stkerr = BusFaultStatusReg & (1 << 4);
	// fault occured during lazy state preservation:
	volatile bool bfsr_lsperr = BusFaultStatusReg & (1 << 5);
	// indicates if BFAR has address that triggrered the fault:
	volatile bool bfsr_BFAR_valid = BusFaultStatusReg & (1 << 7);

	// MMFSR - MemManageFaultStatusReg
	volatile uint8_t MemManageFaultStatusReg = cfsr & 0xFF;
	volatile bool mmfsr_instr_access_violation = MemManageFaultStatusReg & (1 << 0);
	volatile bool mmfsr_data_access_violation = MemManageFaultStatusReg & (1 << 1);
	volatile bool mmfsr_munstkerr = MemManageFaultStatusReg & (1 << 3);
	volatile bool mmfsr_mlsperr = MemManageFaultStatusReg & (1 << 4);
	volatile bool mmfsr_mstkerr = MemManageFaultStatusReg & (1 << 5);
	// indicates that MMFAR holds the address that triggered the fault:
	volatile bool mmfsr_mmfar_valid = MemManageFaultStatusReg & (1 << 7);

	// HFSR - HardFaultStatusReg
	// fault occured due to reading address in vector table
	volatile bool hfsr_vectable = hfsr & (1 << 1);
	// configurable fault was escalated to HardFault (handler not enabled,
	// or fault occurred in handler)
	volatile bool hfsr_forced = hfsr & (1 << 30);
	// Debug event (breakpoint) occured while debug subsystem not enabled
	volatile bool hfsr_debugevt = hfsr & (1 << 31);

	// Try to recover
	//
	const uint32_t usage_fault_mask = 0xffff0000;
	const bool non_usage_fault_occurred = (cfsr & ~usage_fault_mask) != 0;
	// the bottom 8 bits of the xpsr hold the exception number of the
	// executing exception or 0 if the processor is in Thread mode
	const bool faulted_from_exception = ((frame->xpsr & 0xFF) != 0);

	// Force a breakpoint here
	__BKPT();

	if (faulted_from_exception || non_usage_fault_occurred) {
		// For any fault within an ISR or non-usage faults let's reboot the system
		volatile uint32_t *aircr = (volatile uint32_t *)0xE000ED0C;
		*aircr = (0x05FA << 16) | 0x1 << 2;
		while (1) {
		} // should be unreachable
	}

	// If it's just a usage fault, let's "recover"
	// Clear any faults from the CFSR
	volatile uint32_t *cfsr_ptr = (volatile uint32_t *)0xE000ED28;
	*cfsr_ptr |= *cfsr_ptr;
	// the instruction we will return to when we exit from the exception
	frame->return_address = (uint32_t)recover_from_task_fault;
	// the function we are returning to should never branch
	// so set lr to a pattern that would fault if it did
	frame->lr = 0xdeadbeef;
	// reset the psr state and only leave the
	// "thumb instruction interworking" bit set
	frame->xpsr = (1 << 24);

	while (1)
		;
}
//
// void assert_failed(const char* file, uint32_t line) { while (1); }
void NMI_Handler() {
	while (1)
		;
}
void MemManage_Handler() {
	while (1)
		;
}
void BusFault_Handler() {
	while (1)
		;
}
void UsageFault_Handler() {
	while (1)
		;
}
void SVC_Handler() {
	while (1)
		;
}
void DebugMon_Handler() {
	while (1)
		;
}
void PendSV_Handler() {
	while (1)
		;
}
}
