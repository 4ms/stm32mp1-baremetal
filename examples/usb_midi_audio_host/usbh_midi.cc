/**
 ******************************************************************************
 * @file    usbh_midi.cc
 * @author  MCD Application Team
 * @brief   This file is the for USB Host MIDI subclass of Audio Class.
 *
 *  @verbatim
 *
 *          ===================================================================
 *                        Audio Class MIDI Subclass Driver Description
 *          ===================================================================
 *           This driver manages the "Universal Serial Bus Device Class Definition for MIDI Devices"
 *           Revision 1.0 November 1, 1999".
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

static USBH_StatusTypeDef USBH_MIDI_InterfaceInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_MIDI_InterfaceDeInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_MIDI_Process(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_MIDI_SOFProcess(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_MIDI_ClassRequest(USBH_HandleTypeDef *phost);

static void MIDI_ProcessTransmission(USBH_HandleTypeDef *phost);
static void MIDI_ProcessReception(USBH_HandleTypeDef *phost);

constexpr uint8_t AudioClassCode = 0x01;
constexpr uint8_t AudioControlSubclassCode = 0x01;
constexpr uint8_t MIDISubclassCode = 0x03;

constexpr uint8_t AnyProtocol = 0xFF;
constexpr uint8_t NoValidInterfaceFound = 0xFF;

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

	auto CDC_Handle = new_usbhost_class_handle<MidiStreamingHandle>();
	phost->pActiveClass->pData = CDC_Handle;

	if (CDC_Handle == nullptr) {
		USBH_DbgLog("Cannot allocate memory for CDC Handle");
		return USBH_FAIL;
	}

	USBHostHandle host{phost};

	// Look for an optional Audio Control interface
	interface = USBH_FindInterface(phost, AudioClassCode, AudioControlSubclassCode, AnyProtocol);
	if ((interface == NoValidInterfaceFound) || (interface >= USBH_MAX_NUM_INTERFACES)) {
		USBH_DbgLog("Did not find an audio control interface, continuing\n");
	} else {
		USBH_DbgLog("Found Audio Control subclass\n");
		host.link_endpoint_pipe(CDC_Handle->ControlItf.ControlEP, interface, 0);
		host.open_pipe(CDC_Handle->ControlItf.ControlEP, EndPointType::Intr); // TODO: Is it an Intr EP type?
		host.set_toggle(CDC_Handle->ControlItf.ControlEP, 0);
	}

	interface = USBH_FindInterface(phost, AudioClassCode, MIDISubclassCode, AnyProtocol);
	if ((interface == NoValidInterfaceFound) || (interface >= USBH_MAX_NUM_INTERFACES)) {
		USBH_DbgLog("Cannot find the interface for MIDI subclass: %s.", phost->pActiveClass->Name);
		return USBH_FAIL;
	}

	status = USBH_SelectInterface(phost, interface);
	if (status != USBH_OK)
		return USBH_FAIL;

	if (host.is_in_ep(interface, 0))
		host.link_endpoint_pipe(CDC_Handle->DataItf.InEP, interface, 0);
	else
		host.link_endpoint_pipe(CDC_Handle->DataItf.OutEP, interface, 0);

	if (host.is_in_ep(interface, 1))
		host.link_endpoint_pipe(CDC_Handle->DataItf.InEP, interface, 1);
	else
		host.link_endpoint_pipe(CDC_Handle->DataItf.OutEP, interface, 1);

	host.open_pipe(CDC_Handle->DataItf.OutEP, EndPointType::Bulk);
	host.open_pipe(CDC_Handle->DataItf.InEP, EndPointType::Bulk);

	CDC_Handle->state = CDC_IDLE_STATE;

	host.set_toggle(CDC_Handle->DataItf.OutEP, 0U);
	host.set_toggle(CDC_Handle->DataItf.InEP, 0U);

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
	// USBHostHandle host{phost};
	MidiStreamingHandle *CDC_Handle = (MidiStreamingHandle *)phost->pActiveClass->pData;

	if (CDC_Handle->ControlItf.ControlEP.pipe) {
		USBH_ClosePipe(phost, CDC_Handle->ControlItf.ControlEP.pipe);
		USBH_FreePipe(phost, CDC_Handle->ControlItf.ControlEP.pipe);
		CDC_Handle->ControlItf.ControlEP.pipe = 0U; /* Reset the Channel as Free */
	}

	if (CDC_Handle->DataItf.InEP.pipe) {
		USBH_ClosePipe(phost, CDC_Handle->DataItf.InEP.pipe);
		USBH_FreePipe(phost, CDC_Handle->DataItf.InEP.pipe);
		CDC_Handle->DataItf.InEP.pipe = 0U; /* Reset the Channel as Free */
	}

	if (CDC_Handle->DataItf.OutEP.pipe) {
		USBH_ClosePipe(phost, CDC_Handle->DataItf.OutEP.pipe);
		USBH_FreePipe(phost, CDC_Handle->DataItf.OutEP.pipe);
		CDC_Handle->DataItf.OutEP.pipe = 0U; /* Reset the Channel as Free */
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
	MidiStreamingHandle *CDC_Handle = (MidiStreamingHandle *)phost->pActiveClass->pData;

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
	MidiStreamingHandle *CDC_Handle = (MidiStreamingHandle *)phost->pActiveClass->pData;

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
	MidiStreamingHandle *CDC_Handle = (MidiStreamingHandle *)phost->pActiveClass->pData;

	if (phost->gState == HOST_CLASS) {
		CDC_Handle->state = CDC_IDLE_STATE;

		USBH_ClosePipe(phost, CDC_Handle->ControlItf.ControlEP.pipe);
		USBH_ClosePipe(phost, CDC_Handle->DataItf.InEP.pipe);
		USBH_ClosePipe(phost, CDC_Handle->DataItf.OutEP.pipe);
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
	MidiStreamingHandle *CDC_Handle = (MidiStreamingHandle *)phost->pActiveClass->pData;

	if (phost->gState == HOST_CLASS) {
		dataSize = USBH_LL_GetLastXferSize(phost, CDC_Handle->DataItf.InEP.pipe);
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
	MidiStreamingHandle *CDC_Handle = (MidiStreamingHandle *)phost->pActiveClass->pData;

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
	MidiStreamingHandle *CDC_Handle = (MidiStreamingHandle *)phost->pActiveClass->pData;

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
	MidiStreamingHandle *CDC_Handle = (MidiStreamingHandle *)phost->pActiveClass->pData;
	USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;

	switch (CDC_Handle->data_tx_state) {
		case CDC_SEND_DATA:
			if (CDC_Handle->TxDataLength > CDC_Handle->DataItf.OutEP.size) {
				USBH_BulkSendData(
					phost, CDC_Handle->pTxData, CDC_Handle->DataItf.OutEP.size, CDC_Handle->DataItf.OutEP.pipe, 1U);
			} else {
				USBH_BulkSendData(
					phost, CDC_Handle->pTxData, (uint16_t)CDC_Handle->TxDataLength, CDC_Handle->DataItf.OutEP.pipe, 1U);
			}

			CDC_Handle->data_tx_state = CDC_SEND_DATA_WAIT;
			break;

		case CDC_SEND_DATA_WAIT:

			URB_Status = USBH_LL_GetURBState(phost, CDC_Handle->DataItf.OutEP.pipe);

			/* Check the status done for transmission */
			if (URB_Status == USBH_URB_DONE) {
				if (CDC_Handle->TxDataLength > CDC_Handle->DataItf.OutEP.size) {
					CDC_Handle->TxDataLength -= CDC_Handle->DataItf.OutEP.size;
					CDC_Handle->pTxData += CDC_Handle->DataItf.OutEP.size;
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
	MidiStreamingHandle *CDC_Handle = (MidiStreamingHandle *)phost->pActiveClass->pData;
	USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
	uint32_t length;

	switch (CDC_Handle->data_rx_state) {

		case CDC_RECEIVE_DATA:

			USBH_BulkReceiveData(
				phost, CDC_Handle->pRxData, CDC_Handle->DataItf.InEP.size, CDC_Handle->DataItf.InEP.pipe);

			CDC_Handle->data_rx_state = CDC_RECEIVE_DATA_WAIT;

			break;

		case CDC_RECEIVE_DATA_WAIT:

			URB_Status = USBH_LL_GetURBState(phost, CDC_Handle->DataItf.InEP.pipe);

			/*Check the status done for reception*/
			if (URB_Status == USBH_URB_DONE) {
				length = USBH_LL_GetLastXferSize(phost, CDC_Handle->DataItf.InEP.pipe);

				if (((CDC_Handle->RxDataLength - length) > 0U) && (length > CDC_Handle->DataItf.InEP.size)) {
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
