#include "interrupt.hh"

extern "C" void ISRHandler(unsigned irqnum)
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
		"mrs lr, spsr					\n" // SPSR_irq contains mode bits indicating which mode was interrupted
		"push {lr} 						\n"

		"cps MODE_SVC 		 			\n" // Switch to SVC mode to handle IRQ
		"push {r0-r12, lr} 	 			\n" // Push everything the interrupted context might use

		"mov r1, #GICCPU_BASE_low		\n" // Acknowledge interrupt with a read to the Interrupt Acknowledge Register
		"movt r1, #GICCPU_BASE_high		\n"
		"ldr r0, [r1, #0x0C]			\n" // +0x0C = IAR

		"cmp r0, #0x03fc 				\n" // Skip this if it's spurious
		"bge InvalidIRQNum 				\n" // (0x3ff = spurious, 0x3fc/d/e = reserved)

		"mov r2, sp 					\n" // If stack bit 2 is set, then it's not 8-byte aligned,
		"and r2, r2, #4 				\n" // so we should grow the stack by 4-bytes, and store this bit in r2.
		"sub sp, sp, r2 				\n" // If stack bit 2 is not set, nothing changes and we store 0 in r2

		"push {r0-r4, lr} 	 			\n" // r0 = IRQnum, r2 = stack alignment, r1 = GICCPU base address. Oddly, it's
											// faster to push r0-r4,lr than to push r0-r2,lr

		// Todo: consider loading a bool from InterruptManager that says whether we should use the FPU regs or not
		"vmrs r1, FPSCR 				\n" // Push all FPU regs and FPU status reg
		"vpush {d0-d15} 				\n"
		"vpush {d16-d31} 				\n"
		"push {r1} 						\n"

		"cpsie i 						\n"

		"bl ISRHandler 					\n" // Call the ISR Handler.

		"cpsid i 						\n" // Disable interrupts so we can exit

		"pop {r1} 						\n" // Pop all FPU regs and FPU status reg
		"vpop {d16-d31} 				\n"
		"vpop {d0-d15} 					\n"
		"vmsr FPSCR, r1 				\n"

		"pop {r0-r4, lr} 				\n"

		"add sp, sp, r2 				\n"

		"str r0, [r1, #0x10] 			\n" // +0x10 = EOIR: Write IRQ num to End Interrupt Register

		"InvalidIRQNum: 				\n"
		"pop {r0-r12, lr} 				\n" // Restore the registers of the interrupted context

		"cps MODE_IRQ 					\n" // Go back to IRQ mode and pop the LR and SPSR so we can return
		"pop {lr} 						\n"
		"msr spsr_cxsf, lr				\n"
		"pop {lr} 						\n"
		"movs pc, lr 					\n" // Return, switching to previous mode (because CPSR_irq contains mode bits
											// set to the previous mode)
	);
}

void FIQ_Handler()
{
	while (1)
		;
}
void SVC_Handler()
{
	while (1)
		;
}
}
