/**
 ******************************************************************************
 * @file    USB_Device/MSC_Standalone/Src/usbd_conf.c
 * @author  MCD Application Team
 * @brief   This file implements the USB Device library callbacks and MSP
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------ */
#include "stm32mp1xx.h"
#include "usbd_core.h"
#include "usbd_msc.h"
#include "drivers/cache-c.h"

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
PCD_HandleTypeDef hpcd;

/* Private function prototypes ----------------------------------------------- */
/* Private functions --------------------------------------------------------- */

/*******************************************************************************
					   PCD BSP Routines
*******************************************************************************/

/**
 * @brief  Initializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd) {
	GPIO_InitTypeDef GPIO_InitStruct;

	if (hpcd->Instance == USB_OTG_HS) {
		// /* Configure VBUS Pin */
		// GPIO_InitStruct.Pin = GPIO_PIN_9;
		// GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		// GPIO_InitStruct.Pull = GPIO_NOPULL;
		// HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// /* Configure ID pin */
		// GPIO_InitStruct.Pin = GPIO_PIN_10;
		// GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		// GPIO_InitStruct.Pull = GPIO_PULLUP;
		// GPIO_InitStruct.Alternate = GPIO_AF10_OTG2_FS;
		// HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		__HAL_RCC_USBO_FORCE_RESET();
		__HAL_RCC_USBO_RELEASE_RESET();
		__HAL_RCC_USBPHY_FORCE_RESET();
		__HAL_RCC_USBPHY_RELEASE_RESET();

		__HAL_RCC_USBO_CLK_ENABLE();
		__HAL_RCC_USBPHY_CLK_ENABLE();

		GIC_SetTarget(OTG_IRQn, 1);
		GIC_SetPriority(OTG_IRQn, 0b01111000);
		GIC_SetConfiguration(OTG_IRQn, 0b10); // Edge triggered
		GIC_EnableIRQ(OTG_IRQn);

		USBD_DbgLog("Initialized MSP\r\n");
	}
}

/**
 * @brief  De-Initializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd) {
	if (hpcd->Instance == USB_OTG_HS) {
		__HAL_RCC_USBO_CLK_DISABLE();
	}
}

/*******************************************************************************
					   LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/

/**
 * @brief  SetupStage callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) {
	USBD_LL_SetupStage(hpcd->pData, (uint8_t *)hpcd->Setup);
}

/**
 * @brief  DataOut Stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
 * @brief  DataIn Stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

/**
 * @brief  SOF callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) {
	USBD_LL_SOF(hpcd->pData);
}

/**
 * @brief  Reset callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) {
	USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

	/* Set USB Current Speed */
	switch (hpcd->Init.speed) {
		case PCD_SPEED_HIGH:
			speed = USBD_SPEED_HIGH;
			break;

		case PCD_SPEED_FULL:
			speed = USBD_SPEED_FULL;
			break;

		default:
			speed = USBD_SPEED_FULL;
			break;
	}

	/* Reset Device */
	USBD_LL_Reset(hpcd->pData);

	USBD_LL_SetSpeed(hpcd->pData, speed);
}

/**
 * @brief  Suspend callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd) {
	USBD_LL_Suspend(hpcd->pData);
	__HAL_PCD_GATE_PHYCLOCK(hpcd); // added by hfrtx
}

/**
 * @brief  Resume callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd) {
	USBD_LL_Resume(hpcd->pData);
}

/**
 * @brief  ISOOUTIncomplete callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

/**
 * @brief  ISOINIncomplete callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

/**
 * @brief  ConnectCallback callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd) {
	USBD_LL_DevConnected(hpcd->pData);
}

/**
 * @brief  Disconnect callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd) {
	USBD_LL_DevDisconnected(hpcd->pData);
}

// Added by hftrx 
void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg) { 
	switch (msg) { 
		case PCD_LPM_L0_ACTIVE: 
			if (hpcd->Init.low_power_enable) { 
				////      SystemClock_Config(); 
				/* Reset SLEEPDEEP bit of Cortex System Control Register. */ 
				////      SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk)); 
			} 
			__HAL_PCD_UNGATE_PHYCLOCK(hpcd); 
			USBD_LL_Resume(hpcd->pData); 
			break; 
 
		case PCD_LPM_L1_ACTIVE: 
			__HAL_PCD_GATE_PHYCLOCK(hpcd); 
			USBD_LL_Suspend(hpcd->pData); 
 
			/* Enter in STOP mode. */ 
			if (hpcd->Init.low_power_enable) { 
				/* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register. */ 
				////     SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk)); 
			} 
			break; 
	} 
} 


