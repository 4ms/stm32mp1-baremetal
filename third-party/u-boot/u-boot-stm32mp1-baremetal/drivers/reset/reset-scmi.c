// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2019 Linaro Limited
 */
#include <asm/types.h>
#include <common.h>
#include <dm.h>
#include <errno.h>
#include <reset-uclass.h>
#include <scmi_agent.h>

enum scmi_reset_domain_message_id {
	SCMI_RESET_DOMAIN_RESET = 0x4,
};

#define SCMI_RD_RESET_FLAG_ASSERT	BIT(1)
#define SCMI_RD_RESET_FLAG_DEASSERT	0

struct scmi_rd_reset_in {
	u32 domain_id;
	u32 flags;
	u32 reset_state;
};

struct scmi_rd_reset_out {
	s32 status;
};

static int scmi_reset_set_state(struct reset_ctl *rst, int assert_not_deassert)
{
	struct scmi_rd_reset_in in = {
		.domain_id = rst->id,
		.flags = assert_not_deassert ? SCMI_RD_RESET_FLAG_ASSERT :
			 SCMI_RD_RESET_FLAG_DEASSERT,
		.reset_state = 0,
	};
	struct scmi_rd_reset_out out;
	struct scmi_msg scmi_msg = {
		.protocol_id = SCMI_PROTOCOL_ID_RESET_DOMAIN,
		.message_id = SCMI_RESET_DOMAIN_RESET,
		.in_msg = (u8 *)&in,
		.in_msg_sz = sizeof(in),
		.out_msg = (u8 *)&out,
		.out_msg_sz = sizeof(out),
	};
	int rc;

	rc = scmi_agent_process_msg(rst->dev->parent, &scmi_msg);
	if (rc)
		return rc;

	return scmi_to_linux_errno(out.status);
}

static int scmi_reset_assert(struct reset_ctl *rst)
{
	return scmi_reset_set_state(rst, SCMI_RD_RESET_FLAG_ASSERT);
}

static int scmi_reset_deassert(struct reset_ctl *rst)
{
	return scmi_reset_set_state(rst, SCMI_RD_RESET_FLAG_DEASSERT);
}

static int scmi_reset_request(struct reset_ctl *reset_ctl)
{
	return 0;
}

static int scmi_reset_free(struct reset_ctl *reset_ctl)
{
	return 0;
}

static const struct reset_ops scmi_reset_domain_ops = {
	.request	= scmi_reset_request,
	.free		= scmi_reset_free,
	.rst_assert	= scmi_reset_assert,
	.rst_deassert	= scmi_reset_deassert,
};

U_BOOT_DRIVER(scmi_reset_domain) = {
	.name = "scmi_reset_domain",
	.id = UCLASS_RESET,
	.ops = &scmi_reset_domain_ops,
};
