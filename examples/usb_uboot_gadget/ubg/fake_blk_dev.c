#undef STM32MP1BAREMETAL

#include <linux/kconfig.h>
#include <part.h>

static struct blk_desc _blkdesc;
static uint8_t DUMMY_BLK_DEVICE[0x1000];
#define SECTOR_SIZE 512

unsigned long blk_dread(struct blk_desc *block_dev, lbaint_t start, lbaint_t blkcnt, void *buffer)
{
	// if (block_dev != &_blkdesc) return -1;
	uint8_t *buf8 = (uint8_t *)buffer;
	for (unsigned i = 0; i < blkcnt; i++)
		buf8[start * SECTOR_SIZE + i] = DUMMY_BLK_DEVICE[start * SECTOR_SIZE + i];
	return 0;
}

unsigned long blk_dwrite(struct blk_desc *block_dev, lbaint_t start, lbaint_t blkcnt, const void *buffer)
{
	// if (block_dev != &_blkdesc) return -1;
	uint8_t *buf8 = (uint8_t *)buffer;
	for (unsigned i = 0; i < blkcnt; i++)
		DUMMY_BLK_DEVICE[start * SECTOR_SIZE + i] = buf8[start * SECTOR_SIZE + i];
	return 0;
}

int blk_get_device_part_str(const char *ifname,
							const char *dev_part_str,
							struct blk_desc **dev_desc,
							disk_partition_t *info,
							int allow_whole_dev)
{
	_blkdesc.blksz = 512;
	*dev_desc = &_blkdesc;
	return 0;
}
