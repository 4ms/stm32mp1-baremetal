#include "main.h"
#include "stm32mp1xx_hal.h"
#include "lcd.h"
#include "ft6x06.h"
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"

/* User define H_RES and V_RES to configure display in Landscape or Portrait. 
 *   For Landscape set H_RES 800 and V_RES 480
 *   For Portraint set H_RES 480 and V_RES 800
 * Set desired method to transfer pixel buffer to framebuffer. 
 *   To user 2 full framebuffers set DOUBLE_BUFFER 1
 *   To transfer pixel data with MDMA set MDMA_PIXEL_XFER 1 (DOUBLE_BUFFER must be 0)
 *   To transfer pixel data with DMA set DMA_PIXEL_XFER 1 (DOUBLE_BUFFER and MDMA_PIXEL_XFER must be 0)
 *   To transfer pixel data using CPU set DOUBLE_BUFFER 0, MDMA_PIXEL_XFER 0, and DMA_PIXEL_XFER 0
 *     CPU_PIXEL_XFER can be set to 1 but is only used for readability. 
 */
#define H_RES 480
#define V_RES 800
#define DOUBLE_BUFFER 1
#define MDMA_PIXEL_XFER 0
#define DMA_PIXEL_XFER 0
#define CPU_PIXEL_XFER 0

int32_t x1_flush, y1_flush, x2_flush, y2_fill, y_fill_act;
uint32_t buf_to_flush;
uint8_t touched = 0;

void UART4_Init(void);
void GPIO_Init(void);
void TIM2_Init(void);
void I2C1_Init(void);
void MDMA_Config(int32_t buf_offset);
void MDMA_TransferComplete(MDMA_HandleTypeDef *han);
void MDMA_Callback(void);
void DMA_Config(void);
void DMA_Callback(void);
void DMA_Complete(DMA_HandleTypeDef *han);
void my_flush_cb(lv_disp_drv_t * disp_drv_f, const lv_area_t * area, lv_color_t * color_p);
void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t * data);
void tp_int(void);
void tim2_elapsed(void);
void set_interrupt(uint32_t irq_num, uint32_t cpu, uint32_t priority, uint32_t edge_detect, void* callback);
void Error_Handler(void);
void soft_breakpoint(void);
void local_lv_example(void);

UART_HandleTypeDef huart4;
TIM_HandleTypeDef htim2;
I2C_HandleTypeDef hi2c1;
MDMA_HandleTypeDef hmdma;
DMA_HandleTypeDef hdma;

uint8_t run_lv_task_handler = 0;
static lv_disp_drv_t disp_drv;

void (*irq_callback[249])();

__attribute__((section (".RAM2"))) static lv_color_t fb_1[H_RES * V_RES];
#if DOUBLE_BUFFER
__attribute__((section (".RAM2"))) static lv_color_t fb_2[H_RES * V_RES];
#else
__attribute__((section (".RAM2"))) static lv_color_t buf_1[H_RES * V_RES / 10];
__attribute__((section (".RAM2"))) static lv_color_t buf_2[H_RES * V_RES / 10];
#endif

