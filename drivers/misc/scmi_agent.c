// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2015-2019, Arm Limited and Contributors. All rights reserved.
 * Copyright (C) 2019 Linaro Limited.
 */

#include <common.h>
#include <cpu_func.h>
#include <asm/system.h>
#include <asm/types.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <dm/ofnode.h>
#include <errno.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <mailbox.h>
#include <memalign.h>
#include <scmi_agent.h>
#include <tee.h>

struct error_code {
	int scmi;
	int errno;
};

static const struct error_code scmi_linux_errmap[] = {
	{ .scmi = SCMI_NOT_SUPPORTED, .errno = -EOPNOTSUPP, },
	{ .scmi = SCMI_INVALID_PARAMETERS, .errno = -EINVAL, },
	{ .scmi = SCMI_DENIED, .errno = -EACCES, },
	{ .scmi = SCMI_NOT_FOUND, .errno = -ENOENT, },
	{ .scmi = SCMI_OUT_OF_RANGE, .errno = -ERANGE, },
	{ .scmi = SCMI_BUSY, .errno = -EBUSY, },
	{ .scmi = SCMI_COMMS_ERROR, .errno = -ECOMM, },
	{ .scmi = SCMI_GENERIC_ERROR, .errno = -EIO, },
	{ .scmi = SCMI_HARDWARE_ERROR, .errno = -EREMOTEIO, },
	{ .scmi = SCMI_PROTOCOL_ERROR, .errno = -EPROTO, },
};

int scmi_to_linux_errno(int32_t scmi_code)
{
	int n;

	if (scmi_code == 0)
		return 0;

	for (n = 0; n < ARRAY_SIZE(scmi_linux_errmap); n++)
		if (scmi_code == scmi_linux_errmap[n].scmi)
			return scmi_linux_errmap[1].errno;

	return -EPROTO;
}

struct method_ops {
	int (*process_msg)(struct udevice *dev, struct scmi_msg *msg);
	int (*remove_agent)(struct udevice *dev);
};

struct scmi_agent {
	struct method_ops	*method_ops;
	void			*method_priv;
};

/*
 * Shared Memory based Transport (SMT) message buffer management
 *
 * SMT uses 28 byte header prior message payload to handle the state of
 * the communication channel realized by the shared memory area and
 * to define SCMI protocol information the payload relates to.
 */
struct scmi_smt_header {
	__le32 reserved;
	__le32 channel_status;
#define SCMI_SHMEM_CHAN_STAT_CHANNEL_ERROR	BIT(1)
#define SCMI_SHMEM_CHAN_STAT_CHANNEL_FREE	BIT(0)
	__le32 reserved1[2];
	__le32 flags;
#define SCMI_SHMEM_FLAG_INTR_ENABLED		BIT(0)
	__le32 length;
	__le32 msg_header;
	u8 msg_payload[0];
};

#define SMT_HEADER_TOKEN(token)		(((token) << 18) & GENMASK(31, 18))
#define SMT_HEADER_PROTOCOL_ID(proto)	(((proto) << 10) & GENMASK(17, 10))
#define SMT_HEADER_MESSAGE_TYPE(type)	(((type) << 18) & GENMASK(9, 8))
#define SMT_HEADER_MESSAGE_ID(id)	((id) & GENMASK(7, 0))

struct scmi_shm_buf {
	u8	*buf;
	size_t	size;
};

static int get_shm_buffer(struct udevice *dev, struct scmi_shm_buf *shm)
{
	int rc;
	struct ofnode_phandle_args args;
	struct resource resource;
	fdt32_t faddr;
	phys_addr_t paddr;

	rc = dev_read_phandle_with_args(dev, "shmem", NULL, 0, 0, &args);
	if (rc)
		return rc;

	rc = ofnode_read_resource(args.node, 0, &resource);
	if (rc)
		return rc;

	faddr = cpu_to_fdt32(resource.start);
	paddr = ofnode_translate_address(args.node, &faddr);

	shm->size = resource_size(&resource);
	if (shm->size < sizeof(struct scmi_smt_header)) {
		dev_err(dev, "Shared memory buffer too small\n");
		return -EINVAL;
	}

	shm->buf = devm_ioremap(dev, paddr, shm->size);
	if (!shm->buf)
		return -ENOMEM;

	if (dcache_status())
		mmu_set_region_dcache_behaviour((uintptr_t)shm->buf,
						shm->size, DCACHE_OFF);

	return 0;
}

/*
 * Mailbox support
 */
struct scmi_mbox_channel {
	struct scmi_shm_buf shm_buf;
	struct mbox_chan mbox;
	ulong timeout_us;
};

