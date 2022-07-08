extern "C" {
#include "crc32.h"
}
#include "compiler.h"
#include "part_efi.h"

// TODO: convert pr_err and log messages to print() format
#include "print_messages.hh"
// #define log(...)
// #define pr_err(...)

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
	bool valid_guid = false;
	for (unsigned i = 0; i < sizeof(pte.partition_type_guid.b); i++) {
		if (pte.partition_type_guid.b[i] != 0)
			valid_guid = true;
	}
	if (!valid_guid) {
		log("Invalid GUID (all zeroes) found on partition entry. Skipping\n");
		return false;
	}

	log("partition_type_guid[0]: %hhu\n", pte.partition_type_guid.b[0]);  // efi_guid_t
	log("unique_partition_guid: %hhu\n", pte.unique_partition_guid.b[0]); // efi_guid_t
	log("starting_lba: %llu\n", pte.starting_lba);						  //__le64
	log("ending_lba: %llu\n", pte.ending_lba);							  //__le64
	log("attributes: %llu\n", pte.attributes.raw);						  // gpt_entry_attributes
	// log("partition_name[PARTNAME_SZ]: %36s\n", (char *)pte.partition_name); // efi_char16_t
	return true;
}
