/**
 ******************************************************************************
 * @file    usbh_midi.cc
 * @author  MCD Application Team
 * @brief   This file is the for USB Host MIDI subclass of Audio Class.
 *
 *  @verbatim
 *
 *          ===================================================================
 *                                CDC Class Driver Description
 *          ===================================================================
 *           This driver manages the "Universal Serial Bus Class Definitions for Communications Devices
 *           Revision 1.2 November 16, 2007" and the sub-protocol specification of "Universal Serial Bus
 *           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
 *           This driver implements the following aspects of the specification:
 *             - Device descriptor management
 *             - Configuration descriptor management
 *             - Enumeration as CDC device with 2 data endpoints (IN and OUT) and 1 command endpoint (IN)
 *             - Requests management (as described in section 6.2 in specification)
 *             - Abstract Control Model compliant
 *             - Union Functional collection (using 1 IN endpoint for control)
 *             - Data interface class
 *
 *  @endverbatim
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                      www.st.com/SLA0044
 *
 ******************************************************************************
 */

#include "usbh_midi.hh"

#define USBH_MIDI_BUFFER_SIZE 1024

static USBH_StatusTypeDef USBH_MIDI_InterfaceInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_MIDI_InterfaceDeInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_MIDI_Process(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_MIDI_SOFProcess(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_MIDI_ClassRequest(USBH_HandleTypeDef *phost);

static void MIDI_ProcessTransmission(USBH_HandleTypeDef *phost);
static void MIDI_ProcessReception(USBH_HandleTypeDef *phost);

constexpr uint8_t AudioClassCode = 0x02;
constexpr uint8_t MIDISubclassCode = 0x02;
constexpr uint8_t AnyProtocol = 0xFF;

USBH_ClassTypeDef MIDI_Class = {
	"MIDI",
	AudioClassCode,
	USBH_MIDI_InterfaceInit,
	USBH_MIDI_InterfaceDeInit,
	USBH_MIDI_ClassRequest,
	USBH_MIDI_Process,
	USBH_MIDI_SOFProcess,
	nullptr,
};

class MIDIHost {
public:
	static CDC_HandleTypeDef *new_handle()
	{
		auto handle = static_cast<CDC_HandleTypeDef *>(USBH_malloc(sizeof(CDC_HandleTypeDef)));
		if (handle)
			USBH_memset(handle, 0, sizeof(CDC_HandleTypeDef));

		return handle;
	}
};

/**
 * @brief  USBH_MIDI_InterfaceInit
 *         The function init the MIDI class.
 * @param  phost: Host handle
 * @retval USBH Status
 */
static USBH_StatusTypeDef USBH_MIDI_InterfaceInit(USBH_HandleTypeDef *phost)
{

	USBH_StatusTypeDef status;
	uint8_t interface;

	interface = USBH_FindInterface(phost, AudioClassCode, MIDISubclassCode, AnyProtocol);

	if ((interface == 0xFFU) || (interface >= USBH_MAX_NUM_INTERFACES)) /* No Valid Interface */ {
		USBH_DbgLog("Cannot Find the interface for Communication Interface Class. %s", phost->pActiveClass->Name);
		return USBH_FAIL;
	}

	status = USBH_SelectInterface(phost, interface);
	if (status != USBH_OK)
		return USBH_FAIL;

	auto CDC_Handle = MIDIHost::new_handle();
	phost->pActiveClass->pData = CDC_Handle;

	if (CDC_Handle == nullptr) {
		USBH_DbgLog("Cannot allocate memory for CDC Handle");
		return USBH_FAIL;
	}

	/*Collect the notification endpoint address and length*/
	if (phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].bEndpointAddress & 0x80U) {
		CDC_Handle->CommItf.NotifEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].bEndpointAddress;
		CDC_Handle->CommItf.NotifEpSize = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].wMaxPacketSize;
	}

	/*Allocate the length for host channel number in*/
	CDC_Handle->CommItf.NotifPipe = USBH_AllocPipe(phost, CDC_Handle->CommItf.NotifEp);

	/* Open pipe for Notification endpoint */
	USBH_OpenPipe(phost,
				  CDC_Handle->CommItf.NotifPipe,
				  CDC_Handle->CommItf.NotifEp,
				  phost->device.address,
				  phost->device.speed,
				  USB_EP_TYPE_INTR,
				  CDC_Handle->CommItf.NotifEpSize);

	USBH_LL_SetToggle(phost, CDC_Handle->CommItf.NotifPipe, 0U);

	interface = USBH_FindInterface(phost, DATA_INTERFACE_CLASS_CODE, CDC_RESERVED, NO_CLASS_SPECIFIC_PROTOCOL_CODE);

	if ((interface == 0xFFU) || (interface >= USBH_MAX_NUM_INTERFACES)) /* No Valid Interface */ {
		USBH_DbgLog("Cannot Find the interface for Data Interface Class: %s.", phost->pActiveClass->Name);
		return USBH_FAIL;
	}

	/*Collect the class specific endpoint address and length*/
	{
		auto ep_addr = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].bEndpointAddress;
		auto mps = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].wMaxPacketSize;
		if (ep_addr & 0x80U) {
			CDC_Handle->DataItf.InEp = ep_addr;
			CDC_Handle->DataItf.InEpSize = mps;
		} else {
			CDC_Handle->DataItf.OutEp = ep_addr;
			CDC_Handle->DataItf.OutEpSize = mps;
		}
	}

	if (phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[1].bEndpointAddress & 0x80U) {
		CDC_Handle->DataItf.InEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[1].bEndpointAddress;
		CDC_Handle->DataItf.InEpSize = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[1].wMaxPacketSize;
	} else {
		CDC_Handle->DataItf.OutEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[1].bEndpointAddress;
		CDC_Handle->DataItf.OutEpSize = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[1].wMaxPacketSize;
	}

	/*Allocate the length for host channel number out*/
	CDC_Handle->DataItf.OutPipe = USBH_AllocPipe(phost, CDC_Handle->DataItf.OutEp);

	/*Allocate the length for host channel number in*/
	CDC_Handle->DataItf.InPipe = USBH_AllocPipe(phost, CDC_Handle->DataItf.InEp);

	/* Open channel for OUT endpoint */
	USBH_OpenPipe(phost,
				  CDC_Handle->DataItf.OutPipe,
				  CDC_Handle->DataItf.OutEp,
				  phost->device.address,
				  phost->device.speed,
				  USB_EP_TYPE_BULK,
				  CDC_Handle->DataItf.OutEpSize);

	/* Open channel for IN endpoint */
	USBH_OpenPipe(phost,
				  CDC_Handle->DataItf.InPipe,
				  CDC_Handle->DataItf.InEp,
				  phost->device.address,
				  phost->device.speed,
				  USB_EP_TYPE_BULK,
				  CDC_Handle->DataItf.InEpSize);

	CDC_Handle->state = CDC_IDLE_STATE;

	USBH_LL_SetToggle(phost, CDC_Handle->DataItf.OutPipe, 0U);
	USBH_LL_SetToggle(phost, CDC_Handle->DataItf.InPipe, 0U);

	return USBH_OK;
}

