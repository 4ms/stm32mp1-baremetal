#include <stdint.h>

#define UART4_TDR 0x40010028
volatile uint8_t *uart0 = (uint8_t *)UART4_TDR;

const uint32_t *UART4_ISR = (uint32_t *)0x40010028;
const uint32_t TXFE = (1 << 23); //FIFO Empty
const uint32_t TXFNF = (1 << 7); //TX FIFO Not Full
const uint32_t TC = (1 << 6);	 //Tx Complete

void delay_for_uart(void) {
	int i = 255;
	while (i--)
		;
}

uint32_t uart_ready() {
	uint32_t isr = (*UART4_ISR);
	if (isr & TC)
		return 1;
	else
		return 0;
}

void write(const char *str) {
	while (*str) {
		*uart0 = *str++;
		delay_for_uart();
	}
}

int main() {
	*uart0 = 'X';
	*uart0 = 'Y';
	*uart0 = 'Z';
	*uart0 = '\r';
	*uart0 = '\n';

	const char *s = "Hello world from bare-metal!\r\n";
	write(s);

	const char *s2 = "And hi to you too!\r\n";
	while (*s2 != '\0') {
		*uart0 = *s2;
		s2++;
		delay_for_uart();
	}

	while (1) {
	};

	return 0;
}

void SystemInit(void) {
	return;
}

