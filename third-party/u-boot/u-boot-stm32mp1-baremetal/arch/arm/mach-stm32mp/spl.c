// SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause
/*
 * Copyright (C) 2018, STMicroelectronics - All Rights Reserved
 */

#include <common.h>
#include <dm.h>
#include <spl.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <linux/libfdt.h>

u32 spl_boot_device(void)
{
	u32 boot_mode;

	boot_mode = get_bootmode();

	switch (boot_mode) {
	case BOOT_FLASH_SD_1:
	case BOOT_FLASH_EMMC_1:
		return BOOT_DEVICE_MMC1;
	case BOOT_FLASH_SD_2:
	case BOOT_FLASH_EMMC_2:
		return BOOT_DEVICE_MMC2;
	case BOOT_SERIAL_UART_1:
	case BOOT_SERIAL_UART_2:
	case BOOT_SERIAL_UART_3:
	case BOOT_SERIAL_UART_4:
	case BOOT_SERIAL_UART_5:
	case BOOT_SERIAL_UART_6:
	case BOOT_SERIAL_UART_7:
	case BOOT_SERIAL_UART_8:
		return BOOT_DEVICE_UART;
	case BOOT_SERIAL_USB_OTG:
		return BOOT_DEVICE_USB;
	case BOOT_FLASH_NAND_FMC:
		return BOOT_DEVICE_NAND;
	case BOOT_FLASH_NOR_QSPI:
		return BOOT_DEVICE_SPI;
	case BOOT_FLASH_SPINAND_1:
		return BOOT_DEVICE_NONE; /* SPINAND not supported in SPL */
	}

	return BOOT_DEVICE_MMC1;
}

u32 spl_boot_mode(const u32 boot_device)
{
	return MMCSD_MODE_RAW;
}

int spl_boot_partition(const u32 boot_device)
{
	switch (boot_device) {
	case BOOT_DEVICE_MMC1:
		return CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_PARTITION;
	case BOOT_DEVICE_MMC2:
		return CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_PARTITION_MMC2;
	default:
		return -EINVAL;
	}
}

#ifdef CONFIG_SPL_DISPLAY_PRINT
void spl_display_print(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	const char *model;

	/* same code than show_board_info() but not compiled for SPL
	 * see CONFIG_DISPLAY_BOARDINFO & common/board_info.c
	 */
	model = fdt_getprop(gd->fdt_blob, 0, "model", NULL);
	if (model)
		printf("Model: %s\n", model);
}
#endif

__weak int board_vddcore_set(void)
{
	return 0;
}

void board_init_f(ulong dummy)
{
	struct udevice *dev;
	int ret, clk, reset, pinctrl, power;

	arch_cpu_init();

	ret = spl_early_init();
	if (ret) {
		debug("%s: spl_early_init() failed: %d\n", __func__, ret);
		hang();
	}

	clk = uclass_get_device(UCLASS_CLK, 0, &dev);
	if (clk)
		debug("%s: Clock init failed: %d\n", __func__, clk);

	reset = uclass_get_device(UCLASS_RESET, 0, &dev);
	if (reset)
		debug("%s: Reset init failed: %d\n", __func__, reset);

	pinctrl = uclass_get_device(UCLASS_PINCTRL, 0, &dev);
	if (pinctrl)
		debug("%s: Cannot find pinctrl device: %d\n",
		      __func__, pinctrl);

	/* enable console uart printing */
	preloader_console_init();

	/* change vddcore if needed after clock tree init */
	power = board_vddcore_set();

	if (clk || reset || pinctrl || power)
		printf("%s: probe failed clk=%d reset=%d pinctrl=%d power=%d\n",
		       __func__, clk, reset, pinctrl, power);

	ret = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (ret) {
		printf("DRAM init failed: %d\n", ret);
		hang();
	}
}