/**
 * @brief  USBH_MIDI_InterfaceDeInit
 *         The function DeInit the Pipes used for the MIDI class.
 * @param  phost: Host handle
 * @retval USBH Status
 */
static USBH_StatusTypeDef USBH_MIDI_InterfaceDeInit(USBH_HandleTypeDef *phost)
{
	CDC_HandleTypeDef *CDC_Handle = (CDC_HandleTypeDef *)phost->pActiveClass->pData;

	if (CDC_Handle->CommItf.NotifPipe) {
		USBH_ClosePipe(phost, CDC_Handle->CommItf.NotifPipe);
		USBH_FreePipe(phost, CDC_Handle->CommItf.NotifPipe);
		CDC_Handle->CommItf.NotifPipe = 0U; /* Reset the Channel as Free */
	}

	if (CDC_Handle->DataItf.InPipe) {
		USBH_ClosePipe(phost, CDC_Handle->DataItf.InPipe);
		USBH_FreePipe(phost, CDC_Handle->DataItf.InPipe);
		CDC_Handle->DataItf.InPipe = 0U; /* Reset the Channel as Free */
	}

	if (CDC_Handle->DataItf.OutPipe) {
		USBH_ClosePipe(phost, CDC_Handle->DataItf.OutPipe);
		USBH_FreePipe(phost, CDC_Handle->DataItf.OutPipe);
		CDC_Handle->DataItf.OutPipe = 0U; /* Reset the Channel as Free */
	}

	if (phost->pActiveClass->pData) {
		USBH_free(phost->pActiveClass->pData);
		phost->pActiveClass->pData = 0U;
	}

	return USBH_OK;
}

/**
 * @brief  USBH_MIDI_ClassRequest
 *         The function is responsible for handling Standard requests
 *         for MIDI class.
 * @param  phost: Host handle
 * @retval USBH Status
 */
static USBH_StatusTypeDef USBH_MIDI_ClassRequest(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status;
	CDC_HandleTypeDef *CDC_Handle = (CDC_HandleTypeDef *)phost->pActiveClass->pData;

	if (status == USBH_OK) {
		phost->pUser(phost, HOST_USER_CLASS_ACTIVE);
	} else if (status == USBH_NOT_SUPPORTED) {
		USBH_ErrLog("Control error: CDC: Device Get Line Coding configuration failed");
	} else {
		/* .. */
	}

	return status;
}

