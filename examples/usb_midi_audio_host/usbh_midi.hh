/**
 ******************************************************************************
 * @file    usbh_cdc.h
 * @author  MCD Application Team
 * @brief   This file contains all the prototypes for the usbh_cdc.c
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

#ifndef __USBH_CDC_H
#define __USBH_CDC_H

#include "usbh_core.h"
#include "usbh_host.hH"

/* States for CDC State Machine */
enum CDC_DataStateTypeDef {
	CDC_IDLE = 0U,
	CDC_SEND_DATA,
	CDC_SEND_DATA_WAIT,
	CDC_RECEIVE_DATA,
	CDC_RECEIVE_DATA_WAIT,
};

enum CDC_StateTypeDef {
	CDC_IDLE_STATE = 0U,
	CDC_TRANSFER_DATA,
	CDC_ERROR_STATE,
};

struct MidiInterfaceHeaderDesc {
	uint8_t bLength;			// Size of this descriptor = 7
	uint8_t bDescriptorType;	// CS_INTERFACE (0x24)
	uint8_t bDescriptorSubType; // MS_HEADER
	uint16_t bcdMSC;			// Version = 0100
	uint16_t wTotalLength;		// combined length of this desc header + all Jack and Element desc
};

struct MidiInJackDesc {
	uint8_t bLength;			// Size of this descriptor = 6
	uint8_t bDescriptorType;	// CS_INTERFACE (0x24)
	uint8_t bDescriptorSubType; // MIDI_IN_JACK
	uint8_t bJackType;			// EMBEDDED or EXTERNAL
	uint8_t bJackID;			// Unique ID for the MIDI IN Jack
	uint8_t iJack;				// index of string descriptor
};

struct MidiOutJackDesc {
	constexpr static size_t MAX_SOURCES = 16;
	uint8_t bLength;			 // Size of this descriptor = 6 + 2*p, where p is number of pins
	uint8_t bDescriptorType;	 // CS_INTERFACE (0x24)
	uint8_t bDescriptorSubType;	 // MIDI_OUT_JACK
	uint8_t bJackType;			 // EMBEDDED or EXTERNAL
	uint8_t bJackID;			 // Unique ID for the MIDI OUT Jack
	uint8_t bNrInputPins;		 // Number of Input Pins of this MIDI OUT Jack: p
	struct Source {				 //
		uint8_t bSourceID;		 // ID of the Entity to which this Input Pin is connected.
		uint8_t bSourcePin;		 // Output Pin number of the Entity to which this Input Pin is connected.
	};							 //
	Source aSource[MAX_SOURCES]; // Array of SourceID and SourcePin, with p entries
	uint8_t iJack;				 // index of string descriptor
};

struct MidiElementDesc {
	constexpr static size_t MAX_SOURCES = 16;
	constexpr static size_t MAX_CAPS = 16;
	uint8_t bLength;				 // Size of this descriptor = 10 + 2*p + n, where p is number of pins
	uint8_t bDescriptorType;		 // CS_INTERFACE (0x24)
	uint8_t bDescriptorSubType;		 // ELEMENT
	uint8_t bElementID;				 // Unique ID for the Element
	uint8_t bNrInputPins;			 // Number of Input Pins of this Element: p
	struct Source {					 //
		uint8_t bSourceID;			 // ID of the Entity to which this Input Pin is connected.
		uint8_t bSourcePin;			 // Output Pin number of the Entity to which this Input Pin is connected.
	};								 //
	Source aSource[MAX_SOURCES];	 // Array of SourceID and SourcePin, with p entries
	uint8_t bNrOutputPins;			 // Number of Output Pins of this Element: q
	uint8_t bInTerminalLink;		 // The Terminal ID of the Input Terminal to which this Element is connected
	uint8_t bOutTerminalLink;		 // The Terminal ID of the Output Terminal to which this Element is connected
	uint8_t bElCapsSize;			 // Number of bytes in the bmElementCaps field. n
	uint8_t bmElementCaps[MAX_CAPS]; // capabilities array, size n
	uint8_t iJack;					 // index of string descriptor
};

struct MidiInterfaceDesc {
	MidiInterfaceHeaderDesc header;
	MidiInJackDesc injack;
	MidiOutJackDesc outjack;
	MidiElementDesc element;
};

struct AudioControlItf {
	EndPoint ControlEP;
	uint8_t buff[8];
};

struct MidiItf {
	EndPoint InEP;
	EndPoint OutEP;
	uint8_t buff[8];
};

struct CDC_HandleTypeDef {
struct MidiStreamingHandle {
	AudioControlItf ControlItf;
	MidiItf DataItf;
	uint8_t *pTxData;
	uint8_t *pRxData;
	uint32_t TxDataLength;
	uint32_t RxDataLength;
	CDC_StateTypeDef state;
	CDC_DataStateTypeDef data_tx_state;
	CDC_DataStateTypeDef data_rx_state;
};

extern USBH_ClassTypeDef MIDI_Class;
#define USBH_MIDI_CLASS &MIDI_Class

USBH_StatusTypeDef USBH_MIDI_Transmit(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint32_t length);
USBH_StatusTypeDef USBH_MIDI_Receive(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint32_t length);
uint16_t USBH_MIDI_GetLastReceivedDataSize(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MIDI_Stop(USBH_HandleTypeDef *phost);
void USBH_MIDI_TransmitCallback(USBH_HandleTypeDef *phost);
void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost);

#endif /* __USBH_CDC_H */
