// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2019, Linaro Limited
 */

#include <common.h>
#include <dm.h>
#include <mailbox-uclass.h>
#include <linux/arm-smccc.h>

#define ARM_SMC_METHOD		0
#define ARM_HVC_METHOD		1

typedef void (invoke_fn_t)(unsigned long);

struct smc_pdata {
	unsigned long func_id;
	invoke_fn_t *invoke_fn;
};

/* Simple wrapper functions to be able to use a function pointer */
static void smccc_smc(unsigned long a0)
{
	struct arm_smccc_res res;

	arm_smccc_smc(a0, 0, 0, 0, 0, 0, 0, 0, &res);
}

static void smccc_hvc(unsigned long a0)
{
	struct arm_smccc_res res;

	arm_smccc_hvc(a0, 0, 0, 0, 0, 0, 0, 0, &res);
}

static int smc_mbox_send(struct mbox_chan *chan, const void *data)
{
	struct smc_pdata *pdata = dev_get_platdata(chan->dev);

	/*
	 * This mailbox invokes secure world for a channel event.
	 * Message is already in the channel's shared memory.
	 */
	pdata->invoke_fn(pdata->func_id);

	return 0;
}

static int smc_mbox_recv(struct mbox_chan *chan, void *data)
{
	/* Mbox owner already got the return message from shared memory */
	return 0;
}

static int smc_mbox_request(struct mbox_chan *chan)
{
	return 0;
}

static int smc_mbox_free(struct mbox_chan *chan)
{
	return 0;
}

static int smc_mbox_of_xlate(struct mbox_chan *chan,
			     struct ofnode_phandle_args *args)
{
	if (args->args_count)
		dev_warn(chan->dev, "Expect no argument to smc-mbox cells\n");

	chan->id = 0;

	return 0;
}

static int smc_mbox_ofdata_to_platdata(struct udevice *dev)
{
	ulong compat_data = dev_get_driver_data(dev);
	struct smc_pdata *pdata = dev_get_platdata(dev);
	u32 func_id;

	if (dev_read_u32(dev, "arm,func-id", &func_id)) {
		dev_err(dev, "Missing property arm,func-id\n");
		return -EINVAL;
	}

	pdata->func_id = func_id;

	if (compat_data == ARM_SMC_METHOD)
		pdata->invoke_fn = smccc_smc;
	else
		pdata->invoke_fn = smccc_hvc;

	return 0;
}

static const struct udevice_id smc_mbox_ids[] = {
	{ .compatible = "arm,smc-mbox", .data = ARM_SMC_METHOD, },
	{ .compatible = "arm,hvc-mbox", .data = ARM_HVC_METHOD, },
	{ }
};

struct mbox_ops smc_mbox_ops = {
	.of_xlate = smc_mbox_of_xlate,
	.request = smc_mbox_request,
	.free = smc_mbox_free,
	.send = smc_mbox_send,
	.recv = smc_mbox_recv,
};

U_BOOT_DRIVER(smc_mbox) = {
	.name = "arm_smc_mbox",
	.id = UCLASS_MAILBOX,
	.of_match = smc_mbox_ids,
	.ofdata_to_platdata = smc_mbox_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct smc_pdata),
	.ops = &smc_mbox_ops,
};
