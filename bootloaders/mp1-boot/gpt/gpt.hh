#pragma once
#include "part_efi.h"
#include <cstdint>

uint32_t get_gpt_partition_block_addr(uint32_t partnum);
bool validate_gpt_header(gpt_header *gpt_h, lbaint_t lba, lbaint_t lastlba);
bool validate_partition_entry(const gpt_entry &pte);