static int mbox_process_msg(struct udevice *dev, struct scmi_msg *msg)
{
	struct scmi_agent *agent = dev_get_priv(dev);
	struct scmi_mbox_channel *chan = agent->method_priv;
	struct scmi_smt_header *hdr = (void *)chan->shm_buf.buf;
	int rc;

	/* Basic check-up */
	if (!(hdr->channel_status & SCMI_SHMEM_CHAN_STAT_CHANNEL_FREE)) {
		dev_dbg(dev, "Channel busy\n");
		return -EBUSY;
	}

	if ((!msg->in_msg && msg->in_msg_sz) ||
	    (!msg->out_msg && msg->out_msg_sz))
		return -EINVAL;

	if (chan->shm_buf.size < (sizeof(*hdr) + msg->in_msg_sz) ||
	    chan->shm_buf.size < (sizeof(*hdr) + msg->out_msg_sz)) {
		dev_dbg(dev, "Buffer too small\n");
		return -ETOOSMALL;
	}

	/* Load message in shared memory */
	hdr->channel_status &= ~SCMI_SHMEM_CHAN_STAT_CHANNEL_FREE;
	hdr->length = msg->in_msg_sz + sizeof(hdr->msg_header);
	hdr->msg_header = SMT_HEADER_TOKEN(0) |
			  SMT_HEADER_MESSAGE_TYPE(0) |
			  SMT_HEADER_PROTOCOL_ID(msg->protocol_id) |
			  SMT_HEADER_MESSAGE_ID(msg->message_id);

	memcpy(hdr->msg_payload, msg->in_msg, msg->in_msg_sz);

	/* Give shm addr to mbox in case it is meaningful */
	rc = mbox_send(&chan->mbox, hdr);
	if (rc) {
		dev_err(dev, "Message send failed: %d\n", rc);
		goto out;
	}

	/* Receive the response */
	rc = mbox_recv(&chan->mbox, hdr, chan->timeout_us);
	if (rc) {
		dev_err(dev, "Response failed: %d, abort\n", rc);
		goto out;
	}

	/* Check the statuses */
	if (!(hdr->channel_status & SCMI_SHMEM_CHAN_STAT_CHANNEL_FREE)) {
		dev_err(dev, "Channel unexpectedly busy, reset channel\n");
		rc = -ECOMM;
		goto out;
	}

	if (hdr->channel_status & SCMI_SHMEM_CHAN_STAT_CHANNEL_ERROR) {
		dev_err(dev, "Channel error reported, reset channel\n");
		rc = -ECOMM;
		goto out;
	}

	if (hdr->length > msg->out_msg_sz + sizeof(hdr->msg_header)) {
		dev_err(dev, "Buffer to small\n");
		rc = -ETOOSMALL;
		goto out;
	}

	/* Get the data */
	msg->out_msg_sz = hdr->length - sizeof(hdr->msg_header);
	memcpy(msg->out_msg, hdr->msg_payload, msg->out_msg_sz);

out:
	/* Free channel for further communication */
	hdr->channel_status |= SCMI_SHMEM_CHAN_STAT_CHANNEL_FREE;
	hdr->channel_status &= ~SCMI_SHMEM_CHAN_STAT_CHANNEL_ERROR;

	return rc;
}

struct method_ops mbox_channel_ops = {
	.process_msg = mbox_process_msg,
};

static int probe_smc_mailbox_channel(struct udevice *dev)
{
	struct scmi_agent *agent = dev_get_priv(dev);
	struct scmi_mbox_channel *chan;
	int rc;

	chan = devm_kzalloc(dev, sizeof(*chan), GFP_KERNEL);
	if (!chan)
		return -ENOMEM;

	chan->timeout_us = 10000;

	rc = mbox_get_by_index(dev, 0, &chan->mbox);
	if (rc) {
		dev_err(dev, "Failed to find mailbox: %d\n", rc);
		goto out;
	}

	rc = get_shm_buffer(dev, &chan->shm_buf);
	if (rc)
		dev_err(dev, "Failed to get shm resources: %d\n", rc);

out:
	if (rc) {
		devm_kfree(dev, chan);
	} else {
		agent->method_ops = &mbox_channel_ops;
		agent->method_priv = (void *)chan;
	}

	return rc;
}

/*
 * Exported functions by the SCMI agent
 */

int scmi_agent_process_msg(struct udevice *dev, struct scmi_msg *msg)
{
	struct scmi_agent *agent = dev_get_priv(dev);

	return agent->method_ops->process_msg(dev, msg);
}

static int scmi_agent_remove(struct udevice *dev)
{
	struct scmi_agent *agent = dev_get_priv(dev);

	if (agent->method_ops->remove_agent)
		return agent->method_ops->remove_agent(dev);

	return 0;
}

static int scmi_agent_probe(struct udevice *dev)
{
	/* Only mailbox method supported for now */
	return probe_smc_mailbox_channel(dev);
}

static int scmi_agent_bind(struct udevice *dev)
{
	int rc = 0;
	ofnode node;
	struct driver *drv;

	dev_for_each_subnode(node, dev) {
		u32 protocol_id;

		if (!ofnode_is_available(node))
			continue;

		if (ofnode_read_u32(node, "reg", &protocol_id))
			continue;

		switch (protocol_id) {
		case SCMI_PROTOCOL_ID_CLOCK:
			drv = DM_GET_DRIVER(scmi_clock);
			break;
		case SCMI_PROTOCOL_ID_RESET_DOMAIN:
			drv = DM_GET_DRIVER(scmi_reset_domain);
			break;
		default:
			dev_info(dev, "Ignore unsupported SCMI protocol %u\n",
				 protocol_id);
			continue;
		}

		rc = device_bind_ofnode(dev, drv, ofnode_get_name(node),
					NULL, node, NULL);
		if (rc)
			break;
	}

	if (rc)
		device_unbind(dev);

	return rc;
}

static const struct udevice_id scmi_agent_ids[] = {
	{ .compatible = "arm,scmi", },
	{ }
};

U_BOOT_DRIVER(scmi_agent) = {
	.name		= "scmi-agent",
	.id		= UCLASS_NOP,
	.of_match	= scmi_agent_ids,
	.priv_auto_alloc_size = sizeof(struct scmi_agent),
	.bind		= scmi_agent_bind,
	.probe		= scmi_agent_probe,
	.remove		= scmi_agent_remove,
	.flags		= DM_FLAG_OS_PREPARE,
};
