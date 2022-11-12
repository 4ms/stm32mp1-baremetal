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

// template<typename ClassHandleT>
class USBHostHandle {
public:
	USBH_HandleTypeDef *phost;
	// ClassHandleT *class_h = nullptr;

	USBHostHandle(USBH_HandleTypeDef *phost)
		: phost{phost}
	{
		// if (phost)
		// 	if (phost->pActiveClass)
		// 		class_h = phost->pActiveClass->pData;
	}

	bool is_in_ep(uint8_t interface, uint8_t ep_index)
	{
		auto addr = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[ep_index].bEndpointAddress;
		return (addr & 0x80U);
	}
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
};

// Create a new, zeroed handle
// Uses the user-defined USBH_malloc and USBH_memset
// so that we can inter-operate with other classes from the STM32 Host Library
template<typename HandleType>
HandleType *new_usbhost_class_handle()
{
	auto handle = static_cast<HandleType *>(USBH_malloc(sizeof(HandleType)));
	if (handle)
		USBH_memset(handle, 0, sizeof(HandleType));

	return handle;
}
