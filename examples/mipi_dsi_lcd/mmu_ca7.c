/****************************************************************************
 * @file     mmu_ARMCA7.c
 * @brief    MMU Configuration for Arm Cortex-A7 Device Series
 * @version  V1.2.0
 * @date     15. May 2019
 *
 * @note
 *
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * Modified by Dan Green, 2021
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "stm32mp157cxx_ca7.h"

// Todo: use values from the linker script, don't redefine here
#define __ROM_BASE 0xC2000000
#define __ROM_SIZE 0x04000000 /* 64M */

#define __RAM_BASE 0xC0200000
#define __RAM_SIZE (__ROM_BASE - __RAM_BASE)

#define __RAM2_BASE 0xC2100000
#define __RAM2_SIZE (0xD0000000 - __RAM2_BASE)

#define __HEAP_BASE 0xD0000000
#define __HEAP_SIZE 0x10000000

#define __TTB_BASE 0xC0100000

#define A7_SYSRAM_BASE 0x2FFC0000
#define A7_SYSRAM_SIZE 0x00040000			  /* 256kB */
#define A7_SYSRAM_1MB_SECTION_BASE 0x2FF00000 /* 1M below top of SYSRAM */

#define A7_SRAM1_BASE (0x10000000UL)
#define A7_SRAM2_BASE (0x10020000UL)
#define A7_SRAM3_BASE (0x10040000UL)
#define A7_SRAM4_BASE (0x10050000UL)

#define M4_VECTORS_BASE (0x38000000UL)

// TTB base address
#define TTB_BASE ((uint32_t *)__TTB_BASE)

// L2 table pointers
//----------------------------------------

// The L1 translation table divides the full 4GB address space of a 32-bit core
// into 4096 equally sized sections, each of which describes 1MB of virtual memory space.
// The L1 translation table therefore contains 4096 32-bit (word-sized) entries.
#define TTB_L1_SIZE (0x00004000)
#define TTB_L2_SIZE (0x00000400)

//"Private" peripherals is the GIC on STM32MP15x
#define PRIVATE_TABLE_L2_BASE_4k (__TTB_BASE + TTB_L1_SIZE)

/* #define M4_VECTORS_TABLE_L2_BASE_64K (__TTB_BASE + TTB_L1_SIZE + 0x400) // Map 64k */

static uint32_t Sect_Normal;	 // outer & inner wb/wa, non-shareable, executable, rw, domain 0, base addr 0
static uint32_t Sect_Normal_Cod; // outer & inner wb/wa, non-shareable, executable, ro, domain 0, base addr 0
static uint32_t Sect_Normal_RO;	 // as Sect_Normal_Cod, but not executable
static uint32_t Sect_Normal_RW;	 // as Sect_Normal_Cod, but writeable and not executable
static uint32_t Sect_Device_RO;	 // device, non-shareable, non-executable, ro, domain 0, base addr 0
static uint32_t Sect_Device_RW;	 // as Sect_Device_RO, but writeable
static uint32_t Sect_StronglyOrdered;
static uint32_t Sect_Normal_NonCache;

static uint32_t Page_L1_4k = 0x0;	// generic
static uint32_t Page_L1_64k = 0x0;	// generic
static uint32_t Page_4k_Device_RW;	// Shared device, not executable, rw, domain 0
static uint32_t Page_64k_Device_RW; // Shared device, not executable, rw, domain 0

