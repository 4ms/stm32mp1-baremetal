// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2011-2015 Panasonic Corporation
 * Copyright (C) 2015-2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 */

#include <linux/io.h>

#include "../init.h"
#include "../sc-regs.h"

void uniphier_ld4_clk_init(void)
{
	u32 tmp;

	/* deassert reset */
	tmp = readl(sc_base + SC_RSTCTRL);
#ifdef CONFIG_NAND_DENALI
	tmp |= SC_RSTCTRL_NRST_NAND;
#endif
	writel(tmp, sc_base + SC_RSTCTRL);
	readl(sc_base + SC_RSTCTRL); /* dummy read */

	/* provide clocks */
	tmp = readl(sc_base + SC_CLKCTRL);
#ifdef CONFIG_NAND_DENALI
	tmp |= SC_CLKCTRL_CEN_NAND;
#endif
	writel(tmp, sc_base + SC_CLKCTRL);
	readl(sc_base + SC_CLKCTRL); /* dummy read */
}