void main() {
	HAL_Init();
	HAL_Delay(1);
	GPIO_Init();
	UART4_Init();
	I2C1_Init();
	DMA_Config();
	TIM2_Init();
	if(H_RES > V_RES)
		LCD_Init(OTM8009A_ORIENTATION_LANDSCAPE, (uint32_t)fb_1);
	else
		LCD_Init(OTM8009A_ORIENTATION_PORTRAIT, (uint32_t)fb_1);
	
	// ft6x06 driver is compatible with ft6236
	ft6x06_Init(0x70);
	
	lv_init();
	
	/*A static or global variable to store the buffers*/
	static lv_disp_draw_buf_t disp_buf;
	
	/*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
#if DOUBLE_BUFFER
	lv_disp_draw_buf_init(&disp_buf, fb_1, fb_2, H_RES * V_RES);
	uint32_t full_refresh = 1;
#else
	lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, H_RES * V_RES / 10);
	uint32_t full_refresh = 0;
#endif
	
	lv_disp_drv_init(&disp_drv);		/*Basic initialization*/
	disp_drv.draw_buf = &disp_buf;	/*Set an initialized buffer*/
	disp_drv.flush_cb = my_flush_cb;	/*Set a flush callback to draw to the display*/
	disp_drv.hor_res = H_RES;		/*Set the horizontal resolution in pixels*/
	disp_drv.ver_res = V_RES;		/*Set the vertical resolution in pixels*/
	disp_drv.full_refresh = full_refresh;

	static lv_disp_t * disp;
	disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/

	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);		/*Basic initialization*/
	indev_drv.type = LV_INDEV_TYPE_POINTER;	/*See below.*/
	indev_drv.read_cb = my_input_read;		/*See below.*/
	/*Register the driver in LVGL and save the created input device object*/
	lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
	
	HAL_TIM_Base_Start_IT(&htim2);
	
	/* lv_demo_widgets requires LV_USE_DEMO_WIDGETS be set to 1 in lv_conf.h */
	lv_demo_widgets();

	/* lv_demo_benchmark requires LV_USE_DEMO_BENCHMARK be set to 1 in lv_conf.h */
//	lv_demo_benchmark();

	/* lv_demo_music requires LV_USE_DEMO_MUSIC, LV_FONT_MONTSERRAT_12, and LV_FONT_MONTSERRAT_16
	   be set to 1 in lv_conf.h */
//	lv_demo_music();

	/* lv_demo_stress requires LV_USE_DEMO_STRESS be set to 1 in lv_conf.h */
//	lv_demo_stress();
	
	while (1) {
		if(run_lv_task_handler) {
			run_lv_task_handler = 0;
			lv_task_handler();
		}
	}
}

void my_flush_cb(lv_disp_drv_t * disp_drv_f, const lv_area_t * area, lv_color_t * color_p) {
	/*Return if the area is out the screen*/
	if (area->x2 < 0) return;
	if (area->y2 < 0) return;
	if (area->x1 > H_RES - 1) return;
	if (area->y1 > V_RES - 1) return;

	/*Truncate the area to the screen*/
	int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
	int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
	int32_t act_x2 = area->x2 > H_RES - 1 ? H_RES - 1 : area->x2;
	int32_t act_y2 = area->y2 > V_RES - 1 ? V_RES - 1 : area->y2;
#if DOUBLE_BUFFER
	Set_Framebuffer((uint32_t)color_p);
	lv_disp_flush_ready(&disp_drv);
#elif MDMA_PIXEL_XFER
	uint32_t x_size = ((uint32_t)act_x2 - (uint32_t)act_x1 + 1) * 4;
	uint32_t y_size = (uint32_t)act_y2 - (uint32_t)act_y1 + 1;
	uint32_t src_addr = (uint32_t)color_p;
	uint32_t dest_addr = (uint32_t)fb_1+(((act_y1 * H_RES)+act_x1)*4);
	MDMA_Config(x_size);
	if(HAL_MDMA_Start_IT(&hmdma,src_addr, dest_addr, x_size, y_size) != HAL_OK) {
		Error_Handler();
	}
#elif DMA_PIXEL_XFER
	x1_flush = act_x1;
	y1_flush = act_y1;
	x2_flush = act_x2;
	y2_fill = act_y2;
	y_fill_act = act_y1;
	buf_to_flush = (uint32_t)color_p;

	if(HAL_DMA_Start_IT(&hdma,
					(uint32_t) buf_to_flush,
					(uint32_t) fb_1+(((y_fill_act * H_RES)+x1_flush)*4),
					(x2_flush - x1_flush + 1)) != HAL_OK) {
		Error_Handler();
	}
#else
	int32_t x, y;
	for(y = area->y1; y <= area->y2; y++) {
		for(x = area->x1; x <= area->x2; x++) {
			*(uint32_t*) ((uint32_t)fb_1 + (4 * ((y * H_RES) + x))) = color_p->full;
			color_p++;
		}
	}
	lv_disp_flush_ready(&disp_drv);
#endif
}

