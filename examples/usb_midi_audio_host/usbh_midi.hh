/**
 ******************************************************************************
 * @file    usbh_midi.cc
 * @author  Dan Green. Based on CDC Class in STM32 USB Host Library v3.5.0
 * @brief   This file is the for USB Host MIDI subclass of Audio Class.
 *
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
 ******************************************************************************
 */

#pragma once

#include "usb_host_helper.hh"
#include "usbh_core.h"
#include <functional>


constexpr uint8_t AudioClassCode = 0x01;
constexpr uint8_t AudioControlSubclassCode = 0x01;
constexpr uint8_t MidiStreamingSubClass = 0x03;

constexpr uint8_t AnyProtocol = 0xFF;
constexpr uint8_t NoValidInterfaceFound = 0xFF;

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

struct MidiStreamingItf {
	EndPoint InEP;
	EndPoint OutEP;
	uint8_t buff[8];
};

enum class MidiStreamingDataState {
	Idle,
	SendData,
	SendDataWait,
	ReceiveData,
	ReceiveDataWait,
};

enum class MidiStreamingState {
	Idle,
	TransferData,
	Error,
};

using MidiStreamRxCallbackType = std::function<void(uint8_t *, uint32_t)>;
using MidiStreamTxCallbackType = std::function<void()>;

struct MidiStreamingHandle {
	AudioControlItf ControlItf;
	MidiStreamingItf DataItf;
	uint8_t *pTxData;
	uint8_t *pRxData;
	uint32_t TxDataLength;
	uint32_t RxDataLength;
	MidiStreamingState state;
	MidiStreamingDataState data_tx_state;
	MidiStreamingDataState data_rx_state;

	static void _default_rx_cb(uint8_t *, uint32_t) {}
	static void _default_tx_cb() {}
	MidiStreamRxCallbackType rx_callback = _default_rx_cb;
	MidiStreamTxCallbackType tx_callback = _default_tx_cb;

	uint8_t rx_buffer[MidiStreamingBufferSize];
};

USBH_StatusTypeDef USBH_MIDI_InterfaceInit(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MIDI_InterfaceDeInit(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MIDI_Process(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MIDI_SOFProcess(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MIDI_ClassRequest(USBH_HandleTypeDef *phost);

USBH_StatusTypeDef USBH_MIDI_Transmit(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint32_t length);
USBH_StatusTypeDef USBH_MIDI_Receive(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint32_t length);
uint16_t USBH_MIDI_GetLastReceivedDataSize(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MIDI_Stop(USBH_HandleTypeDef *phost);

// extern USBH_ClassTypeDef MIDI_Class_Ops;
// #define USBH_MIDI_CLASS &MIDI_Class_Ops
