#include <stdint.h>

#define UART4_TDR 0x40010028
volatile uint8_t *uart0 = (uint8_t *)UART4_TDR;

void write(const char *str) {
	while (*str) {
		*uart0 = *str++;
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
	while (*s != '\0') {
		*uart0 = *s;
		s++;
	}
	while (1) {
	};

	return 0;
}

void SystemInit(void) { return; }