/*******************************************************************************
					   LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/
static uint32_t usbd_makeTXFSIZ(uint_fast16_t base, uint_fast16_t size) {
	return ((uint32_t)size << USB_OTG_DIEPTXF_INEPTXFD_Pos) | ((uint32_t)base << USB_OTG_DIEPTXF_INEPTXSA_Pos) | 0;
}

// Преобразование размера в байтах размера данных в требования к fifo
// Расчет аргумента функции HAL_PCDEx_SetRxFiFo, HAL_PCDEx_SetTxFiFo
static uint_fast16_t size2buff4(uint_fast16_t size) {
	const uint_fast16_t size4 = (size + 3) / 4; // размер в 32-бит значениях
	return MAX(0x10, size4);
}
static void
usbd_fifo_initialize(PCD_HandleTypeDef *hpcd, uint_fast16_t fullsize, uint_fast8_t bigbuff, uint_fast8_t dma) {
	const int add3tx = bigbuff ? 3 : 1; // tx fifo add places
	uint_fast16_t maxoutpacketsize4 = size2buff4(USB_OTG_MAX_EP0_SIZE);
	maxoutpacketsize4 += 6;
	uint_fast8_t numcontrolendpoints = 1;
	uint_fast8_t numoutendpoints = 1;
	uint_fast8_t addplaces = 3;

	const uint_fast16_t full4 = fullsize / 4;
	uint_fast16_t last4 = full4;
	uint_fast16_t base4 = 0;

	// TX
	{
		const uint_fast16_t size4 = 2 * (size2buff4(USB_OTG_MAX_EP0_SIZE) + add3tx);
		// ASSERT(last4 >= size4);
		last4 -= size4;
		hpcd->Instance->DIEPTXF0_HNPTXFSIZ = usbd_makeTXFSIZ(last4, size4);
	}

	// RX
	/* Установить размер RX FIFO -  теперь все что осталоь - используем last4 вместо size4 */
	// (4 * number of control endpoints + 6) +
	// ((largest USB packet used / 4) + 1 for status information) +
	// (2 * number of OUT endpoints) +
	// 1 for Global NAK
	{
		const uint_fast16_t size4 =
			(4 * numcontrolendpoints + 6) + (maxoutpacketsize4 + 1) + (2 * numoutendpoints) + 1 + addplaces;

		// ASSERT(last4 >= size4);
		hpcd->Instance->GRXFSIZ = (hpcd->Instance->GRXFSIZ & ~USB_OTG_GRXFSIZ_RXFD) |
								  (last4 << USB_OTG_GRXFSIZ_RXFD_Pos) | // was: size4 - то что осталось
								  0;
		base4 += size4;
	}

	(void)USB_FlushRxFifo(hpcd->Instance);
	(void)USB_FlushTxFifo(hpcd->Instance, 0x10U); /* all Tx FIFOs */
}

