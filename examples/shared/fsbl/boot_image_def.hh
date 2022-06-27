#pragma once
#include <cstdint>

namespace BootImageDef
{
constexpr uint32_t NorFlashSSBLAddr = 0x80000;
constexpr uint32_t LZ4F_MAGIC = 0x184D2204; /* LZ4 Magic Number		*/
constexpr uint32_t IH_MAGIC = 0x27051956;	/* Image Magic Number		*/
constexpr uint32_t IH_NMLEN = 32;			/* Image Name Length		*/

//  Legacy format image header,
//  all data in network byte order (aka natural aka bigendian).
struct image_header {
	uint32_t ih_magic;		   /* Image Header Magic Number	*/
	uint32_t ih_hcrc;		   /* Image Header CRC Checksum	*/
	uint32_t ih_time;		   /* Image Creation Timestamp	*/
	uint32_t ih_size;		   /* Image Data Size		*/
	uint32_t ih_load;		   /* Data	 Load  Address		*/
	uint32_t ih_ep;			   /* Entry Point Address		*/
	uint32_t ih_dcrc;		   /* Image Data CRC Checksum	*/
	uint8_t ih_os;			   /* Operating System		*/
	uint8_t ih_arch;		   /* CPU architecture		*/
	uint8_t ih_type;		   /* Image Type			*/
	uint8_t ih_comp;		   /* Compression Type		*/
	uint8_t ih_name[IH_NMLEN]; /* Image Name		*/
};

constexpr uint32_t HeaderSize = sizeof(image_header);

struct spl_image_info {
	const char *name;
	uint8_t os;
	uintptr_t load_addr;
	uintptr_t entry_point;
	uint32_t boot_device;
	uint32_t size;
	uint32_t flags;
	void *arg;
};

enum spl_image_flags {
	SPL_COPY_PAYLOAD_ONLY = 1,
};
} // namespace BootImageDef
