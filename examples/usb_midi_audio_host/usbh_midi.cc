/**
 ******************************************************************************
 * @file    usbh_midi.cc
 * @author  Dan Green. Based on CDC Class in STM32 USB Host Library v3.5.0
 * @brief   USB Host Audio Class MIDI Streaming Subclass driver
 *
 *   See "Universal Serial Bus Device Class Definition for MIDI Devices"
 *   Revision 1.0 November 1, 1999
 *
 ******************************************************************************
 * @attention
 * Copyright (c) 2022 Dan Green.
 * Licensed by the MIT License, see LICENSE file
 *
 * Portions of this file may be also:
 * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
 * All rights reserved.</center></h2>
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                      www.st.com/SLA0044
 *
 ******************************************************************************
 */

#include "usbh_midi.hh"

static void MIDI_ProcessTransmission(USBH_HandleTypeDef *phost);
static void MIDI_ProcessReception(USBH_HandleTypeDef *phost);

/**
 * @brief  USBH_MIDI_InterfaceInit
 *         The function init the MIDI class.
 * @param  phost: Host handle
 * @retval USBH Status
 */
USBH_StatusTypeDef USBH_MIDI_InterfaceInit(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status;
	uint8_t interface;

	// In most STM Host Lib Classes, we USBH_malloc() a class handle here.
	// But in this class, we already specified it by passing it in the pData field
	// of the USBH_ClassTypeDef struct passed to USBH_RegisterClass
	// This allows the app to own the class handle, managing its memory as it likes
	// without requiring either dynamic memory or static/globals/singletons

	if (phost->pActiveClass->pData == nullptr) {
		USBH_DbgLog("Cannot allocate memory for CDC Handle");
		return USBH_FAIL;
	}

	USBHostHelper host{phost};
	auto MSHandle = host.get_class_handle<MidiStreamingHandle>();

	// Look for an optional Audio Control interface
	interface = USBH_FindInterface(phost, AudioClassCode, AudioControlSubclassCode, AnyProtocol);
	if ((interface == NoValidInterfaceFound) || (interface >= USBH_MAX_NUM_INTERFACES)) {
		USBH_DbgLog("Did not find an audio control interface, continuing\n");
	} else {
		USBH_DbgLog("Found Audio Control subclass\n");
		host.link_endpoint_pipe(MSHandle->ControlItf.ControlEP, interface, 0);
		host.open_pipe(MSHandle->ControlItf.ControlEP, EndPointType::Intr); // TODO: Is it an Intr EP type?
		host.set_toggle(MSHandle->ControlItf.ControlEP, 0);
	}

	// Look for MidiStreamingSubClass
	interface = USBH_FindInterface(phost, AudioClassCode, MidiStreamingSubClass, AnyProtocol);
	if ((interface == NoValidInterfaceFound) || (interface >= USBH_MAX_NUM_INTERFACES)) {
		USBH_DbgLog("Cannot find the interface for MIDI subclass: %s.", phost->pActiveClass->Name);
		return USBH_FAIL;
	}

	status = USBH_SelectInterface(phost, interface);
	if (status != USBH_OK)
		return USBH_FAIL;

	if (host.is_in_ep(interface, 0))
		host.link_endpoint_pipe(MSHandle->DataItf.InEP, interface, 0);
	else
		host.link_endpoint_pipe(MSHandle->DataItf.OutEP, interface, 0);

	if (host.is_in_ep(interface, 1))
		host.link_endpoint_pipe(MSHandle->DataItf.InEP, interface, 1);
	else
		host.link_endpoint_pipe(MSHandle->DataItf.OutEP, interface, 1);

	host.open_pipe(MSHandle->DataItf.OutEP, EndPointType::Bulk);
	host.open_pipe(MSHandle->DataItf.InEP, EndPointType::Bulk);

	MSHandle->state = MidiStreamingState::Idle;

	host.set_toggle(MSHandle->DataItf.OutEP, 0);
	host.set_toggle(MSHandle->DataItf.InEP, 0);

	return USBH_OK;
}

/**
 * @brief  USBH_MIDI_InterfaceDeInit
 *         The function DeInit the Pipes used for the MIDI class.
 * @param  phost: Host handle
 * @retval USBH Status
 */
USBH_StatusTypeDef USBH_MIDI_InterfaceDeInit(USBH_HandleTypeDef *phost)
{
	USBHostHelper host{phost};
	auto MSHandle = host.get_class_handle<MidiStreamingHandle>();
	if (!MSHandle)
		return USBH_FAIL;

	host.close_and_free_pipe(MSHandle->ControlItf.ControlEP);
	host.close_and_free_pipe(MSHandle->DataItf.InEP);
	host.close_and_free_pipe(MSHandle->DataItf.OutEP);

	// USBH_free(phost->pActiveClass->pData);
	// phost->pActiveClass->pData = nullptr;

	return USBH_OK;
}

