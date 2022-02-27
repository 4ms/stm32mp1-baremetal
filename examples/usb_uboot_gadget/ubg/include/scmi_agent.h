/* SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause */
/*
 * Copyright (c) 2015-2019, Arm Limited and Contributors. All rights reserved.
 * Copyright (C) 2019, Linaro Limited
 */
#ifndef SCMI_AGENT_H
#define SCMI_AGENT_H

#include <asm/types.h>

/**
 * An SCMI agent represent on communication path from a device driver to
 * the remote SCMI server which driver sends messages to and receives
 * response messages from.
 */
struct scmi_agent;

enum scmi_std_protocol {
	SCMI_PROTOCOL_ID_BASE = 0x10,
	SCMI_PROTOCOL_ID_POWER_DOMAIN = 0x11,
	SCMI_PROTOCOL_ID_SYSTEM = 0x12,
	SCMI_PROTOCOL_ID_PERF = 0x13,
	SCMI_PROTOCOL_ID_CLOCK = 0x14,
	SCMI_PROTOCOL_ID_SENSOR = 0x15,
	SCMI_PROTOCOL_ID_RESET_DOMAIN = 0x16,
};

enum scmi_status_code {
	SCMI_SUCCESS =  0,
	SCMI_NOT_SUPPORTED = -1,
	SCMI_INVALID_PARAMETERS = -2,
	SCMI_DENIED = -3,
	SCMI_NOT_FOUND = -4,
	SCMI_OUT_OF_RANGE = -5,
	SCMI_BUSY = -6,
	SCMI_COMMS_ERROR = -7,
	SCMI_GENERIC_ERROR = -8,
	SCMI_HARDWARE_ERROR = -9,
	SCMI_PROTOCOL_ERROR = -10,
};

/*
 * struct scmi_msg - Context of a SCMI message sent and the response received
 *
 * @protocol_id: SCMI protocol ID
 * @message_id: SCMI message ID for a defined protocol ID
 * @in_msg: pointer to the message payload sent by the driver
 * @in_msg_sz: byte size of the message payload sent
 * @out_msg: pointer to buffer to store response message payload
 * @out_msg_size: Byte size of the response buffer or payload
 */
struct scmi_msg {
	unsigned int protocol_id;
	unsigned int message_id;
	u8 *in_msg;
	size_t in_msg_sz;
	u8 *out_msg;
	size_t out_msg_sz;
};

/**
 * scmi_send_and_process_msg() - send and process a SCMI message
 *
 * Send a message to a SCMI server through a target SCMI agent device.
 * Caller sets scmi_msg::out_msg_sz to the output message buffer size.
 * On return, scmi_msg::out_msg_sz stores the response payload size.
 *
 * @dev: SCMI agent device
 * @msg: Message structure reference
 * @return 0 on success, a negative errno otherwise
 */
int scmi_agent_process_msg(struct udevice *dev, struct scmi_msg *msg);

/**
 * scmi_to_linux_errno() - Convert an SCMI error code into a Linux errno code
 *
 * @scmi_errno: SCMI error code value
 * @return 0 for successful status and a negative errno otherwise
 */
int scmi_to_linux_errno(int32_t scmi_errno);

#endif /* SCMI_AGENT_H */
