extern "C" {


//This is the IRQ Handler that responds to interrupts. It gets the IRQ number
//and then calls ISRHandler(irqnum), which you MUST define somewhere in your project 
//(using extern "C" if you have a c++ project).
//
//When the GIC receives an IRQ for this CPU core, it calls this function.
//This is a minimal example, see nested_irq project for a more fully-featured IRQ handler.
//
// You must define a function named ISRHandler to handle interrupts with this signature:
// void ISRHandler(uint32_t IRQ_num)
//
void __attribute__((naked)) __attribute__((section(".irqhandler"))) IRQ_Handler()
{
	asm volatile(
		".equ GICCPU_BASE_high, 0xA002 	\n"
		".equ GICCPU_BASE_low, 0x2000 	\n"

		"sub lr, lr, #4 				\n" // LR = addr of *next* instr to execute; subtract 4 so we return to instr
											// that was about to be executed

		"push {lr} 						\n" // Push corrected LR and SPSR to IRQ-mode stack
		"mrs r0, spsr					\n" // SPSR_irq contains mode bits indicating which mode was interrupted
		"push {r0} 						\n"


		"mov r3, #GICCPU_BASE_low		\n" // Acknowledge interrupt with a read to the Interrupt Acknowledge Register
		"movt r3, #GICCPU_BASE_high		\n" // Reading this tells the GIC we're actively handling the IRQ now
		"ldr r0, [r3, #0x0C]			\n" // +0x0C = IAR

		"cmp r0, #0x03fc 				\n" // Skip this if it's spurious
		"bge ExitIRQHandler				\n" // (0x3ff = spurious, 0x3fc/d/e = reserved)

		"mov r1, sp 					\n" // Align the stack: If stack bit 2 is set, then it's not 8-byte aligned,
		"and r1, r1, #4 				\n" // so we should grow the stack by 4 bytes, and store this bit in r2.
		"sub sp, sp, r1 				\n" // If stack bit 2 is not set, nothing changes and we store 0 in r2

		"push {r0-r1} 	 				\n" // r0 = IRQnum, r1 = stack alignment bit

		"bl ISRHandler 					\n" // Call the ISR Handler.

		"pop {r0-r1} 	 				\n"

		"add sp, sp, r1 				\n" // Restore the stack alignment

		"mov r3, #GICCPU_BASE_low		\n" // Write IRQ num to End Interrupt Register
		"movt r3, #GICCPU_BASE_high		\n" // This tells the GIC we're done
		"str r0, [r3, #0x10] 			\n" // +0x10 = EOIR

		"ExitIRQHandler: 				\n"
		"pop {lr} 						\n" // pop the LR and SPSR so we can return
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
