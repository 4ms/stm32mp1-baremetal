#include "lcd.h"

LTDC_HandleTypeDef hltdc;
DSI_HandleTypeDef hdsi;
uint16_t h_res;
uint16_t v_res;

void LCD_Init(uint8_t orientation, uint32_t framebuffer)
{
	DSI_PLLInitTypeDef dsiPllInit;
	DSI_VidCfgTypeDef hdsi_video;
	uint32_t LcdClock, laneByteClk_kHz, txclkesc;

	uint32_t VSA; /*!< Vertical start active time in units of lines */
	uint32_t VBP; /*!< Vertical Back Porch time in units of lines */
	uint32_t VFP; /*!< Vertical Front Porch time in units of lines */
	uint32_t VACT; /*!< Vertical Active time in units of lines = imageSize Y in pixels to display */
	uint32_t HSA; /*!< Horizontal start active time in units of lcdClk */
	uint32_t HBP; /*!< Horizontal Back Porch time in units of lcdClk */
	uint32_t HFP; /*!< Horizontal Front Porch time in units of lcdClk */
	uint32_t HACT; /*!< Horizontal Active time in units of lcdClk = imageSize X in pixels to display */

	// Hardware reset
	LCD_Reset();

	// Set pixel clock to 29.7 MHz
	__HAL_RCC_DSI_CLK_DISABLE();
	__HAL_RCC_LTDC_CLK_DISABLE();
	RCC->PLL4CR &= ~RCC_PLL4CR_DIVQEN;
	RCC->PLL4CFGR2 &= ~RCC_PLL4CFGR2_DIVQ;
	RCC->PLL4CFGR2 |= (19U << RCC_PLL4CFGR2_DIVQ_Pos);	// DIVQ4 /20
	RCC->PLL4CR |= RCC_PLL4CR_DIVQEN;

	// Enable LTDC and DSI Host clocks
	__HAL_RCC_LTDC_CLK_ENABLE();
	__HAL_RCC_DSI_CLK_ENABLE();

/*************************DSI Initialization***********************************/

	hdsi.Instance = DSI;

	if(HAL_DSI_DeInit(&hdsi) != HAL_OK) {
		Error_Handler();
	}

	dsiPllInit.PLLNDIV	= 107;
	dsiPllInit.PLLIDF	 = DSI_PLL_IN_DIV3;
	dsiPllInit.PLLODF	= DSI_PLL_OUT_DIV2;

	laneByteClk_kHz = (HSE_VALUE / dsiPllInit.PLLIDF * 2 * dsiPllInit.PLLNDIV / 2 / (1 << dsiPllInit.PLLODF) / 8) / 1000;
	LcdClock = 29700;

	// Set number of Lanes
	hdsi.Init.NumberOfLanes = DSI_TWO_DATA_LANES;

	hdsi.Init.TXEscapeCkdiv = 3;

	hdsi.Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_ENABLE;

	if(HAL_DSI_Init(&(hdsi), &(dsiPllInit)) != HAL_OK) {
		Error_Handler();
	}

	if(orientation == 0)
	{
		h_res = OTM8009A_480X800_WIDTH;	/* 480 */
		v_res = OTM8009A_480X800_HEIGHT; 	/* 800 */
	}
	else
	{
		/* lcd_orientation == LCD_ORIENTATION_LANDSCAPE */
		h_res = OTM8009A_800X480_WIDTH;	/* 800 */
		v_res = OTM8009A_800X480_HEIGHT; 	/* 480 */
	}

	HACT = h_res;
	VACT = v_res;

	/* The following values are same for portrait and landscape orientations */
	VSA	= OTM8009A_480X800_VSYNC;
	VBP	= OTM8009A_480X800_VBP;
	VFP	= OTM8009A_480X800_VFP;
	HSA	= OTM8009A_480X800_HSYNC;
	HBP	= OTM8009A_480X800_HBP;
	HFP	= OTM8009A_480X800_HFP;

	hdsi_video.VirtualChannelID = LCD_OTM8009A_ID;
	hdsi_video.ColorCoding = DSI_RGB888;
	hdsi_video.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
	hdsi_video.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
	hdsi_video.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
	hdsi_video.Mode = DSI_VID_MODE_BURST;
	hdsi_video.NullPacketSize = 0;
	hdsi_video.NumberOfChunks = 0;
	hdsi_video.PacketSize					= HACT;
	hdsi_video.HorizontalSyncActive			= (HSA * laneByteClk_kHz)/LcdClock;
	hdsi_video.HorizontalBackPorch			= (HBP * laneByteClk_kHz)/LcdClock;
	hdsi_video.HorizontalLine				= ((HACT + HSA + HBP + HFP) * laneByteClk_kHz)/LcdClock;
	hdsi_video.VerticalSyncActive				= VSA;
	hdsi_video.VerticalBackPorch				= VBP;
	hdsi_video.VerticalFrontPorch				= VFP;
	hdsi_video.VerticalActive				= VACT;

	// Enable sending commands in mode LP
	hdsi_video.LPCommandEnable = DSI_LP_COMMAND_ENABLE;

	// Largest packet size for VSA, VBP, VFP regions in LP mode
	hdsi_video.LPLargestPacketSize = 16;

	// Largest packet size for HFP region during VACT period in LP mode
	hdsi_video.LPVACTLargestPacketSize = 4;

	/* Specify for each region of the video frame, if the transmission of command in LP mode is allowed in this region */
	/* while streaming is active in video mode */
	hdsi_video.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;	 /* Allow sending LP commands during HFP period */
	hdsi_video.LPHorizontalBackPorchEnable = DSI_LP_HBP_ENABLE;	 /* Allow sending LP commands during HBP period */
	hdsi_video.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;	/* Allow sending LP commands during VACT period */
	hdsi_video.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;	 /* Allow sending LP commands during VFP period */
	hdsi_video.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;	 /* Allow sending LP commands during VBP period */
	hdsi_video.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE; /* Allow sending LP commands during VSync = VSA period */

	/* Configure DSI Video mode timings with settings set above */
	if(HAL_DSI_ConfigVideoMode(&(hdsi), &(hdsi_video)) != HAL_OK) {
		Error_Handler();
	}

/*************************End DSI Initialization*******************************/


/************************LTDC Initialization***********************************/

	/* Timing Configuration */
	hltdc.Init.HorizontalSync = HSA - 1;
	hltdc.Init.AccumulatedHBP = HSA + HBP - 1;
	hltdc.Init.AccumulatedActiveW = HACT + HSA + HBP - 1;
	hltdc.Init.TotalWidth = HACT + HSA + HBP + HFP - 1;

	/* Initialize the LCD pixel width and pixel height */
	hltdc.LayerCfg->ImageWidth = HACT;
	hltdc.LayerCfg->ImageHeight = VACT;

	/* Background value */
	hltdc.Init.Backcolor.Blue = 0;
	hltdc.Init.Backcolor.Green = 0;
	hltdc.Init.Backcolor.Red = 0;
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
	hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
	hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
	hltdc.Instance = LTDC;

	/* Get LTDC Configuration from DSI Configuration */
	if(HAL_LTDCEx_StructInitFromVideoConfig(&(hltdc), &(hdsi_video)) != HAL_OK) {
		Error_Handler();
	}

	/* Initialize the LTDC */
	if(HAL_LTDC_Init(&hltdc) != HAL_OK) {
		Error_Handler();
	}

	/* Enable the DSI host and wrapper after the LTDC initialization
		 To avoid any synchronization issue, the DSI shall be started after enabling the LTDC */
	if(HAL_DSI_Start(&hdsi) != HAL_OK) {
		Error_Handler();
	}

/************************End LTDC Initialization*******************************/


/***********************OTM8009A Initialization********************************/

	/* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver)
	*	depending on configuration set in 'hdsi_video'.
	*/
	if(OTM8009A_Init(OTM8009A_FORMAT_RGB888, orientation)) {
		Error_Handler();
	}

/***********************End OTM8009A Initialization****************************/

	LTDC_LayerCfgTypeDef Layercfg;

	/* Layer Init */
	Layercfg.WindowX0 = 0;
	Layercfg.WindowX1 = HACT;
	Layercfg.WindowY0 = 0;
	Layercfg.WindowY1 = VACT;
	Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
	Layercfg.FBStartAdress = framebuffer;
	Layercfg.Alpha = 255;
	Layercfg.Alpha0 = 0;
	Layercfg.Backcolor.Blue = 0;
	Layercfg.Backcolor.Green = 0;
	Layercfg.Backcolor.Red = 0;
	Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	Layercfg.ImageWidth = HACT;
	Layercfg.ImageHeight = VACT;

	if(HAL_LTDC_ConfigLayer(&hltdc, &Layercfg, 0) != HAL_OK) {
		Error_Handler();
	}
}

void LCD_Reset() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(10);
}

uint16_t LCD_GetXSize() {
	return h_res;
}

uint16_t LCD_GetYSize() {
	return v_res;
}

void Set_Framebuffer(uint32_t fb) {
	HAL_LTDC_SetAddress(&hltdc, fb, 0);
}

void OTM8009A_IO_Delay(uint32_t Delay)
{
	HAL_Delay(Delay);
}

void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams)
{
	if(NbrParams <= 1) {
		HAL_DSI_ShortWrite(&hdsi, LCD_OTM8009A_ID, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]);
	}
	else {
		HAL_DSI_LongWrite(&hdsi, LCD_OTM8009A_ID, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[NbrParams], pParams);
	}
}

__weak void Error_Handler() {
	while(1) ;
}