/**
 * @brief  USBH_MIDI_Process
 *         The function is for managing state machine for MIDI data transfers
 * @param  phost: Host handle
 * @retval USBH Status
 */
static USBH_StatusTypeDef USBH_MIDI_Process(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status = USBH_BUSY;
	USBH_StatusTypeDef req_status = USBH_OK;
	CDC_HandleTypeDef *CDC_Handle = (CDC_HandleTypeDef *)phost->pActiveClass->pData;

	switch (CDC_Handle->state) {

		case CDC_IDLE_STATE:
			status = USBH_OK;
			break;

		case CDC_TRANSFER_DATA:
			MIDI_ProcessTransmission(phost);
			MIDI_ProcessReception(phost);
			break;

		case CDC_ERROR_STATE:
			req_status = USBH_ClrFeature(phost, 0x00U);

			if (req_status == USBH_OK) {
				/*Change the state to waiting*/
				CDC_Handle->state = CDC_IDLE_STATE;
			}
			break;

		default:
			break;
	}

	return status;
}

/**
 * @brief  USBH_MIDI_SOFProcess
 *         The function is for managing SOF callback
 * @param  phost: Host handle
 * @retval USBH Status
 */
static USBH_StatusTypeDef USBH_MIDI_SOFProcess(USBH_HandleTypeDef *phost)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(phost);

	return USBH_OK;
}

/**
 * @brief  USBH_MIDI_Stop
 *         Stop current MIDI Transmission
 * @param  phost: Host handle
 * @retval USBH Status
 */
USBH_StatusTypeDef USBH_MIDI_Stop(USBH_HandleTypeDef *phost)
{
	CDC_HandleTypeDef *CDC_Handle = (CDC_HandleTypeDef *)phost->pActiveClass->pData;

	if (phost->gState == HOST_CLASS) {
		CDC_Handle->state = CDC_IDLE_STATE;

		USBH_ClosePipe(phost, CDC_Handle->CommItf.NotifPipe);
		USBH_ClosePipe(phost, CDC_Handle->DataItf.InPipe);
		USBH_ClosePipe(phost, CDC_Handle->DataItf.OutPipe);
	}
	return USBH_OK;
}

/**
 * @brief  This function return last received data size
 * @param  None
 * @retval None
 */
uint16_t USBH_MIDI_GetLastReceivedDataSize(USBH_HandleTypeDef *phost)
{
	uint32_t dataSize;
	CDC_HandleTypeDef *CDC_Handle = (CDC_HandleTypeDef *)phost->pActiveClass->pData;

	if (phost->gState == HOST_CLASS) {
		dataSize = USBH_LL_GetLastXferSize(phost, CDC_Handle->DataItf.InPipe);
	} else {
		dataSize = 0U;
	}

	return (uint16_t)dataSize;
}

/**
 * @brief  This function prepares the state before issuing the class specific commands
 * @param  None
 * @retval None
 */