void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t * data) {
	if(touched) {
		touched = 0;
		if(ft6x06_TS_DetectTouch(0x70)) {
			if(H_RES > V_RES) {
				// x and y touch points swapped when in landscape
				ft6x06_TS_GetXY(0x70, &data->point.y, &data->point.x);
				data->point.y = 480 - data->point.y;
			} else {
				ft6x06_TS_GetXY(0x70, &data->point.x, &data->point.y);
			}
			data->state = LV_INDEV_STATE_PRESSED;
		}
	} else {
		data->state = LV_INDEV_STATE_RELEASED;
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
	__HAL_RCC_GPIOF_CLK_ENABLE();

	/* Red and Blue LEDs */
	GPIO_InitStruct.Pin = LED_RED_PIN | LED_BLUE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/* I2C1 SCL */
	GPIO_InitStruct.Pin = I2C1_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_I2C1;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* I2C1 SDA */
	GPIO_InitStruct.Pin = I2C1_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_I2C1;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	/* TouchPanel Interrupt */
	GPIO_InitStruct.Pin = TP_INT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TP_INT_PORT, &GPIO_InitStruct);
	set_interrupt(EXTI2_IRQn, 1, 0, 0b10, tp_int);
}

void TIM2_Init(void)
{
	__HAL_RCC_TIM2_CLK_ENABLE();

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 20814;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 50;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	set_interrupt(TIM2_IRQn, 1, 0, 0, tim2_elapsed);
}

void I2C1_Init() {
	__HAL_RCC_I2C1_CLK_ENABLE();
	hi2c1.Instance = I2C1;
	HAL_I2C_DeInit(&hi2c1);
	hi2c1.Init.Timing = 0x20706A61;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if(HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	if(HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
		Error_Handler();
	}
	if(HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 1) != HAL_OK) {
		Error_Handler();
	}
}

void MDMA_Config(int32_t buf_x_size) {
	__HAL_RCC_MDMA_CLK_ENABLE();
	
	hmdma.Instance = MDMA_Channel0;
	HAL_MDMA_DeInit(&hmdma);

	hmdma.Init.Request 			= MDMA_REQUEST_SW;
	hmdma.Init.TransferTriggerMode 	= MDMA_REPEAT_BLOCK_TRANSFER;
	hmdma.Init.Priority 			= MDMA_PRIORITY_HIGH;
	hmdma.Init.Endianness 			= MDMA_LITTLE_ENDIANNESS_PRESERVE;
	hmdma.Init.DataAlignment 		= MDMA_DATAALIGN_PACKENABLE;
	hmdma.Init.SourceBurst 		= MDMA_SOURCE_BURST_SINGLE;
	hmdma.Init.DestBurst 			= MDMA_DEST_BURST_SINGLE;
	hmdma.Init.BufferTransferLength 	= 128;
	hmdma.Init.SourceBlockAddressOffset  	= 0;
	hmdma.Init.DestBlockAddressOffset 	= (H_RES * 4) - buf_x_size;
	hmdma.Init.SourceDataSize 		= MDMA_SRC_DATASIZE_WORD;
	hmdma.Init.DestDataSize 		= MDMA_DEST_DATASIZE_WORD;
	hmdma.Init.SourceInc 			= MDMA_SRC_INC_WORD;
	hmdma.Init.DestinationInc 		= MDMA_DEST_INC_WORD;

	if (HAL_MDMA_Init(&hmdma) != HAL_OK) {
	Error_Handler();
	}
	HAL_MDMA_RegisterCallback(&hmdma, HAL_MDMA_XFER_CPLT_CB_ID, MDMA_TransferComplete);
	set_interrupt(MDMA_IRQn, 1, 0, 0, MDMA_Callback);
}

