/* .equ GPIO1, 0x4804C000 */
/* .equ GPIO_OE, 0x134 */
/* .equ CM_PER, 0x44E00000 */
/* .equ CM_PER_GPIO1_CLKCTRL, 0xAC */
/* .equ GPIO_DATAOUT, 0x13C */

/* GPIO I, pin 8 = red D2 */
/* GPIO I, pin 9 = green D2 */
/* GPIO Z, pin 6 = red D1 */
/* GPIO Z, pin 7 = green D1 */
/* all LEDs are active low */

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
_loop:
	mov r0, #65
	str r0, [r1]
	mov r0, #66
	str r0, [r1]
	mov r0, #67
	str r0, [r1]
_hang:
	b _loop 

/* _start: */
/*     ldr r0, =CM_PER */
/*     ldr r1, =CM_PER_GPIO1_CLKCTRL */
/*     add r0, r1 */
/*     mov r1, #2 */
/*     str r1, [r0] */
/* _led_enable: */
/*     ldr r0, =GPIO1 */
/*     ldr r1, =GPIO_OE */
/*     add r0, r1 */
/*     mov r1, #0 */
/*     str r1, [r0] */
/* _main: */
/*     ldr r0, =GPIO1 */
/*     ldr r1, =GPIO_DATAOUT */
/*     add r0, r1 */
/*     ldr r1, =0xFFFFFFFF */
/*     str r1, [r0] */
/* loop: */
/*     ldr r2, =0x00010000 */
/*     sub r2, #1 */
/*     cmp r2, #0 */
/*     beq _main */
/*     mov r1, #0 */
/*     str r1, [r0] */
/* _hang: */
/*     b _main */
