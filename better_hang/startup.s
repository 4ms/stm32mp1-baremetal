.equ UART4_BASE, 0x40010000
.equ UART_TDR, 0x28
.equ UART4_TDR, 0x40010028

.section .vector_table, "x"
.global _Reset
_Reset:
    b Reset_Handler
    b . /* 0x4  Undefined Instruction */
    b . /* 0x8  Software Interrupt */
    b . /* 0xC  Prefetch Abort */
    b . /* 0x10 Data Abort */
    b . /* 0x14 Reserved */
    b . /* 0x18 IRQ */
    b . /* 0x1C FIQ */

.section .text
Reset_Handler:
_start:
	ldr r1, =UART4_TDR
	mov r0, #65
	str r0, [r1]
	mov r0, #66
	str r0, [r1]
	mov r0, #67
	str r0, [r1]
_hang:
	b .