void DMA_Config(void) {
	__HAL_RCC_DMA2_CLK_ENABLE();
	__HAL_RCC_DMAMUX_CLK_ENABLE();
	hdma.Instance = DMA2_Stream0;
	hdma.Init.Request = DMA_REQUEST_MEM2MEM;
	hdma.Init.Direction = DMA_MEMORY_TO_MEMORY;
	hdma.Init.PeriphInc = DMA_PINC_ENABLE;
	hdma.Init.MemInc = DMA_MINC_ENABLE;
	hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma.Init.Mode = DMA_NORMAL;
	hdma.Init.Priority = DMA_PRIORITY_HIGH;
	hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hdma.Init.MemBurst = DMA_MBURST_INC4;
	hdma.Init.PeriphBurst = DMA_PBURST_INC4;

	HAL_DMA_DeInit(&hdma);
	if (HAL_DMA_Init(&hdma) != HAL_OK) {
	Error_Handler();
	}
	HAL_DMA_RegisterCallback(&hdma, HAL_DMA_XFER_CPLT_CB_ID, DMA_Complete);
	set_interrupt(DMA2_Stream0_IRQn, 1, 0, 0, DMA_Callback);
}

void MDMA_TransferComplete(MDMA_HandleTypeDef *han) {
	lv_disp_flush_ready(&disp_drv);
}

void MDMA_Callback() {
	HAL_MDMA_IRQHandler(&hmdma);
}

void DMA_Complete(DMA_HandleTypeDef *han) {
	y_fill_act++;

	if (y_fill_act > y2_fill) {
		lv_disp_flush_ready(&disp_drv);
	} else {
		buf_to_flush += (x2_flush - x1_flush + 1)*4;
		if (HAL_DMA_Start_IT(han, (uint32_t) buf_to_flush, 
				(uint32_t) fb_1 + (((y_fill_act * H_RES) + x1_flush)*4),
				(x2_flush - x1_flush + 1)) != HAL_OK) {
	    		Error_Handler();
		}
	}
}

void DMA_Callback() {
	HAL_DMA_IRQHandler(&hdma);
}

void tim2_elapsed() {
	run_lv_task_handler = 1;
	lv_tick_inc(5);
	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
}

void tp_int() {
	touched = 1;
	__HAL_GPIO_EXTI_CLEAR_FALLING_IT(TP_INT_PIN);
}

void TS_IO_Init(void) {

}

void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value) {
	uint8_t data[] = {Reg, Value};
	HAL_I2C_Master_Transmit(&hi2c1, Addr, data, 2, 100);
}

uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg) {
	uint8_t data;
	HAL_I2C_Mem_Read(&hi2c1, Addr, Reg, 1, &data, 1, 100);
	return data;
}

uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length) {
	HAL_I2C_Mem_Read(&hi2c1, Addr, Reg, 1, Buffer, Length, 100);
	return 0;
}

void TS_IO_Delay(uint32_t Delay) {
	HAL_Delay(Delay);
}

void set_interrupt(uint32_t irq_num, uint32_t cpu, uint32_t priority, uint32_t edge_detect, void* callback) {
	GIC_DisableIRQ(irq_num);
	GIC_SetTarget(irq_num, cpu);
	GIC_SetPriority(irq_num, priority);
	GIC_SetConfiguration(irq_num, edge_detect);
	irq_callback[irq_num] = callback;
	GIC_ClearPendingIRQ(irq_num);
	GIC_EnableIRQ(irq_num);
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

void __attribute__((interrupt("IRQ"))) IRQ_Handler() {
	uint32_t irq_num = GIC_AcknowledgePending();
	irq_callback[irq_num]();
	GIC_EndInterrupt(irq_num);
}

void Error_Handler() {
	while(1) {
		// handle error
	}
}
