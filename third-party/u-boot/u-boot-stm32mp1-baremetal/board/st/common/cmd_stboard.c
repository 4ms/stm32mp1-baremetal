// SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause
/*
 * Copyright (C) 2019, STMicroelectronics - All Rights Reserved
 */

#ifndef CONFIG_SPL_BUILD
#include <common.h>
#include <console.h>
#include <misc.h>
#include <dm/device.h>
#include <dm/uclass.h>

static bool check_stboard(u16 board)
{
	unsigned int i;
	const u16 st_board_id[] = {
		0x1272,
		0x1263,
		0x1264,
		0x1298,
		0x1341,
		0x1497,
	};

	for (i = 0; i < ARRAY_SIZE(st_board_id); i++)
		if (board == st_board_id[i])
			return true;

	return false;
}

static void display_stboard(u32 otp)
{
	printf("Board: MB%04x Var%d.%d Rev.%c-%02d\n",
	       otp >> 16,
	       (otp >> 12) & 0xF,
	       (otp >> 4) & 0xF,
	       ((otp >> 8) & 0xF) - 1 + 'A',
	       otp & 0xF);
}

static int do_stboard(cmd_tbl_t *cmdtp, int flag, int argc,
		      char * const argv[])
{
	int ret;
	u32 otp, lock;
	u8 revision;
	unsigned long board, var_cpn, var_fg, bom;
	struct udevice *dev;
	int confirmed = argc == 7 && !strcmp(argv[1], "-y");

	argc -= 1 + confirmed;
	argv += 1 + confirmed;

	if (argc != 0 && argc != 5)
		return CMD_RET_USAGE;

	ret = uclass_get_device_by_driver(UCLASS_MISC,
					  DM_GET_DRIVER(stm32mp_bsec),
					  &dev);

	ret = misc_read(dev, STM32_BSEC_OTP(BSEC_OTP_BOARD),
			&otp, sizeof(otp));

	if (ret < 0) {
		puts("OTP read error");
		return CMD_RET_FAILURE;
	}

	ret = misc_read(dev, STM32_BSEC_LOCK(BSEC_OTP_BOARD),
			&lock, sizeof(lock));
	if (ret < 0) {
		puts("LOCK read error");
		return CMD_RET_FAILURE;
	}

	if (argc == 0) {
		if (!otp)
			puts("Board : OTP board FREE\n");
		else
			display_stboard(otp);
		printf("      OTP %d %s locked !\n", BSEC_OTP_BOARD,
		       lock == 1 ? "" : "NOT");
		return CMD_RET_SUCCESS;
	}

	if (otp) {
		display_stboard(otp);
		printf("ERROR: OTP board not FREE\n");
		return CMD_RET_FAILURE;
	}

	if (strict_strtoul(argv[0], 16, &board) < 0 ||
	    board == 0 || board > 0xFFFF) {
		printf("argument %d invalid: %s\n", 1, argv[0]);
		return CMD_RET_USAGE;
	}

	if (strict_strtoul(argv[1], 10, &var_cpn) < 0 ||
	    var_cpn == 0 || var_cpn > 15) {
		printf("argument %d invalid: %s\n", 2, argv[1]);
		return CMD_RET_USAGE;
	}

	revision = argv[2][0] - 'A' + 1;
	if (strlen(argv[2]) > 1 || revision == 0 || revision > 15) {
		printf("argument %d invalid: %s\n", 3, argv[2]);
		return CMD_RET_USAGE;
	}

	if (strict_strtoul(argv[3], 10, &var_fg) < 0 ||
	    var_fg > 15) {
		printf("argument %d invalid: %s\n", 4, argv[3]);
		return CMD_RET_USAGE;
	}

	if (strict_strtoul(argv[4], 10, &bom) < 0 ||
	    bom == 0 || bom > 15) {
		printf("argument %d invalid: %s\n", 4, argv[3]);
		return CMD_RET_USAGE;
	}

	otp = (board << 16) | (var_cpn << 12) | (revision << 8) |
	      (var_fg << 4) | bom;
	display_stboard(otp);
	printf("=> OTP[%d] = %08X\n", BSEC_OTP_BOARD, otp);

	if (!check_stboard((u16)board)) {
		printf("Unknown board MB%04x\n", (u16)board);
		return CMD_RET_FAILURE;
	}
	if (!confirmed) {
		printf("Warning: Programming BOARD in OTP is irreversible!\n");
		printf("Really perform this OTP programming? <y/N>\n");

		if (!confirm_yesno()) {
			puts("BOARD programming aborted\n");
			return CMD_RET_FAILURE;
		}
	}

	ret = misc_write(dev, STM32_BSEC_OTP(BSEC_OTP_BOARD),
			 &otp, sizeof(otp));

	if (ret < 0) {
		puts("BOARD programming error\n");
		return CMD_RET_FAILURE;
	}

	/* write persistent lock */
	otp = 1;
	ret = misc_write(dev, STM32_BSEC_LOCK(BSEC_OTP_BOARD),
			 &otp, sizeof(otp));
	if (ret < 0) {
		puts("BOARD lock error\n");
		return CMD_RET_FAILURE;
	}

	puts("BOARD programming done\n");

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(stboard, 7, 0, do_stboard,
	   "read/write board reference in OTP",
	   "\n"
	   "  Print current board information\n"
	   "stboard [-y] <Board> <VarCPN> <Revision> <VarFG> <BOM>\n"
	   "  Write board information\n"
	   "  - Board: xxxx, example 1264 for MB1264\n"
	   "  - VarCPN: 1...15\n"
	   "  - Revision: A...O\n"
	   "  - VarFG: 0...15\n"
	   "  - BOM: 1...15\n");

#endif
