// SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause
/*
 * Copyright (C) 2018, STMicroelectronics - All Rights Reserved
 */

#include "memsize.h"
#include "stm32mp15-osd32mp1-ddr3-1x4Gb.dtsi"
#include "stm32mp1_ddr.h"
#include "stm32mp1xx.h"
#include <asm/io.h>

static void stm32mp1_ddr_get_config(struct stm32mp1_ddr_config *cfg);

unsigned long stm32mp1_get_hse_freq(void)
{
	return 24000000uL;
}

unsigned long stm32mp1_get_hsi_freq(void)
{
	const uint_fast32_t hsi = 64000000UL;
	const uint_fast32_t hsidiv = (RCC->HSICFGR & RCC_HSICFGR_HSIDIV_Msk) >> RCC_HSICFGR_HSIDIV_Pos;
	return hsi >> hsidiv;
}

unsigned long stm32mp1_get_pll1_2_ref_freq(void)
{
	// PLL1, PLL2 source mux
	// 0x0: HSI selected as PLL clock (hsi_ck) (default after reset)
	// 0x1: HSE selected as PLL clock (hse_ck)
	switch ((RCC->RCK12SELR & RCC_RCK12SELR_PLL12SRC_Msk) >> RCC_RCK12SELR_PLL12SRC_Pos) {
		default:
		case 0x00:
			return stm32mp1_get_hsi_freq();
		case 0x01:
			return stm32mp1_get_hse_freq();
	}
}

unsigned long stm32mp1_get_pll2_freq(void)
{
	const uint_fast32_t pll2divn = ((RCC->PLL2CFGR1 & RCC_PLL2CFGR1_DIVN_Msk) >> RCC_PLL2CFGR1_DIVN_Pos) + 1;
	const uint_fast32_t pll2divm = ((RCC->PLL2CFGR1 & RCC_PLL2CFGR1_DIVM2_Msk) >> RCC_PLL2CFGR1_DIVM2_Pos) + 1;
	return (uint_fast64_t)stm32mp1_get_pll1_2_ref_freq() * pll2divn / pll2divm;
}

unsigned long stm32mp1_get_pll2_p_freq(void)
{
	const uint_fast32_t pll2divp = ((RCC->PLL2CFGR2 & RCC_PLL2CFGR2_DIVP_Msk) >> RCC_PLL2CFGR2_DIVP_Pos) + 1;
	return stm32mp1_get_pll2_freq() / pll2divp;
}

unsigned long stm32mp1_get_pll2_r_freq(void)
{
	const uint_fast32_t pll2divr = ((RCC->PLL2CFGR2 & RCC_PLL2CFGR2_DIVR_Msk) >> RCC_PLL2CFGR2_DIVR_Pos) + 1;
	return stm32mp1_get_pll2_freq() / pll2divr;
}

int stm32mp1_ddr_clk_enable(struct ddr_info *priv, u32 mem_speed)
{
	RCC->DDRITFCR = RCC->DDRITFCR | RCC_DDRITFCR_DDRC1EN | RCC_DDRITFCR_DDRC1LPEN | RCC_DDRITFCR_DDRC2EN |
					RCC_DDRITFCR_DDRC2LPEN | RCC_DDRITFCR_DDRPHYCEN | RCC_DDRITFCR_DDRPHYCLPEN |
					RCC_DDRITFCR_DDRPHYCAPBEN | RCC_DDRITFCR_DDRCAPBLPEN | RCC_DDRITFCR_DDRCAPBEN |
					RCC_DDRITFCR_DDRPHYCAPBLPEN;

	unsigned long ddrphy_clk;
	unsigned long ddr_clk;
	int ret;
	unsigned int idx;

	ddrphy_clk = stm32mp1_get_pll2_r_freq();

	log("DDR: mem_speed (%d kHz), RCC %d kHz\n", mem_speed, (u32)(ddrphy_clk / 1000));

	/* max 10% frequency delta */
	ddr_clk = abs(ddrphy_clk - mem_speed * 1000);
	if (ddr_clk > (mem_speed * 100)) {
		pr_err("DDR expected freq %d kHz, current is %d kHz\n", mem_speed, (u32)(ddrphy_clk / 1000));
		return -EINVAL;
	}

	return 0;
}

