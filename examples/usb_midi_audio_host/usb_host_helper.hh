//
// usbh_host.hh
// Helper functions for USB Hosts
//
// By Dan Green
// Copyright (c) 2022 Dan Green
// Licensed under MIT License, see LICSENSE file
//

#pragma once
#include "usbh_core.h"

struct EndPoint {
	uint8_t addr;
	uint8_t size;
	uint8_t pipe;
};

enum class EndPointType : uint8_t {
	Control = USB_EP_TYPE_CTRL,
	Isoc = USB_EP_TYPE_ISOC,
	Bulk = USB_EP_TYPE_BULK,
	Intr = USB_EP_TYPE_INTR,
};

// Stateless wrapper for USBH_HandleTypeDef*
// That adds helper funcs for common operations in C functions that have a phost parameter
class USBHostHelper {
public:
	USBH_HandleTypeDef *phost;

	USBHostHelper(USBH_HandleTypeDef *phost)
		: phost{phost}
	{}

	// Returns the user data of the active class, cast to whatever type you're expecting.
	// Checks all the pointers before dereferencing them.
	// Returns nullptr if it fails
	template<typename HandleType>
	HandleType *get_class_handle()
	{
		if (phost) {
			if (phost->pActiveClass) {
				if (phost->pActiveClass->pData) {
					return static_cast<HandleType *>(phost->pActiveClass->pData);
				}
			}
		}
		return nullptr;
	}

	// Returns true if the endpoint specified by the interface index and endpoint index is an IN EP
	bool is_in_ep(uint8_t interface, uint8_t ep_index)
	{
		auto addr = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[ep_index].bEndpointAddress;
		return (addr & 0x80U);
	}

	// Returns true if the endpoint specified by the interface index and endpoint index is an OUT EP
	bool is_out_ep(uint8_t interface, uint8_t ep_index) { return !is_in_ep(interface, ep_index); }

	void link_endpoint_pipe(EndPoint &ep, uint8_t interface, uint8_t ep_index)
	{
		auto ep_desc = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[ep_index];
		ep.addr = ep_desc.bEndpointAddress;
		ep.size = ep_desc.wMaxPacketSize;
		ep.pipe = USBH_AllocPipe(phost, ep.addr);
	}

	USBH_StatusTypeDef open_pipe(const EndPoint &ep, EndPointType ep_type)
	{
		return USBH_OpenPipe(phost,
							 ep.pipe,
							 ep.addr,
							 phost->device.address,
							 phost->device.speed,
							 static_cast<uint8_t>(ep_type),
							 ep.size);
	}

	void set_toggle(const EndPoint &ep, uint8_t toggle_val) { USBH_LL_SetToggle(phost, ep.pipe, toggle_val); }

	void close_and_free_pipe(EndPoint &ep)
	{
		if (ep.pipe) {
			USBH_ClosePipe(phost, ep.pipe);
			USBH_FreePipe(phost, ep.pipe);
			ep.pipe = 0U;
		}
	}
};
