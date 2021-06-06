/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2013-2019 Toradex, Inc.
 *
 * Configuration settings for the Toradex Apalis iMX6
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "mx6_common.h"

#undef CONFIG_DISPLAY_BOARDINFO

#define CONFIG_MACH_TYPE		4886

#include <asm/arch/imx-regs.h>
#include <asm/mach-imx/gpio.h>

#ifdef CONFIG_SPL
#include "imx6_spl.h"
#endif

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_SERIAL_TAG

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(32 * 1024 * 1024)

#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE		UART1_BASE

/* I2C Configs */
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_MXC_I2C1		/* enable I2C bus 1 */
#define CONFIG_SYS_I2C_MXC_I2C2		/* enable I2C bus 2 */
#define CONFIG_SYS_I2C_MXC_I2C3		/* enable I2C bus 3 */
#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_SYS_MXC_I2C3_SPEED	400000

/* MMC Configs */
#define CONFIG_SYS_FSL_ESDHC_ADDR	0
#define CONFIG_SYS_FSL_USDHC_NUM	3

/*
 * SATA Configs
 */
#ifdef CONFIG_CMD_SATA
#define CONFIG_LBA48
#endif

/* Network */
#define CONFIG_FEC_MXC
#define IMX_FEC_BASE			ENET_BASE_ADDR
#define CONFIG_FEC_XCV_TYPE		RGMII
#define CONFIG_ETHPRIME			"FEC"
#define PHY_ANEG_TIMEOUT		15000 /* PHY needs longer aneg time */
#define CONFIG_FEC_MXC_PHYADDR		6
#define CONFIG_TFTP_TSIZE

/* USB Configs */
/* Host */
#define CONFIG_USB_MAX_CONTROLLER_COUNT		2
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET	/* For OTG port */
#define CONFIG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS		0
/* Client */
#define CONFIG_USBD_HS

/* Framebuffer and LCD */
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
#define CONFIG_VIDEO_BMP_RLE8
#define CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN
#define CONFIG_BMP_16BPP
#define CONFIG_VIDEO_LOGO
#define CONFIG_VIDEO_BMP_LOGO
#define CONFIG_CONSOLE_MUX
#define CONFIG_IMX_HDMI
#define CONFIG_IMX_VIDEO_SKIP

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/* Command definition */
#undef CONFIG_CMD_LOADB
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_FLASH

#undef CONFIG_IPADDR
#define CONFIG_IPADDR			192.168.10.2
#define CONFIG_NETMASK			255.255.255.0
#undef CONFIG_SERVERIP
#define CONFIG_SERVERIP			192.168.10.1

#define CONFIG_LOADADDR			0x12000000

#ifndef CONFIG_SPL_BUILD
#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0) \
	func(MMC, mmc, 1) \
	func(MMC, mmc, 2) \
	func(USB, usb, 0) \
	func(DHCP, dhcp, na)
#include <config_distro_bootcmd.h>
#undef BOOTENV_RUN_NET_USB_START
#define BOOTENV_RUN_NET_USB_START ""
#else /* CONFIG_SPL_BUILD */
#define BOOTENV
#endif /* CONFIG_SPL_BUILD */

#define DFU_ALT_EMMC_INFO \
	"u-boot.imx raw 0x2 0x3ff mmcpart 0;" \
	"boot part 0 1;" \
	"rootfs part 0 2;" \
	"zImage fat 0 1;" \
	"imx6q-apalis-eval.dtb fat 0 1;" \
	"imx6q-apalis-cam-eval.dtb fat 0 1"

#define EMMC_BOOTCMD \
	"set_emmcargs=setenv emmcargs ip=off root=PARTUUID=${uuid} " \
		"ro,noatime rootfstype=ext4 rootwait\0" \
	"emmcboot=run setup; run emmcfinduuid; run set_emmcargs; " \
		"setenv bootargs ${defargs} ${emmcargs} ${setupargs} " \
		"${vidargs}; echo Booting from internal eMMC chip...; "	\
		"run emmcdtbload; load mmc ${emmcdev}:${emmcbootpart} " \
		"${kernel_addr_r} ${boot_file} && run fdt_fixup && " \
		"bootz ${kernel_addr_r} ${dtbparam}\0" \
	"emmcbootpart=1\0" \
	"emmcdev=0\0" \
	"emmcdtbload=setenv dtbparam; load mmc ${emmcdev}:${emmcbootpart} " \
		"${fdt_addr_r} ${fdt_file} && " \
		"setenv dtbparam \" - ${fdt_addr_r}\" && true\0" \
	"emmcfinduuid=part uuid mmc ${mmcdev}:${emmcrootpart} uuid\0" \
	"emmcrootpart=2\0"