static void stm32mp1_ddr_tz_init(void)
{
	// TrustZone address space controller for DDR (TZC)
	// TZC AXI port 1 clocks enable
	RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_TZC1EN;

	// TZC AXI port 2 clocks enable
	RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_TZC2EN;

	TZC->ACTION = 0x00;
	const uint8_t lastfilter = (TZC->BUILD_CONFIG >> 24) & 0x03;
	const uint32_t mask = (1uL << (lastfilter + 1)) - 1;
	TZC->GATE_KEEPER = mask; // Gate open request

	// Check open status
	while (((TZC->GATE_KEEPER >> 16) & mask) != mask)
		;
	TZC->REG_ATTRIBUTESO |= 0xC0000000; // All (read and write) permitted

	TZC->REG_ID_ACCESSO = 0xFFFFFFFF; // NSAID_WR_EN[15:0], NSAID_RD_EN[15:0] - permits read and write non-secure to the
									  // region for all NSAIDs

	const uint8_t lastregion = TZC->BUILD_CONFIG & 0x1f;
	for (uint8_t i = 1; i <= lastregion; ++i) {
		volatile uint32_t *const REG_ATTRIBUTESx = &TZC->REG_ATTRIBUTESO + (i * 8);
		volatile uint32_t *const REG_ID_ACCESSx = &TZC->REG_ID_ACCESSO + (i * 8);
		volatile uint32_t *const REG_BASE_LOWx = &TZC->REG_BASE_LOWO + (i * 8);
		volatile uint32_t *const REG_BASE_HIGHx = &TZC->REG_BASE_HIGHO + (i * 8);
		volatile uint32_t *const REG_TOP_LOWx = &TZC->REG_TOP_LOWO + (i * 8);
		volatile uint32_t *const REG_TOP_HIGHx = &TZC->REG_TOP_HIGHO + (i * 8);

		*REG_ATTRIBUTESx &= ~0x03uL;
	}
}

int stm32mp1_ddr_setup(void)
{
	struct ddr_info _priv;
	struct ddr_info *priv = &_priv;
	int ret;
	unsigned int idx;
	struct stm32mp1_ddr_config config;

	stm32mp1_ddr_tz_init();

	priv->ctl = (struct stm32mp1_ddrctl *)DDRCTRL_BASE;
	priv->phy = (struct stm32mp1_ddrphy *)DDRPHYC_BASE;
	priv->rcc = RCC_BASE;
	priv->info.base = DRAM_MEM_BASE;
	priv->info.size = 0;

	stm32mp1_ddr_get_config(&config);
	config.p_cal_present = false;

	// Set CKMOD bits = 0b000 during init: "Normal mode: This mode must be selected during DDRC and DDRPHYC
	// initialization phase"
	RCC->DDRITFCR = (RCC->DDRITFCR & ~RCC_DDRITFCR_DDRCKMOD_Msk) | (0 << RCC_DDRITFCR_DDRCKMOD_Pos);

	// Disable AXIDCG clock gating during init
	RCC->DDRITFCR = RCC->DDRITFCR & ~RCC_DDRITFCR_AXIDCGEN;

	stm32mp1_ddr_init(priv, &config);

	// Enable clock gating
	RCC->DDRITFCR = RCC->DDRITFCR | RCC_DDRITFCR_AXIDCGEN;

	/* check size */
	debug("%s : get_ram_size(%x, %x)\n", __func__, (u32)priv->info.base, (u32)DDR_MEM_SIZE);
	priv->info.size = get_ram_size((long *)priv->info.base, DDR_MEM_SIZE);
	debug("%s : %x\n", __func__, (u32)priv->info.size);

	/* check memory access for all memory */
	if (config.info.size != priv->info.size) {
		pr_err("DDR invalid size : 0x%x, expected 0x%x\n", priv->info.size, config.info.size);
		return -EINVAL;
	}
	return 0;
}

