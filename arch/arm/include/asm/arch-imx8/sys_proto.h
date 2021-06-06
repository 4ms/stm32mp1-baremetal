/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2018 NXP
 */

#include <asm/arch/sci/sci.h>
#include <asm/mach-imx/sys_proto.h>
#include <linux/types.h>

struct pass_over_info_t {
	u16 barker;
	u16 len;
	u32 g_bt_cfg_shadow;
	u32 card_address_mode;
	u32 bad_block_count_met;
	u32 g_ap_mu;
};

extern unsigned long boot_pointer[];
void build_info(void);
enum boot_device get_boot_device(void);
int print_bootinfo(void);
int sc_pm_setup_uart(sc_rsrc_t uart_rsrc, sc_pm_clock_rate_t clk_rate);
