/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2011 Samsung Electrnoics
 * Lukasz Majewski <l.majewski@samsung.com>
 */

#ifndef __USB_MASS_STORAGE_H__
#define __USB_MASS_STORAGE_H__

#define SECTOR_SIZE		0x200

// #ifdef STM32MP1BAREMETAL
// #include "blk.h"

#include <part.h>
// #include <linux/usb/composite.h>

/* Wait at maximum 60 seconds for cable connection */
#define UMS_CABLE_READY_TIMEOUT	60

#ifdef __cplusplus
extern "C" {
#endif

struct ums {
	int (*read_sector)(struct ums *ums_dev,
			   ulong start, lbaint_t blkcnt, void *buf);
	int (*write_sector)(struct ums *ums_dev,
			    ulong start, lbaint_t blkcnt, const void *buf);
	unsigned int start_sector;
	unsigned int num_sectors;
	const char *name;
	struct blk_desc block_dev;
};

int fsg_init(struct ums *ums_devs, int count);
void fsg_cleanup(void);
int fsg_main_thread(void *);
//int fsg_add(struct usb_configuration *c);


int do_usb_mass_storage( unsigned int controller_index, const char* devtype, const char*devnum);

#ifdef __cplusplus
}
#endif 

#endif /* __USB_MASS_STORAGE_H__ */
