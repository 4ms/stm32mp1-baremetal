#include <stdint.h>

#define UART4_TDR 0x40010028

void SystemInit(void) { return; }

int main(void) {

	*(char *)UART4_TDR = 'D';
	*(char *)UART4_TDR = 'E';
	*(char *)UART4_TDR = 'F';

	while (1)
		;

	return 0;
}