void MMU_CreateTranslationTable(void)
{
	mmu_region_attributes_Type region;

	// Make entire memory area 0x00000000 to 0xFFFFFFFF faulting (invalid), unless we specify otherwise
	MMU_TTSection(TTB_BASE, 0, 4096, DESCRIPTOR_FAULT);

	section_normal(Sect_Normal, region);
	section_normal_cod(Sect_Normal_Cod, region);
	section_normal_ro(Sect_Normal_RO, region);
	section_normal_rw(Sect_Normal_RW, region);
	section_device_ro(Sect_Device_RO, region);
	section_device_rw(Sect_Device_RW, region);
	section_so(Sect_StronglyOrdered, region);
	section_normal_nc(Sect_Normal_NonCache, region);
	page64k_device_rw(Page_L1_64k, Page_64k_Device_RW, region);
	page4k_device_rw(Page_L1_4k, Page_4k_Device_RW, region);

	// Note from DG:
	// ARM Cortex-A Series Programmer's Guide v4.0, section 17.2.8:
	// "use large MMU mappings (supersections or sections in preference to 4KB pages) as this reduces the cost of
	// individual translation table walks"
	// So we use 1MB sections a lot here.

	// Note from DG:
	// ROM should be Sect_Normal_Cod (RO), but that seems to interfere with SWD/JTAG debugger loading an elf file
	// Setting it to Normal works better. This shouldn't matter when U-boot loads the firmware.
	// Todo: Investigate why this is!
	MMU_TTSection(TTB_BASE, __ROM_BASE, __ROM_SIZE / 0x100000, Sect_Normal);

	// RAM is RW, cacheable
	MMU_TTSection(TTB_BASE, __RAM_BASE, __RAM_SIZE / 0x100000, Sect_Normal_RW);
	MMU_TTSection(TTB_BASE, __RAM2_BASE, __RAM2_SIZE / 0x100000, Sect_StronglyOrdered);
	MMU_TTSection(TTB_BASE, __HEAP_BASE, __HEAP_SIZE / 0x100000, Sect_Normal_RW);

	// SRAM1-4, used by Cortex-M4 MCU for code execution and stack
	// It's actually is only 384kB, but we can set the whole 1MB section
	MMU_TTSection(TTB_BASE, A7_SRAM1_BASE, 1, Sect_Device_RW);

	// Peripheral memory. Again, the actual peripherals use  only bits of pieces of the address spaces
	MMU_TTSection(TTB_BASE, 0x40000000, 0x10000000 / 0x100000, Sect_Device_RW);
	MMU_TTSection(TTB_BASE, 0x50000000, 0x10000000 / 0x100000, Sect_Device_RW);

	// SYSRAM (256kB MCU RAM)
	MMU_TTSection(TTB_BASE, A7_SYSRAM_1MB_SECTION_BASE, 1, Sect_Normal_RW);

	// GIC, aka "Private peripherals". Starts at the address returned by __get_CBAR()
	// For no particular reason, we use 4kb pages here instead of a 1MB section.
	// Mostly I just wanted to document-by-example the process of using 4kb pages.
	//
	// Create (256 * 4k)=1MB faulting entries
	MMU_TTPage4k(TTB_BASE, __get_CBAR(), 256, Page_L1_4k, (uint32_t *)PRIVATE_TABLE_L2_BASE_4k, DESCRIPTOR_FAULT);
	// Define private address space entry. 8 pages of 4k each = 32k = 0x8000
	MMU_TTPage4k(TTB_BASE, __get_CBAR(), 8, Page_L1_4k, (uint32_t *)PRIVATE_TABLE_L2_BASE_4k, Page_4k_Device_RW);

	// Co-processor vector table (Cortex-M4)
	// From A7's perspective, the M4 vectors are at 0x38000000 (form M4's perspective, they're at 0x00000000)
	MMU_TTSection(TTB_BASE, M4_VECTORS_BASE, 1, Sect_Device_RW);

	/* Set location of level 1 page table
	; 31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
	; 13:7  - 0x0
	; 6     - IRGN[0] 0x1  (Inner WB WA)
	; 5     - NOS     0x0  (Non-shared)
	; 4:3   - RGN     0x01 (Outer WB WA)
	; 2     - IMP     0x0  (Implementation Defined)
	; 1     - S       0x0  (Non-shared)
	; 0     - IRGN[1] 0x0  (Inner WB WA) */
	__set_TTBR0(__TTB_BASE | 0x48);
	__ISB();

	/* Set up domain access control register
	; We set domain 0 to Client and all other domains to No Access.
	; All translation table entries specify domain 0 */
	__set_DACR(1);
	__ISB();
}