/**
 * @brief  Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev) {
	/* Set LL Driver parameters */
	hpcd.Instance = USB_OTG_HS;
	hpcd.Init.dev_endpoints = 9;
	hpcd.Init.use_dedicated_ep1 = 0;

	/* Be aware that enabling DMA mode will result in data being sent only by
	 * multiple of 4 packet sizes. This is due to the fact that USB DMA does not
	 * allow sending data from non word-aligned addresses. For this specific
	 * application, it is advised to not enable this option unless required. */
	hpcd.Init.dma_enable = 0;
	hpcd.Init.low_power_enable = 0;
	hpcd.Init.lpm_enable = 0;
	hpcd.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	hpcd.Init.Sof_enable = 0;
	hpcd.Init.speed = PCD_SPEED_HIGH;
	hpcd.Init.vbus_sensing_enable = 0;

	/* Link The driver to the stack */
	hpcd.pData = pdev;
	pdev->pData = &hpcd;

	// Force to null, so ST USB library doesn't try to de-init it
	((USBD_HandleTypeDef *)hpcd.pData)->pClassData = NULL;

	if (HAL_PCD_Init(&hpcd) != HAL_OK)
		return USBD_FAIL;

	// HAL_PCDEx_SetRxFiFo(&hpcd, 0x200);
	// HAL_PCDEx_SetTxFiFo(&hpcd, 0, 0x40);
	// HAL_PCDEx_SetTxFiFo(&hpcd, 1, 0x100);

	// hftrx:
	usbd_fifo_initialize(&hpcd, 4096, 1, hpcd.Init.dma_enable);
	// USB_OTG_MAX_EP0_SIZE

	return USBD_OK;
}

/**
 * @brief  De-Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev) {
	HAL_PCD_DeInit(pdev->pData);
	return USBD_OK;
}

/**
 * @brief  Starts the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev) {
	HAL_PCD_Start(pdev->pData);
	return USBD_OK;
}

/**
 * @brief  Stops the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev) {
	HAL_PCD_Stop(pdev->pData);
	return USBD_OK;
}

/**
 * @brief  Opens an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  ep_type: Endpoint Type
 * @param  ep_mps: Endpoint Max Packet Size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps) {
	HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

	return USBD_OK;
}

/**
 * @brief  Closes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_Close(pdev->pData, ep_addr);
	return USBD_OK;
}

/**
 * @brief  Flushes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_Flush(pdev->pData, ep_addr);
	return USBD_OK;
}

/**
 * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
	return USBD_OK;
}

/**
 * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
	return USBD_OK;
}

/**
 * @brief  Returns Stall condition.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Stall (1: Yes, 0: No)
 */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	PCD_HandleTypeDef *hpcd = pdev->pData;

	if ((ep_addr & 0x80) == 0x80) {
		return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
	} else {
		return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
	}
}

/**
 * @brief  Assigns a USB address to the device.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr) {
	HAL_PCD_SetAddress(pdev->pData, dev_addr);
	return USBD_OK;
}

/**
 * @brief  Transmits data over an endpoint.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be sent
 * @param  size: Data size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size) {
	// added by hftrx and DG:
	if (pbuf != NULL && size != 0) {
		L1_CleanDCache_by_Addr((void *)pbuf, size);
	}

	HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
	return USBD_OK;
}

/**
 * @brief  Prepares an endpoint for reception.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be received
 * @param  size: Data size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size) {
	// added by hftrx and DG
	if (pbuf != NULL && size != 0) {
		L1_CleanInvalidateDCache_by_Addr((void *)pbuf, size);
	}

	HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);

	//added by DG
	if (pbuf != NULL && size != 0) {
		L1_InvalidateDCache_by_Addr((void *)pbuf, size);
	}
	return USBD_OK;
}

/**
 * @brief  Returns the last transferred packet size.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Received Data Size
 */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

/**
 * @brief  Static single allocation.
 * @param  size: Size of allocated memory
 * @retval None
 */
void *USBD_static_malloc(uint32_t size) {
	static uint32_t mem[(sizeof(USBD_MSC_BOT_HandleTypeDef) / 4) + 1]; /* On 32-bit boundary */
	return mem;
}

/**
 * @brief  Dummy memory free
 * @param  p: Pointer to allocated  memory address
 * @retval None
 */
void USBD_static_free(void *p) {
}

/**
 * @brief  Delays routine for the USB Device Library.
 * @param  Delay: Delay in ms
 * @retval None
 */
void USBD_LL_Delay(uint32_t Delay) {
	HAL_Delay(Delay);
}
