#include "compiler.h"
extern "C" {
#include "crc32.h"
}
#include "gpt/blk.h"
#include "memcpy.h"
#include "part_efi.h"
#include "print_messages.h"

struct blk_desc {
	uint32_t blksz;
};

u32 efi_crc32(const void *buf, u32 len)
{
	return crc32(0, (uint8_t *)buf, len);
}

bool validate_gpt_header(gpt_header *gpt_h, lbaint_t lba, lbaint_t lastlba)
{
	uint32_t crc32_backup = 0;
	uint32_t calc_crc32;

	/* Check the GPT header signature */
	if (le64_to_cpu(gpt_h->signature) != GPT_HEADER_SIGNATURE_UBOOT) {
		pr_err("%s signature is wrong: 0x%llX != 0x%llX\n",
			   "GUID Partition Table Header",
			   le64_to_cpu(gpt_h->signature),
			   GPT_HEADER_SIGNATURE_UBOOT);
		return false;
	}

	/* Check the GUID Partition Table CRC */
	crc32_backup = gpt_h->header_crc32;
	gpt_h->header_crc32 = 0;
	calc_crc32 = efi_crc32((const unsigned char *)gpt_h, le32_to_cpu(gpt_h->header_size));
	gpt_h->header_crc32 = crc32_backup;
	if (calc_crc32 != le32_to_cpu(crc32_backup)) {
		pr_err("%s CRC is wrong: 0x%x != 0x%x\n", "GUID Partition Table Header", le32_to_cpu(crc32_backup), calc_crc32);
		return false;
	}

	/*
	 * Check that the my_lba entry points to the LBA that contains the GPT
	 */
	if (le64_to_cpu(gpt_h->my_lba) != lba) {
		pr_err("GPT: my_lba incorrect: %llX != %lX\n", le64_to_cpu(gpt_h->my_lba), lba);
		return false;
	}

	/*
	 * Check that the first_usable_lba and that the last_usable_lba are
	 * within the disk. Also partition_entry_lba (DG)
	 */
	if (le64_to_cpu(gpt_h->first_usable_lba) > lastlba) {
		pr_err("GPT: first_usable_lba incorrect: %llX > %lX\n", le64_to_cpu(gpt_h->first_usable_lba), lastlba);
		return false;
	}
	if (le64_to_cpu(gpt_h->last_usable_lba) > lastlba) {
		pr_err("GPT: last_usable_lba incorrect: %llX > %lX\n", le64_to_cpu(gpt_h->last_usable_lba), lastlba);
		return false;
	}
	if (le64_to_cpu(gpt_h->partition_entry_lba) > lastlba) {
		pr_err("GPT: partition_entry_lba incorrect: %llX > %lX\n", le64_to_cpu(gpt_h->last_usable_lba), lastlba);
		return false;
	}

	if (le64_to_cpu(gpt_h->signature) == GPT_HEADER_CHROMEOS_IGNORE) {
		pr_err("ChromeOS 'IGNOREME' GPT header found and ignored\n");
		return false;
	}

	log("GPT: first_usable_lba: %llX last_usable_lba: %llX last lba: %lX\n",
		le64_to_cpu(gpt_h->first_usable_lba),
		le64_to_cpu(gpt_h->last_usable_lba),
		lastlba);

	log("Raw header:\n");
	log("signature: %llu\n", gpt_h->signature);				  //__le64
	log("revision: %u\n", gpt_h->revision);					  //__le32
	log("header_size: %u\n", gpt_h->header_size);			  //__le32
	log("header_crc32: %u\n", gpt_h->header_crc32);			  //__le32
	log("reserved1: %u\n", gpt_h->reserved1);				  //__le32
	log("my_lba: %llu\n", gpt_h->my_lba);					  //__le64
	log("alternate_lba: %llu\n", gpt_h->alternate_lba);		  //__le64
	log("first_usable_lba: %llu\n", gpt_h->first_usable_lba); //__le64
	log("last_usable_lba: %llu\n", gpt_h->last_usable_lba);	  //__le64
	// log("disk_guid: %lu\n", gpt_h->disk_guid);										// efi_guid_t
	log("partition_entry_lba: %llu\n", gpt_h->partition_entry_lba);				  //__le64
	log("num_partition_entries: %u\n", gpt_h->num_partition_entries);			  //__le32
	log("sizeof_partition_entry: %u\n", gpt_h->sizeof_partition_entry);			  //__le32
	log("partition_entry_array_crc32: %u\n", gpt_h->partition_entry_array_crc32); //__le32

	return true;
}