/**
 * @brief  USBH_MIDI_ClassRequest
 *         The function is responsible for handling Standard requests
 *         for MIDI class.
 * @param  phost: Host handle
 * @retval USBH Status
 */
USBH_StatusTypeDef USBH_MIDI_ClassRequest(USBH_HandleTypeDef *phost)
{
	if (phost->pUser)
		phost->pUser(phost, HOST_USER_CLASS_ACTIVE);

	return USBH_OK;
}

/**
 * @brief  USBH_MIDI_Process
 *         The function is for managing state machine for MIDI data transfers
 * @param  phost: Host handle
 * @retval USBH Status
 */
USBH_StatusTypeDef USBH_MIDI_Process(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status = USBH_BUSY;

	USBHostHelper host{phost};
	auto MSHandle = host.get_class_handle<MidiStreamingHandle>();
	if (!MSHandle)
		return USBH_FAIL;

	switch (MSHandle->state) {
		case MidiStreamingState::Idle:
			status = USBH_OK;
			break;

		case MidiStreamingState::TransferData:
			MIDI_ProcessTransmission(phost);
			MIDI_ProcessReception(phost);
			break;

		case MidiStreamingState::Error: {
			USBH_StatusTypeDef req_status = USBH_ClrFeature(phost, 0x00U);

			if (req_status == USBH_OK) {
				MSHandle->state = MidiStreamingState::Idle;
			}
		} break;
	}

	return status;
}

/**
 * @brief  USBH_MIDI_SOFProcess
 *         The function is for managing SOF callback
 * @param  phost: Host handle
 * @retval USBH Status
 */
