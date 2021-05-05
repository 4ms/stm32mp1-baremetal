#include "drivers/interrupt.hh"

extern "C" /*__attribute__((interrupt))*/ void ISRHandler(unsigned irqnum) {
	InterruptManager::callISR(irqnum);
}

extern "C" {
void __attribute__((interrupt)) SVC_Handler() {
	__BKPT();
}

void __attribute__((interrupt)) __attribute__((section(".irqhandler"))) IRQ_Handler() {
	asm volatile(
		".equ MODE_SYS, 0x1F  			\n"
		".equ GICCPU_BASE_low, 0x2000 	\n"
		".equ GICCPU_BASE_high, 0xA002 	\n"
		"sub lr, lr, #4 				\n" // LR = addr of *next* instr to execute; subtract 4 so we return to instr
											// that was about to be executed
		"srsdb sp!, MODE_SYS 			\n" // Save LR_irq and SPSR_irq onto the System mode stack, (decrement SP_sys)
		"cps MODE_SYS 		 			\n" // Switch to system mode
		"push {r0-r3, r12, lr} 			\n" // Store remaining AAPCS registers on the System mode stack
		"and r3, sp, #4  	 			\n" // Ensure stack is 8-byte aligned.
		"sub sp, sp, r3  				\n" //
		"push {r3}  					\n" // Store adjustment to stack
											//////////FPU
		"vmrs r1, FPSCR 				\n" // Copy FPU status reg to r1
		// "vpush {d0-d15} 				\n" // Push all double and single registers
		// "vpush {d16-d31} 				\n"
		"vpush {s0-s15} 				\n"
		"vpush {s16-s31} 				\n"
		"push {r1} 						\n" // Push the FPU status reg
											/////////

		// The following block used to be 'bl IRQ_GetActiveIRQ', which contained an Errata fix
		"mov r3, #GICCPU_BASE_low		\n"
		"movt r3, #GICCPU_BASE_high		\n" // Load address of the GIC CPU Interface
		"ldr r2, [r3, #24] 				\n" // HPPIR: Get Highest Pending Interrupt number
		"ldr r2, [r3, #12]				\n" // IAR: Acknowledge it with a read to the Interrupt Acknowledge Register
		"ubfx r0, r2, #0, #10 			\n" // Mask IRQ number to 0..1023
		// "uxth r0, r2 					\n" // Grab just the bottom 16bits
		"dsb sy							\n" // Data barrier (Todo: research why)

		"cmp r0, #0x03fc 				\n" // Check if it's a valid IRQ number
		"bge InvalidIRQNum 				\n" // Skip this if it's invalid

		"push {r0} 						\n" // Push IRQ number (r0) so ISRHandler doesn't overwrite
		"cpsie i 						\n" // Enable interrupts
		"bl ISRHandler 					\n" // Handle the ISR
		"pop {r0} 						\n" // Restore the IRQ number

		// The following block was 'bl IRQ_EndOfInterrupt', which contained an Errata fix
		"mov r3, #GICCPU_BASE_low		\n" //
		"movt r3, #GICCPU_BASE_high		\n" // Load address of the GIC CPU Interface
		"str r0, [r3, #16] 				\n" // EOIR: Write IRQ num to End of Interrupt Register to tell GIC we're done

		"cpsid i 						\n" // Disable interrupts while we exit

		"InvalidIRQNum: 				\n"

		//////////FPU
		"pop {r1} 						\n" // Pop the FPU status reg
		"vpop {s16-s31} 				\n"
		"vpop {s0-s15} 					\n"
		// "vpop {d16-d31} 				\n"
		// "vpop {d0-d15} 					\n" // Push all double and single registers
		"vmsr FPSCR, r1 				\n" // Restore FPU status reg from popped r1
											////////////

		"pop {r3} 						\n" // Pop the stack adjustment
		"add sp, sp, r3  				\n" // Restore previous stack pointer
		"pop {r0-r3, r12, lr} 			\n" //
		"rfeia sp! 						\n" // Return to address on stack, and pop SPSR (which restores the en/disable
											// state of IRQs)
	);
}

void __attribute__((interrupt)) FIQ_Handler() {
	__BKPT();
}
}