USBH_StatusTypeDef USBH_MIDI_Transmit(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint32_t length)
{
	USBH_StatusTypeDef Status = USBH_BUSY;
	CDC_HandleTypeDef *CDC_Handle = (CDC_HandleTypeDef *)phost->pActiveClass->pData;

	if ((CDC_Handle->state == CDC_IDLE_STATE) || (CDC_Handle->state == CDC_TRANSFER_DATA)) {
		CDC_Handle->pTxData = pbuff;
		CDC_Handle->TxDataLength = length;
		CDC_Handle->state = CDC_TRANSFER_DATA;
		CDC_Handle->data_tx_state = CDC_SEND_DATA;
		Status = USBH_OK;

#if (USBH_USE_OS == 1U)
		phost->os_msg = (uint32_t)USBH_CLASS_EVENT;
#if (osCMSIS < 0x20000U)
		(void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
		(void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, NULL);
#endif
#endif
	}
	return Status;
}

/**
 * @brief  This function prepares the state before issuing the class specific commands
 * @param  None
 * @retval None
 */
USBH_StatusTypeDef USBH_MIDI_Receive(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint32_t length)
{
	USBH_StatusTypeDef Status = USBH_BUSY;
	CDC_HandleTypeDef *CDC_Handle = (CDC_HandleTypeDef *)phost->pActiveClass->pData;

	if ((CDC_Handle->state == CDC_IDLE_STATE) || (CDC_Handle->state == CDC_TRANSFER_DATA)) {
		CDC_Handle->pRxData = pbuff;
		CDC_Handle->RxDataLength = length;
		CDC_Handle->state = CDC_TRANSFER_DATA;
		CDC_Handle->data_rx_state = CDC_RECEIVE_DATA;
		Status = USBH_OK;

#if (USBH_USE_OS == 1U)
		phost->os_msg = (uint32_t)USBH_CLASS_EVENT;
#if (osCMSIS < 0x20000U)
		(void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
		(void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, NULL);
#endif
#endif
	}
	return Status;
}

/**
 * @brief  The function is responsible for sending data to the device
 *  @param  pdev: Selected device
 * @retval None
 */
static void MIDI_ProcessTransmission(USBH_HandleTypeDef *phost)
{
	CDC_HandleTypeDef *CDC_Handle = (CDC_HandleTypeDef *)phost->pActiveClass->pData;
	USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;

	switch (CDC_Handle->data_tx_state) {
		case CDC_SEND_DATA:
			if (CDC_Handle->TxDataLength > CDC_Handle->DataItf.OutEpSize) {
				USBH_BulkSendData(
					phost, CDC_Handle->pTxData, CDC_Handle->DataItf.OutEpSize, CDC_Handle->DataItf.OutPipe, 1U);
			} else {
				USBH_BulkSendData(
					phost, CDC_Handle->pTxData, (uint16_t)CDC_Handle->TxDataLength, CDC_Handle->DataItf.OutPipe, 1U);
			}

			CDC_Handle->data_tx_state = CDC_SEND_DATA_WAIT;
			break;

		case CDC_SEND_DATA_WAIT:

			URB_Status = USBH_LL_GetURBState(phost, CDC_Handle->DataItf.OutPipe);

			/* Check the status done for transmission */
			if (URB_Status == USBH_URB_DONE) {
				if (CDC_Handle->TxDataLength > CDC_Handle->DataItf.OutEpSize) {
					CDC_Handle->TxDataLength -= CDC_Handle->DataItf.OutEpSize;
					CDC_Handle->pTxData += CDC_Handle->DataItf.OutEpSize;
				} else {
					CDC_Handle->TxDataLength = 0U;
				}

				if (CDC_Handle->TxDataLength > 0U) {
					CDC_Handle->data_tx_state = CDC_SEND_DATA;
				} else {
					CDC_Handle->data_tx_state = CDC_IDLE;
					USBH_MIDI_TransmitCallback(phost);
				}

#if (USBH_USE_OS == 1U)
				phost->os_msg = (uint32_t)USBH_CLASS_EVENT;
#if (osCMSIS < 0x20000U)
				(void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
				(void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, NULL);
#endif
#endif
			} else {
				if (URB_Status == USBH_URB_NOTREADY) {
					CDC_Handle->data_tx_state = CDC_SEND_DATA;

#if (USBH_USE_OS == 1U)
					phost->os_msg = (uint32_t)USBH_CLASS_EVENT;
#if (osCMSIS < 0x20000U)
					(void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
					(void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, NULL);
#endif
#endif
				}
			}
			break;

		default:
			break;
	}
}
/**
 * @brief  This function responsible for reception of data from the device
 *  @param  pdev: Selected device
 * @retval None
 */

static void MIDI_ProcessReception(USBH_HandleTypeDef *phost)
{
	CDC_HandleTypeDef *CDC_Handle = (CDC_HandleTypeDef *)phost->pActiveClass->pData;
	USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
	uint32_t length;

	switch (CDC_Handle->data_rx_state) {

		case CDC_RECEIVE_DATA:

			USBH_BulkReceiveData(phost, CDC_Handle->pRxData, CDC_Handle->DataItf.InEpSize, CDC_Handle->DataItf.InPipe);

			CDC_Handle->data_rx_state = CDC_RECEIVE_DATA_WAIT;

			break;

		case CDC_RECEIVE_DATA_WAIT:

			URB_Status = USBH_LL_GetURBState(phost, CDC_Handle->DataItf.InPipe);

			/*Check the status done for reception*/
			if (URB_Status == USBH_URB_DONE) {
				length = USBH_LL_GetLastXferSize(phost, CDC_Handle->DataItf.InPipe);

				if (((CDC_Handle->RxDataLength - length) > 0U) && (length > CDC_Handle->DataItf.InEpSize)) {
					CDC_Handle->RxDataLength -= length;
					CDC_Handle->pRxData += length;
					CDC_Handle->data_rx_state = CDC_RECEIVE_DATA;
				} else {
					CDC_Handle->data_rx_state = CDC_IDLE;
					USBH_MIDI_ReceiveCallback(phost);
				}

#if (USBH_USE_OS == 1U)
				phost->os_msg = (uint32_t)USBH_CLASS_EVENT;
#if (osCMSIS < 0x20000U)
				(void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
				(void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, NULL);
#endif
#endif
			}
			break;

		default:
			break;
	}
}

/**
 * @brief  The function informs user that data have been received
 *  @param  pdev: Selected device
 * @retval None
 */
__weak void USBH_MIDI_TransmitCallback(USBH_HandleTypeDef *phost)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(phost);
}

/**
 * @brief  The function informs user that data have been sent
 *  @param  pdev: Selected device
 * @retval None
 */
__weak void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(phost);
}