USBH_StatusTypeDef USBH_MIDI_SOFProcess(USBH_HandleTypeDef *phost)
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
	USBHostHelper host{phost};
	auto MSHandle = host.get_class_handle<MidiStreamingHandle>();
	if (!MSHandle)
		return USBH_FAIL;

	if (phost->gState == HOST_CLASS) {
		MSHandle->state = MidiStreamingState::Idle;

		USBH_ClosePipe(phost, MSHandle->ControlItf.ControlEP.pipe);
		USBH_ClosePipe(phost, MSHandle->DataItf.InEP.pipe);
		USBH_ClosePipe(phost, MSHandle->DataItf.OutEP.pipe);
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
	USBHostHelper host{phost};
	auto MSHandle = host.get_class_handle<MidiStreamingHandle>();
	if (!MSHandle)
		return USBH_FAIL;

	uint32_t dataSize;

	if (phost->gState == HOST_CLASS) {
		dataSize = USBH_LL_GetLastXferSize(phost, MSHandle->DataItf.InEP.pipe);
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
	USBHostHelper host{phost};
	auto MSHandle = host.get_class_handle<MidiStreamingHandle>();
	if (!MSHandle)
		return USBH_FAIL;

	USBH_StatusTypeDef status = USBH_BUSY;

	if ((MSHandle->state == MidiStreamingState::Idle) || (MSHandle->state == MidiStreamingState::TransferData)) {
		MSHandle->pTxData = pbuff;
		MSHandle->TxDataLength = length;
		MSHandle->state = MidiStreamingState::TransferData;
		MSHandle->data_tx_state = MidiStreamingDataState::SendData;
		status = USBH_OK;

#if (USBH_USE_OS == 1U)
		phost->os_msg = (uint32_t)USBH_CLASS_EVENT;
		(void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, NULL);
#endif
	}
	return status;
}

/**
 * @brief  This function prepares the state before issuing the class specific commands
 * @param  None
 * @retval None
 */
USBH_StatusTypeDef USBH_MIDI_Receive(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint32_t length)
{
	USBHostHelper host{phost};
	auto MSHandle = host.get_class_handle<MidiStreamingHandle>();
	if (!MSHandle)
		return USBH_FAIL;

	USBH_StatusTypeDef status = USBH_BUSY;

	if ((MSHandle->state == MidiStreamingState::Idle) || (MSHandle->state == MidiStreamingState::TransferData)) {
		MSHandle->pRxData = pbuff;
		MSHandle->RxDataLength = length;
		MSHandle->state = MidiStreamingState::TransferData;
		MSHandle->data_rx_state = MidiStreamingDataState::ReceiveData;
		status = USBH_OK;

#if (USBH_USE_OS == 1U)
		phost->os_msg = (uint32_t)USBH_CLASS_EVENT;
		(void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, NULL);
#endif
	}
	return status;
}

/**
 * @brief  The function is responsible for sending data to the device
 *  @param  pdev: Selected device
 * @retval None
 */
static void MIDI_ProcessTransmission(USBH_HandleTypeDef *phost)
{
	USBHostHelper host{phost};
	auto MSHandle = host.get_class_handle<MidiStreamingHandle>();
	if (!MSHandle)
		return;

	USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;

	switch (MSHandle->data_tx_state) {
		case MidiStreamingDataState::SendData:
			if (MSHandle->TxDataLength > MSHandle->DataItf.OutEP.size) {
				USBH_BulkSendData(
					phost, MSHandle->pTxData, MSHandle->DataItf.OutEP.size, MSHandle->DataItf.OutEP.pipe, 1U);
			} else {
				USBH_BulkSendData(
					phost, MSHandle->pTxData, (uint16_t)MSHandle->TxDataLength, MSHandle->DataItf.OutEP.pipe, 1U);
			}

			MSHandle->data_tx_state = MidiStreamingDataState::SendDataWait;
			break;

		case MidiStreamingDataState::SendDataWait:

			URB_Status = USBH_LL_GetURBState(phost, MSHandle->DataItf.OutEP.pipe);

			/* Check the status done for transmission */
			if (URB_Status == USBH_URB_DONE) {
				if (MSHandle->TxDataLength > MSHandle->DataItf.OutEP.size) {
					MSHandle->TxDataLength -= MSHandle->DataItf.OutEP.size;
					MSHandle->pTxData += MSHandle->DataItf.OutEP.size;
				} else {
					MSHandle->TxDataLength = 0U;
				}

				if (MSHandle->TxDataLength > 0U) {
					MSHandle->data_tx_state = MidiStreamingDataState::SendData;
				} else {
					MSHandle->data_tx_state = MidiStreamingDataState::Idle;
					MSHandle->tx_callback();
				}

#if (USBH_USE_OS == 1U)
				phost->os_msg = (uint32_t)USBH_CLASS_EVENT;
				(void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, NULL);
#endif
			} else {
				if (URB_Status == USBH_URB_NOTREADY) {
					MSHandle->data_tx_state = MidiStreamingDataState::SendData;

#if (USBH_USE_OS == 1U)
					phost->os_msg = (uint32_t)USBH_CLASS_EVENT;
					(void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, NULL);
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
	USBHostHelper host{phost};
	auto MSHandle = host.get_class_handle<MidiStreamingHandle>();
	if (!MSHandle)
		return;

	USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
	uint32_t length;

	switch (MSHandle->data_rx_state) {

		case MidiStreamingDataState::ReceiveData:
			USBH_BulkReceiveData(phost, MSHandle->pRxData, MSHandle->DataItf.InEP.size, MSHandle->DataItf.InEP.pipe);
			MSHandle->data_rx_state = MidiStreamingDataState::ReceiveDataWait;
			break;

		case MidiStreamingDataState::ReceiveDataWait:
			URB_Status = USBH_LL_GetURBState(phost, MSHandle->DataItf.InEP.pipe);

			/*Check the status done for reception*/
			if (URB_Status == USBH_URB_DONE) {
				length = USBH_LL_GetLastXferSize(phost, MSHandle->DataItf.InEP.pipe);

				if (((MSHandle->RxDataLength - length) > 0U) && (length > MSHandle->DataItf.InEP.size)) {
					MSHandle->RxDataLength -= length;
					MSHandle->pRxData += length;
					MSHandle->data_rx_state = MidiStreamingDataState::ReceiveData;
				} else {
					MSHandle->data_rx_state = MidiStreamingDataState::Idle;
					MSHandle->rx_callback(MSHandle->pRxData, length);
				}

#if (USBH_USE_OS == 1U)
				phost->os_msg = (uint32_t)USBH_CLASS_EVENT;
				(void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, NULL);
#endif
			}
			break;

		default:
			break;
	}
}

// Note: To use this class like other STM32 Host Classes,
// Do something like this (in your application code):

// static MidiStreamingHandle MSHandle;
//
// USBH_ClassTypeDef MIDI_Class_Ops = {
// 	"MIDI",
// 	AudioClassCode,
// 	USBH_MIDI_InterfaceInit,
// 	USBH_MIDI_InterfaceDeInit,
// 	USBH_MIDI_ClassRequest,
// 	USBH_MIDI_Process,
// 	USBH_MIDI_SOFProcess,
// 	&MSHandle,
// };