#define MEM_LAYOUT_ENV_SETTINGS \
	"bootm_size=0x20000000\0" \
	"fdt_addr_r=0x12100000\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"kernel_addr_r=0x11000000\0" \
	"pxefile_addr_r=0x17100000\0" \
	"ramdisk_addr_r=0x12200000\0" \
	"scriptaddr=0x17000000\0"

#define NFS_BOOTCMD \
	"nfsargs=ip=:::::eth0:on root=/dev/nfs ro\0" \
	"nfsboot=run setup; " \
		"setenv bootargs ${defargs} ${nfsargs} ${setupargs} " \
		"${vidargs}; echo Booting via DHCP/TFTP/NFS...; " \
		"run nfsdtbload; dhcp ${kernel_addr_r} " \
		"&& run fdt_fixup && bootz ${kernel_addr_r} ${dtbparam}\0" \
	"nfsdtbload=setenv dtbparam; tftp ${fdt_addr_r} ${fdt_file} " \
		"&& setenv dtbparam \" - ${fdt_addr_r}\" && true\0"

#ifndef CONFIG_TDX_APALIS_IMX6_V1_0
#define FDT_FILE "imx6q-apalis-eval.dtb"
#define FDT_FILE_V1_0 "imx6q-apalis_v1_0-eval.dtb"
#else
#define FDT_FILE "imx6q-apalis_v1_0-eval.dtb"
#endif
#define CONFIG_EXTRA_ENV_SETTINGS \
	BOOTENV \
	"bootcmd=setenv fdtfile ${fdt_file}; run distro_bootcmd ; " \
		"usb start ; " \
		"setenv stdout serial,vga ; setenv stdin serial,usbkbd\0" \
	"boot_file=zImage\0" \
	"console=ttymxc0\0" \
	"defargs=enable_wait_mode=off vmalloc=400M\0" \
	"dfu_alt_info=" DFU_ALT_EMMC_INFO "\0" \
	EMMC_BOOTCMD \
	"fdt_file=" FDT_FILE "\0" \
	"fdt_fixup=;\0" \
	MEM_LAYOUT_ENV_SETTINGS \
	NFS_BOOTCMD \
	"setethupdate=if env exists ethaddr; then; else setenv ethaddr " \
		"00:14:2d:00:00:00; fi; tftpboot ${loadaddr} " \
		"flash_eth.img && source ${loadaddr}\0" \
	"setsdupdate=setenv interface mmc; setenv drive 1; mmc rescan; " \
		"load ${interface} ${drive}:1 ${loadaddr} flash_blk.img " \
		"|| setenv drive 2; mmc rescan; load ${interface} ${drive}:1" \
		" ${loadaddr} flash_blk.img && " \
		"source ${loadaddr}\0" \
	"setup=setenv setupargs fec_mac=${ethaddr} " \
		"consoleblank=0 no_console_suspend=1 console=tty1 " \
		"console=${console},${baudrate}n8\0 " \
	"setupdate=run setsdupdate || run setusbupdate || run setethupdate\0" \
	"setusbupdate=usb start && setenv interface usb; setenv drive 0; " \
		"load ${interface} ${drive}:1 ${loadaddr} flash_blk.img && " \
		"source ${loadaddr}\0" \
	"splashpos=m,m\0" \
	"splashimage=" __stringify(CONFIG_LOADADDR) "\0" \
	"vidargs=mxc_hdmi.only_cea=1 " \
		"video=mxcfb0:dev=hdmi,1920x1080M@60,if=RGB24 " \
		"video=mxcfb1:off video=mxcfb2:off video=mxcfb3:off " \
		"fbmem=32M\0 "

/* Miscellaneous configurable options */
#undef CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE		1024
#undef CONFIG_SYS_MAXARGS
#define CONFIG_SYS_MAXARGS		48

#define CONFIG_SYS_MEMTEST_START	0x10000000
#define CONFIG_SYS_MEMTEST_END		0x10010000
#define CONFIG_SYS_MEMTEST_SCRATCH	0x10800000

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* environment organization */

#if defined(CONFIG_ENV_IS_IN_MMC)
/* Environment in eMMC, before config block at the end of 1st "boot sector" */
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART		1
#endif

#define CONFIG_CMD_TIME

#endif	/* __CONFIG_H */
