#include "main.h"
#include "stm32mp1xx_hal.h"

void UART4_Init(void);
void GPIO_Init(void);
void Error_Handler(void);
void soft_breakpoint();

UART_HandleTypeDef huart4;
const char *global_constant_string = "And hi to you too!\r\n";

void main() {
	HAL_Init();
	HAL_Delay(1);
	GPIO_Init();
	UART4_Init();
	
	printf("\r\nHello from HAL Driver!\r\n");
	printf("%s", global_constant_string);

	uint32_t sec = 0;
	uint32_t tim = 0;
	uint32_t old_tim = 0;
	while (1) {
		tim = HAL_GetTick();
		if(tim > (old_tim + 1000)) {
			printf("%d\r\n", sec);
			sec++;
			old_tim = tim;
			HAL_GPIO_TogglePin(LED_RED_PORT, LED_RED_PIN);
			HAL_GPIO_TogglePin(LED_BLUE_PORT, LED_BLUE_PIN);
		}
	}
}

void UART4_Init(void)
{
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart4.FifoMode = UART_FIFOMODE_ENABLE;
	huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if(HAL_UART_Init(&huart4) != HAL_OK) {
		Error_Handler();
	}
}

void GPIO_Init() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = LED_RED_PIN | LED_BLUE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

PUTCHAR_PROTOTYPE {
  HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

// Handy utility when using a debugger
void soft_breakpoint() {
	volatile int stop = 1;
	while (stop) {
		// Attach a debugger and manually change the value at the address of `stop` in RAM from 1 to 0
	}
}

void IRQ_Initialize(void) {
	// do nothing, just a stub
}

void Error_Handler() {
	while(1) {
		// handle error
	}
}
