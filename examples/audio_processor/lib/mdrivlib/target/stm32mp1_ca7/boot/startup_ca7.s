.syntax unified
.cpu cortex-a7

.equ MODE_FIQ, 0x11
.equ MODE_IRQ, 0x12
.equ MODE_SVC, 0x13
.equ MODE_ABT, 0x17
.equ MODE_UND, 0x1B
.equ MODE_SYS, 0x1F

.equ UART4_TDR, 0x40010028

.section .vector_table, "x"
.global _Reset
_Reset:
    b Reset_Handler
    b Undef_Handler 								// 0x4  Undefined Instruction 
    b SVC_Handler 									// Software Interrupt 
    b PAbt_Handler  								// 0xC  Prefetch Abort
    b DAbt_Handler 									// 0x10 Data Abort
    b . 											// 0x14 Reserved 
    b IRQ_Handler 									// 0x18 IRQ 
    b FIQ_Handler 									// 0x1C FIQ/

.section .resethandler
Reset_Handler:
	cpsid   if 										// Mask Interrupts

	mrc     p15, 0, R0, c1, c0, 0					// Read CP15 System Control register
	bic     R0, R0, #(0x1 << 12) 					// Clear I bit 12 to disable I Cache
	bic     R0, R0, #(0x1 <<  2) 					// Clear C bit  2 to disable D Cache
	bic     R0, R0, #0x1 							// Clear M bit  0 to disable MMU
	bic     R0, R0, #(0x1 << 11) 					// Clear Z bit 11 to disable branch prediction
	bic     R0, R0, #(0x1 << 13) 					// Clear V bit 13 to disable hivecs
	mcr     p15, 0, R0, c1, c0, 0 					// Write value back to CP15 System Control register
	isb
													// Configure ACTLR
	mrc     p15, 0, r0, c1, c0, 1 					// Read CP15 Auxiliary Control Register
	orr     r0, r0, #(1 <<  1) 						// Enable L2 prefetch hint (UNK/WI since r4p1)
	mcr     p15, 0, r0, c1, c0, 1 					// Write CP15 Auxiliary Control Register

													// Set Vector Base Address Register (VBAR) to point to this application's vector table
	ldr    R0, =0xC2000040
	mcr    p15, 0, R0, c12, c0, 0

	ldr r4, =UART4_TDR 								// UART: print 'A'
	mov r0, #65
	str r0, [r4]
    												// FIQ stack
    msr cpsr_c, MODE_FIQ
    ldr r1, =_fiq_stack_start
    ldr sp, =_fiq_stack_end
    movw r0, #0xFEFF
    movt r0, #0xFEFF
fiq_loop:
    cmp r1, sp
    strlt r0, [r1], #4
    blt fiq_loop

   													// IRQ stack
    msr cpsr_c, MODE_IRQ
    ldr r1, =_irq_stack_start
    ldr sp, =_irq_stack_end
    movw r0, #0xF1F1
    movt r0, #0xF1F1
irq_loop:
    cmp r1, sp
    strlt r0, [r1], #4
    blt irq_loop

   													// Supervisor (SVC) stack
    msr cpsr_c, MODE_SVC
    ldr r1, =_svc_stack_start
    ldr sp, =_svc_stack_end
    movw r0, #0xF5F5
    movt r0, #0xF5F5
svc_loop:
    cmp r1, sp
    strlt r0, [r1], #4
    blt svc_loop
													/* // Initialize nested ISR count */
	/* ldr r0, =0 */
	/* ldr r2, =_svc_nested_isr_count */
	/* str r0, [r2] */

													// USER and SYS mode stack
	msr cpsr_c, MODE_SYS
    ldr r1, =_user_stack_start
	ldr sp, =_user_stack_end
    movw r0, #0xF0F0
    movt r0, #0xF0F0
usrsys_loop:
    cmp r1, sp
    strlt r0, [r1], #4
    blt usrsys_loop
    												// Copying initialization values (.data)
    ldr r0, =_text_end
    ldr r1, =_data_start
    ldr r2, =_data_end

data_loop:
    cmp r1, r2
    ldrlt r3, [r0], #4
    strlt r3, [r1], #4
    blt data_loop
    												// Initialize .bss
    mov r0, #0
    ldr r1, =_bss_start
    ldr r2, =_bss_end

bss_loop:
    cmp r1, r2
    strlt r0, [r1], #4
    blt bss_loop

													// UART: print 'B'
	mov r5, #66
	str r5, [r4]

	bl SystemInit 									// System and libc/cpp init
    bl __libc_init_array

													// UART: print 'C'
	mov r5, #67
	str r5, [r4]

	CPSIE  i 										// enable irq interrupts

    bl main
    b Abort_Exception


.global aux_core_start
aux_core_start:
	cpsid   if 										// Disable Interrupts: keep them disabled since we use IRQ as a inter-processor signal
													
	bl SystemInitAuxCore 							// System and libc/cpp init

	msr cpsr_c, MODE_SYS 							// Setup secondary core user/sys mode stack
	ldr r1, =_auxcore_user_stack_start
	ldr sp, =_auxcore_user_stack_end
    movw r0, #0xF2F2
    movt r0, #0xF2F2
auxcore_usrsys_loop:
    cmp r1, sp
    strlt r0, [r1], #4
    blt auxcore_usrsys_loop

	cpsid   if 										// Disable Interrupts: keep them disabled since we use IRQ as a inter-processor signal
	bl aux_core_main 								// Go to secondary core main code



Abort_Exception:
	b .

Undef_Handler:
	b .

PAbt_Handler:
	b .

DAbt_Handler:
	b .