uint32_t stm32mp1_ddr_get_size()
{
	return DDR_MEM_SIZE;
}

static void stm32mp1_ddr_get_config(struct stm32mp1_ddr_config *cfg)
{
	cfg->info.speed = DDR_MEM_SPEED; // kHz
	cfg->info.size = DDR_MEM_SIZE;
	cfg->info.name = DDR_MEM_NAME;

	cfg->c_reg.mstr = DDR_MSTR;
	cfg->c_reg.mrctrl0 = DDR_MRCTRL0;
	cfg->c_reg.mrctrl1 = DDR_MRCTRL1;
	cfg->c_reg.derateen = DDR_DERATEEN;
	cfg->c_reg.derateint = DDR_DERATEINT;
	cfg->c_reg.pwrctl = DDR_PWRCTL;
	cfg->c_reg.pwrtmg = DDR_PWRTMG;
	cfg->c_reg.hwlpctl = DDR_HWLPCTL;
	cfg->c_reg.rfshctl0 = DDR_RFSHCTL0;
	cfg->c_reg.rfshctl3 = DDR_RFSHCTL3;
	cfg->c_reg.crcparctl0 = DDR_CRCPARCTL0;
	cfg->c_reg.zqctl0 = DDR_ZQCTL0;
	cfg->c_reg.dfitmg0 = DDR_DFITMG0;
	cfg->c_reg.dfitmg1 = DDR_DFITMG1;
	cfg->c_reg.dfilpcfg0 = DDR_DFILPCFG0;
	cfg->c_reg.dfiupd0 = DDR_DFIUPD0;
	cfg->c_reg.dfiupd1 = DDR_DFIUPD1;
	cfg->c_reg.dfiupd2 = DDR_DFIUPD2;
	cfg->c_reg.dfiphymstr = DDR_DFIPHYMSTR;
	cfg->c_reg.odtmap = DDR_ODTMAP;
	cfg->c_reg.dbg0 = DDR_DBG0;
	cfg->c_reg.dbg1 = DDR_DBG1;
	cfg->c_reg.dbgcmd = DDR_DBGCMD;
	cfg->c_reg.poisoncfg = DDR_POISONCFG;
	cfg->c_reg.pccfg = DDR_PCCFG;

	cfg->c_timing.rfshtmg = DDR_RFSHTMG;
	cfg->c_timing.dramtmg0 = DDR_DRAMTMG0;
	cfg->c_timing.dramtmg1 = DDR_DRAMTMG1;
	cfg->c_timing.dramtmg2 = DDR_DRAMTMG2;
	cfg->c_timing.dramtmg3 = DDR_DRAMTMG3;
	cfg->c_timing.dramtmg4 = DDR_DRAMTMG4;
	cfg->c_timing.dramtmg5 = DDR_DRAMTMG5;
	cfg->c_timing.dramtmg6 = DDR_DRAMTMG6;
	cfg->c_timing.dramtmg7 = DDR_DRAMTMG7;
	cfg->c_timing.dramtmg8 = DDR_DRAMTMG8;
	cfg->c_timing.dramtmg14 = DDR_DRAMTMG14;
	cfg->c_timing.odtcfg = DDR_ODTCFG;

	cfg->c_perf.sched = DDR_SCHED;
	cfg->c_perf.sched1 = DDR_SCHED1;
	cfg->c_perf.perfhpr1 = DDR_PERFHPR1;
	cfg->c_perf.perflpr1 = DDR_PERFLPR1;
	cfg->c_perf.perfwr1 = DDR_PERFWR1;
	cfg->c_perf.pcfgr_0 = DDR_PCFGR_0;
	cfg->c_perf.pcfgw_0 = DDR_PCFGW_0;
	cfg->c_perf.pcfgqos0_0 = DDR_PCFGQOS0_0;
	cfg->c_perf.pcfgqos1_0 = DDR_PCFGQOS1_0;
	cfg->c_perf.pcfgwqos0_0 = DDR_PCFGWQOS0_0;
	cfg->c_perf.pcfgwqos1_0 = DDR_PCFGWQOS1_0;
	cfg->c_perf.pcfgr_1 = DDR_PCFGR_1;
	cfg->c_perf.pcfgw_1 = DDR_PCFGW_1;
	cfg->c_perf.pcfgqos0_1 = DDR_PCFGQOS0_1;
	cfg->c_perf.pcfgqos1_1 = DDR_PCFGQOS1_1;
	cfg->c_perf.pcfgwqos0_1 = DDR_PCFGWQOS0_1;
	cfg->c_perf.pcfgwqos1_1 = DDR_PCFGWQOS1_1;

	cfg->c_map.addrmap1 = DDR_ADDRMAP1;
	cfg->c_map.addrmap2 = DDR_ADDRMAP2;
	cfg->c_map.addrmap3 = DDR_ADDRMAP3;
	cfg->c_map.addrmap4 = DDR_ADDRMAP4;
	cfg->c_map.addrmap5 = DDR_ADDRMAP5;
	cfg->c_map.addrmap6 = DDR_ADDRMAP6;
	cfg->c_map.addrmap9 = DDR_ADDRMAP9;
	cfg->c_map.addrmap10 = DDR_ADDRMAP10;
	cfg->c_map.addrmap11 = DDR_ADDRMAP11;

	cfg->p_reg.pgcr = DDR_PGCR;
	cfg->p_reg.aciocr = DDR_ACIOCR; //(DDR_ACIOCR & ~(DDRPHYC_ACIOCR_ACSR_Msk)) | (0x02uL << DDRPHYC_ACIOCR_ACSR_Pos);
	cfg->p_reg.dxccr = DDR_DXCCR;
	cfg->p_reg.dsgcr = DDR_DSGCR;
	cfg->p_reg.dcr = DDR_DCR;
	cfg->p_reg.odtcr = DDR_ODTCR;

	cfg->p_timing.ptr0 = DDR_PTR0;
	cfg->p_timing.ptr1 = DDR_PTR1;
	cfg->p_timing.ptr2 = DDR_PTR2;
	cfg->p_timing.dtpr0 = DDR_DTPR0;
	cfg->p_timing.dtpr1 = DDR_DTPR1;
	cfg->p_timing.dtpr2 = DDR_DTPR2;
	cfg->p_timing.mr0 = DDR_MR0;
	cfg->p_timing.mr1 = DDR_MR1;
	cfg->p_timing.mr2 = DDR_MR2;
	cfg->p_timing.mr3 = DDR_MR3;

	cfg->p_reg.zq0cr1 = DDR_ZQ0CR1;
	cfg->p_reg.dx0gcr = DDR_DX0GCR;
	cfg->p_cal.dx0dllcr = DDR_DX0DLLCR;
	cfg->p_cal.dx0dqtr = DDR_DX0DQTR;
	cfg->p_cal.dx0dqstr = DDR_DX0DQSTR;
	cfg->p_reg.dx1gcr = DDR_DX1GCR;
	cfg->p_cal.dx1dllcr = DDR_DX1DLLCR;
	cfg->p_cal.dx1dqtr = DDR_DX1DQTR;
	cfg->p_cal.dx1dqstr = DDR_DX1DQSTR;
	cfg->p_reg.dx2gcr = DDR_DX2GCR;
	cfg->p_cal.dx2dllcr = DDR_DX2DLLCR;
	cfg->p_cal.dx2dqtr = DDR_DX2DQTR;
	cfg->p_cal.dx2dqstr = DDR_DX2DQSTR;
	cfg->p_reg.dx3gcr = DDR_DX3GCR;
	cfg->p_cal.dx3dllcr = DDR_DX3DLLCR;
	cfg->p_cal.dx3dqtr = DDR_DX3DQTR;
	cfg->p_cal.dx3dqstr = DDR_DX3DQSTR;
}
