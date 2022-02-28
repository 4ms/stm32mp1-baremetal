#undef STM32MP1BAREMETAL

#include <linux/kconfig.h>
#include <part.h>

static struct blk_desc _blkdesc;

static const unsigned _SECTOR_SIZE = 512;
// static const unsigned _NUM_SECTORS = 0x1000;
#define _NUM_SECTORS 0x1000
static uint8_t DUMMY_BLK_DEVICE[_NUM_SECTORS];

unsigned long blk_dread(struct blk_desc *block_dev, lbaint_t start, lbaint_t blkcnt, void *buffer)
{
	// if (block_dev != &_blkdesc) return -1;
	uint8_t *buf8 = (uint8_t *)buffer;
	for (unsigned i = 0; i < blkcnt; i++)
		buf8[start * _SECTOR_SIZE + i] = DUMMY_BLK_DEVICE[start * _SECTOR_SIZE + i];
	return 0;
}

unsigned long blk_dwrite(struct blk_desc *block_dev, lbaint_t start, lbaint_t blkcnt, const void *buffer)
{
	// if (block_dev != &_blkdesc) return -1;
	uint8_t *buf8 = (uint8_t *)buffer;
	for (unsigned i = 0; i < blkcnt; i++)
		DUMMY_BLK_DEVICE[start * _SECTOR_SIZE + i] = buf8[start * _SECTOR_SIZE + i];
	return 0;
}

int blk_get_device_part_str(const char *ifname,
							const char *dev_part_str,
							struct blk_desc **dev_desc,
							disk_partition_t *info,
							int allow_whole_dev)
{
	_blkdesc.blksz = _SECTOR_SIZE;
	_blkdesc.lba = _NUM_SECTORS;
	_blkdesc.devnum = 0;
	_blkdesc.hwpart = 0;
	info->start = 0;
	info->size = _NUM_SECTORS;

	*dev_desc = &_blkdesc;
	return 0;
}
