#include "interrupt.hh"

extern "C" /*__attribute__((interrupt("IRQ")))*/ void ISRHandler(unsigned irqnum)
{
	InterruptManager::callISR(irqnum);
}

extern "C" {

void __attribute__((naked)) __attribute__((section(".irqhandler"))) IRQ_Handler()
{
	asm volatile(

		".equ MODE_IRQ, 0x12  			\n"
		".equ MODE_SVC, 0x13  			\n"
		".equ MODE_SYS, 0x1F  			\n"
		".equ GICCPU_BASE_high, 0xA002 	\n"
		".equ GICCPU_BASE_low, 0x2000 	\n"

		"sub lr, lr, #4 				\n" // LR = addr of *next* instr to execute; subtract 4 so we return to instr
											// that was about to be executed

		"push {lr} 						\n" // Push corrected LR and SPSR to IRQ-mode stack
		"mrs lr, spsr					\n"
		"push {lr} 						\n"

		"cps MODE_SVC 		 			\n" // Switch to SVC mode to handle IRQ
		"push {r0-r4, r12} 		 		\n" // Push everything we use in this routine, to allow re-entry.

		"mov r3, #GICCPU_BASE_low		\n" // Acknowledge interrupt with a read to the Interrupt Acknowledge Register
		"movt r3, #GICCPU_BASE_high		\n"
		"ldr r0, [r3, #0x0C]			\n" // +0x0C = IAR

		"cmp r0, #0x03fc 				\n" // Skip this if it's not a real interrupt
		"bge InvalidIRQNum 				\n" // (0x3ff = spurious, 0x3fc/d/e = reserved)

		"mov r2, sp 					\n" // If stack bit 2 is set, then it's not 8-byte aligned,
		"and r2, r2, #4 				\n" // so we should grow the stack by 4-bytes, and store this bit in r2.
		"sub sp, sp, r2 				\n" // If stack bit 2 is not set, nothing changes and we store 0 in r2

		"push {r0-r4, lr} 				\n" // r0 = IRQnum, r2 = stack alignment, r3 = GICCPU base address
											// r1, r4 might not be needed (?) why lr?
		"vmrs r1, FPSCR 				\n" // Push all FPU regs and FPU status reg
		"vpush {d0-d15} 				\n"
		"vpush {d16-d31} 				\n"
		"push {r1} 						\n"

		"dsb 							\n"
		"isb 							\n"
		"cpsie i 						\n"

		"bl ISRHandler 					\n" // Call the ISR Handler. compiler should make sure all registers are
											// restored
											// TODO: Check that all regs really are restored, and see if that includes
											// FPU regs (so we don't have to pop them ourselves

		"cpsid i 						\n" // Disable interrupts so we can exit
		"dsb 							\n"
		"isb 							\n"

		"pop {r1} 						\n" // Pop all FPU regs and FPU status reg
		"vpop {d16-d31} 				\n"
		"vpop {d0-d15} 					\n"
		"vmsr FPSCR, r1 				\n"

		"pop {r0-r4, lr} 				\n"

		"add sp, sp, r2 				\n"

		//"dsb 							\n" //From zephyr
		"str r0, [r3, #0x10] 			\n" // +0x10 = EOIR: Write IRQ num to End Interrupt Register

		"InvalidIRQNum: 				\n"
		"pop {r0-r4, r12} 				\n" // Restore the registers of SVC mode
		"cps MODE_IRQ 					\n" // Go back to IRQ mode and pop the LR and SPSR so we can return
		"pop {lr} 						\n"
		"msr spsr_cxsf, lr				\n"
		"pop {lr} 						\n"
		"movs pc, lr 					\n");
}

void __attribute__((interrupt)) FIQ_Handler()
{
	while (1)
		;
}
void __attribute__((interrupt)) SVC_Handler()
{
	while (1)
		;
}
}
