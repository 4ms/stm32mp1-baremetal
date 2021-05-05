#include "stm32mp157cxx_ca7.h"
#include "stm32mp1xx_ll_gpio.h"
#include "stm32mp1xx_ll_usart.h"
#include <stdint.h>

void delay_long();
void soft_breakpoint();
inline void delay_for_uart(void);
void write(const char *str);
inline void init_leds();

// OSD32MP1 board:
// GPIO I, pin 8 = red D2
// GPIO I, pin 9 = green D2
// GPIO Z, pin 6 = red D1
// GPIO Z, pin 7 = green D1
// all LEDs are active low

int main() {
	// Test UART
	UART4->TDR = 'X';
	delay_for_uart();
	UART4->TDR = 'Y';
	delay_for_uart();
	UART4->TDR = 'Z';
	delay_for_uart();
	UART4->TDR = '\r';
	delay_for_uart();
	UART4->TDR = '\n';
	delay_for_uart();

	init_leds();

	LL_GPIO_SetOutputPin(GPIOI, LL_GPIO_PIN_8);
	LL_GPIO_ResetOutputPin(GPIOI, LL_GPIO_PIN_8);

	LL_GPIO_SetOutputPin(GPIOI, LL_GPIO_PIN_9);
	LL_GPIO_ResetOutputPin(GPIOI, LL_GPIO_PIN_9);

	LL_GPIO_SetOutputPin(GPIOZ, LL_GPIO_PIN_6);
	LL_GPIO_ResetOutputPin(GPIOZ, LL_GPIO_PIN_6);

	LL_GPIO_SetOutputPin(GPIOZ, LL_GPIO_PIN_7);
	LL_GPIO_ResetOutputPin(GPIOZ, LL_GPIO_PIN_7);

	// Test function calls (tests the stack)
	// So far, every function call has been inline
	const char *s = "Hello world from bare-metal!\r\n";
	write(s);

	const char *s2 = "And hi to you too!\r\n";
	while (*s2 != '\0') {
		UART4->TDR = *s2;
		s2++;
		delay_for_uart();
	}

	// Blink those lights forever!
	while (1) {
		LL_GPIO_SetOutputPin(GPIOI, LL_GPIO_PIN_8);
		delay_long();
		LL_GPIO_ResetOutputPin(GPIOI, LL_GPIO_PIN_8);

		LL_GPIO_SetOutputPin(GPIOI, LL_GPIO_PIN_9);
		delay_long();
		LL_GPIO_ResetOutputPin(GPIOI, LL_GPIO_PIN_9);

		LL_GPIO_SetOutputPin(GPIOZ, LL_GPIO_PIN_6);
		delay_long();
		LL_GPIO_ResetOutputPin(GPIOZ, LL_GPIO_PIN_6);

		LL_GPIO_SetOutputPin(GPIOZ, LL_GPIO_PIN_7);
		delay_long();
		LL_GPIO_ResetOutputPin(GPIOZ, LL_GPIO_PIN_7);
	};

	return 0;
}

void delay_long() {
	uint32_t i = 0x100000;
	while (i--)
		;
}

// Handy utility when using a debugger
void soft_breakpoint() {
	volatile int stop = 1;
	while (stop) {
		// Attach a debugger and manually change the value at the address of `stop` in RAM from 1 to 0
	}
}

// Inline because we don't want to assume function calling and returning works yet!
inline void delay_for_uart(void) {
	int i = 255;
	while (i--)
		;
}

void write(const char *str) {
	while (*str) {
		UART4->TDR = *str++;
		delay_for_uart();
	}
}

// Inline because we don't want to assume function calling and returning works yet!
inline void init_leds() {
	// Enable RCC for GPIO I and GPIO Z (for the LEDs on the OSD32 board)
	RCC->MC_AHB4ENSETR |= RCC_MC_AHB4ENSETR_GPIOIEN;
	RCC->MC_AHB5ENSETR |= RCC_MC_AHB5ENSETR_GPIOZEN;

	LL_GPIO_SetPinMode(GPIOI, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(GPIOI, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinSpeed(GPIOI, LL_GPIO_PIN_8, LL_GPIO_SPEED_FREQ_MEDIUM);
	LL_GPIO_SetPinMode(GPIOI, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(GPIOI, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinSpeed(GPIOI, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_MEDIUM);
}

// Todo:
// - Test IRQ handler: UART RX ISR, or HSEM ISR
// - Test static initialization of cpp objects: create a data structure, then output it via the UART
// - Test SysTick (do we have HAL_Tick()?)
// - Get USE_HAL_DRIVER working