bool validate_partition_entry(const gpt_entry &pte)
{
	log("partition_type_guid[0]: %hhu\n", pte.partition_type_guid.b[0]);	// efi_guid_t
	log("unique_partition_guid: %hhu\n", pte.unique_partition_guid.b[0]);	// efi_guid_t
	log("starting_lba: %llu\n", pte.starting_lba);							//__le64
	log("ending_lba: %llu\n", pte.ending_lba);								//__le64
	log("attributes: %llu\n", pte.attributes.raw);							// gpt_entry_attributes
	log("partition_name[PARTNAME_SZ]: %36s\n", (char *)pte.partition_name); // efi_char16_t
	return true;
}

#if 0
static int find_valid_gpt(blk_desc *dev_desc, gpt_header *gpt_head, gpt_entry **pgpt_pte);

int part_get_info_efi(blk_desc *dev_desc, int part, disk_partition_t *info)
{
	gpt_header gpt_head; // align to blk size?
	gpt_entry gpt_pte;

	if (part < 1) {
		pr_err("%s: Invalid partition number\n", __func__);
		return -1;
	}

	/* This function validates AND fills in the GPT header and PTE */
	if (find_valid_gpt(dev_desc, &gpt_head, &gpt_pte) != 1)
		return -1;

	if (part > le32_to_cpu(gpt_head->num_partition_entries) || !is_pte_valid(&gpt_pte[part - 1])) {
		debug("%s: *** ERROR: Invalid partition number %d ***\n", __func__, part);
		free(gpt_pte);
		return -1;
	}

	/* The 'lbaint_t' casting may limit the maximum disk size to 2 TB */
	info->start = (lbaint_t)le64_to_cpu(gpt_pte[part - 1].starting_lba);
	/* The ending LBA is inclusive, to calculate size, add 1 to it */
	info->size = (lbaint_t)le64_to_cpu(gpt_pte[part - 1].ending_lba) + 1 - info->start;
	info->blksz = dev_desc->blksz;

	snprintf((char *)info->name, sizeof(info->name), "%s", print_efiname(&gpt_pte[part - 1]));
	strcpy((char *)info->type, "U-Boot");
	info->bootable = is_bootable(&gpt_pte[part - 1]);
#if CONFIG_IS_ENABLED(PARTITION_UUIDS)
	uuid_bin_to_str(gpt_pte[part - 1].unique_partition_guid.b, info->uuid, UUID_STR_FORMAT_GUID);
#endif
#ifdef CONFIG_PARTITION_TYPE_GUID
	uuid_bin_to_str(gpt_pte[part - 1].partition_type_guid.b, info->type_guid, UUID_STR_FORMAT_GUID);
#endif

	debug("%s: start 0x" LBAF ", size 0x" LBAF ", name %s\n", __func__, info->start, info->size, info->name);

	/* Remember to free pte */
	free(gpt_pte);
	return 0;
}

static int find_valid_gpt(struct blk_desc *dev_desc, gpt_header *gpt_head, gpt_entry **pgpt_pte)
{
	int r;

	r = is_gpt_valid(dev_desc, GPT_PRIMARY_PARTITION_TABLE_LBA, gpt_head, pgpt_pte);

	if (r != 1) {
		if (r != 2)
			pr_err("%s: *** ERROR: Invalid GPT ***\n", __func__);

		if (is_gpt_valid(dev_desc, (dev_desc->lba - 1), gpt_head, pgpt_pte) != 1) {
			pr_err("%s: *** ERROR: Invalid Backup GPT ***\n", __func__);
			return 0;
		}
		if (r != 2)
			pr_err("%s: ***        Using Backup GPT ***\n", __func__);
	}
	return 1;
}

/**
 * is_gpt_valid() - tests one GPT header and PTEs for validity
 *
 * lba is the logical block address of the GPT header to test
 * gpt is a GPT header ptr, filled on return.
 * ptes is a PTEs ptr, filled on return.
 *
 * Description: returns 1 if valid,  0 on error, 2 if ignored header
 * If valid, returns pointers to PTEs.
 */
// static int is_gpt_valid(struct blk_desc *dev_desc, u64 lba, gpt_header *pgpt_head, gpt_entry **pgpt_pte)
// {
// 	/* Confirm valid arguments prior to allocation. */
// 	if (!dev_desc || !pgpt_head) {
// 		pr_err("%s: Invalid Argument(s)\n", __func__);
// 		return 0;
// 	}

// 	// ALLOC_CACHE_ALIGN_BUFFER_PAD(legacy_mbr, mbr, 1, dev_desc->blksz);
// 	legacy_mbr mbr;

