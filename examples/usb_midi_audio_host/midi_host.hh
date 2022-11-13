#pragma once
#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "usbh_midi.hh"

class MidiHost {
public:
	MidiStreamingHandle MSHandle;
	USBH_HandleTypeDef usbhost;

	USBH_ClassTypeDef midi_class_ops = {
		"MIDI",
		AudioClassCode,
		USBH_MIDI_InterfaceInit,
		USBH_MIDI_InterfaceDeInit,
		USBH_MIDI_ClassRequest,
		USBH_MIDI_Process,
		USBH_MIDI_SOFProcess,
		&MSHandle,
	};

	MidiHost() = default;

	void set_rx_callback(MidiStreamRxCallbackType rx_callback) { MSHandle.rx_callback = rx_callback; }
	void set_tx_callback(MidiStreamTxCallbackType tx_callback) { MSHandle.tx_callback = tx_callback; }

	bool init()
	{
		auto status = USBH_Init(&usbhost, usbh_state_change_callback, 0);
		if (status != USBH_OK)
			return false;

		// defined in usbh_conf.c
		extern HCD_HandleTypeDef hhcd;
		InterruptControl::disable_irq(OTG_IRQn);
		InterruptManager::registerISR(OTG_IRQn, [] { HAL_HCD_IRQHandler(&hhcd); });
		InterruptControl::set_irq_priority(OTG_IRQn, 0, 0);
		InterruptControl::enable_irq(OTG_IRQn);

		USBH_RegisterClass(&usbhost, &midi_class_ops);

		return true;
	}

	bool start() { return USBH_Start(&usbhost) == USBH_OK; }
	bool stop() { return USBH_Stop(&usbhost) == USBH_OK; }
	void process() { USBH_Process(&usbhost); }
	USBH_StatusTypeDef receive() { return USBH_MIDI_Receive(&usbhost, MSHandle.rx_buffer, 128); }
	USBH_StatusTypeDef transmit(uint8_t *buff, uint32_t len) { return USBH_MIDI_Transmit(&usbhost, buff, len); }

	static void usbh_state_change_callback(USBH_HandleTypeDef *phost, uint8_t id)
	{
		USBHostHandle host{phost};
		auto mshandle = host.get_class_handle<MidiStreamingHandle>();
		if (!mshandle)
			return;

		switch (id) {
			case HOST_USER_SELECT_CONFIGURATION:
				printf("Select config\n");
				break;

			case HOST_USER_CONNECTION:
				printf("Connected\n");
				break;

			case HOST_USER_CLASS_SELECTED:
				printf("Class selected\n");
				break;

			case HOST_USER_CLASS_ACTIVE:
				printf("Class active\n");
				USBH_MIDI_Receive(phost, mshandle->rx_buffer, MidiStreamingBufferSize);
				break;

			case HOST_USER_DISCONNECTION:
				printf("Disconnected\n");
				break;

			case HOST_USER_UNRECOVERED_ERROR:
				printf("Error\n");
				break;
		}
	}
};