// 	/* Read MBR Header from device */
// 	if (blk_dread(dev_desc, 0, 1, (ulong *)&mbr) != 1) {
// 		pr_err("*** ERROR: Can't read MBR header ***\n");
// 		return 0;
// 	}

// 	/* Read GPT Header from device */
// 	if (blk_dread(dev_desc, (lbaint_t)lba, 1, pgpt_head) != 1) {
// 		pr_err("*** ERROR: Can't read GPT header ***\n");
// 		return 0;
// 	}

// 	/* Invalid but nothing to yell about. */
// 	if (le64_to_cpu(pgpt_head->signature) == GPT_HEADER_CHROMEOS_IGNORE) {
// 		debug("ChromeOS 'IGNOREME' GPT header found and ignored\n");
// 		return 2;
// 	}

// 	if (validate_gpt_header(pgpt_head, (lbaint_t)lba, dev_desc->lba))
// 		return 0;

// 	if (dev_desc->sig_type == SIG_TYPE_NONE) {
// 		efi_guid_t empty = {};
// 		if (memcmp(&pgpt_head->disk_guid, &empty, sizeof(empty))) {
// 			dev_desc->sig_type = SIG_TYPE_GUID;
// 			memcpy(&dev_desc->guid_sig, &pgpt_head->disk_guid, sizeof(empty));
// 		} else if (mbr->unique_mbr_signature != 0) {
// 			dev_desc->sig_type = SIG_TYPE_MBR;
// 			dev_desc->mbr_sig = mbr->unique_mbr_signature;
// 		}
// 	}
// 	return 1;
// }

static int read_gpt_entry(struct blk_desc *dev_desc, u64 lba, gpt_header *pgpt_head, gpt_entry **pgpt_pte)
{
	/* Read and allocate Partition Table Entries */
	*pgpt_pte = alloc_read_gpt_entries(dev_desc, pgpt_head);
	if (*pgpt_pte == NULL) {
		pr_err("GPT: Failed to allocate memory for PTE\n");
		return 0;
	}

	if (validate_gpt_entries(pgpt_head, *pgpt_pte)) {
		free(*pgpt_pte);
		return 0;
	}

	/* We're done, all's well */
	return 1;
}

static gpt_entry *alloc_read_gpt_entries(struct blk_desc *dev_desc, gpt_header *pgpt_head)
{
	size_t count = 0, blk_cnt;
	lbaint_t blk;
	gpt_entry *pte = NULL;

	if (!dev_desc || !pgpt_head) {
		pr_err("%s: Invalid Argument(s)\n", __func__);
		return NULL;
	}

	count = le32_to_cpu(pgpt_head->num_partition_entries) * le32_to_cpu(pgpt_head->sizeof_partition_entry);

	debug("%s: count = %u * %u = %lu\n",
		  __func__,
		  (u32)le32_to_cpu(pgpt_head->num_partition_entries),
		  (u32)le32_to_cpu(pgpt_head->sizeof_partition_entry),
		  (ulong)count);

	/* Allocate memory for PTE, remember to FREE */
	if (count != 0) {
		pte = memalign(ARCH_DMA_MINALIGN, PAD_TO_BLOCKSIZE(count, dev_desc));
	}

	if (count == 0 || pte == NULL) {
		pr_err("%s: ERROR: Can't allocate %#lX bytes for GPT Entries\n", __func__, (ulong)count);
		return NULL;
	}

	/* Read GPT Entries from device */
	blk = le64_to_cpu(pgpt_head->partition_entry_lba);
	blk_cnt = BLOCK_CNT(count, dev_desc);
	if (blk_dread(dev_desc, blk, (lbaint_t)blk_cnt, pte) != blk_cnt) {
		pr_err("*** ERROR: Can't read GPT Entries ***\n");
		free(pte);
		return NULL;
	}
	return pte;
}

static int validate_gpt_entries(gpt_header *gpt_h, gpt_entry *gpt_e)
{
	uint32_t calc_crc32;

	/* Check the GUID Partition Table Entry Array CRC */
	calc_crc32 = efi_crc32((const unsigned char *)gpt_e,
						   le32_to_cpu(gpt_h->num_partition_entries) * le32_to_cpu(gpt_h->sizeof_partition_entry));

	if (calc_crc32 != le32_to_cpu(gpt_h->partition_entry_array_crc32)) {
		pr_err("%s: 0x%x != 0x%x\n",
			   "GUID Partition Table Entry Array CRC is wrong",
			   le32_to_cpu(gpt_h->partition_entry_array_crc32),
			   calc_crc32);
		return -1;
	}

	return 0;
}

#endif
