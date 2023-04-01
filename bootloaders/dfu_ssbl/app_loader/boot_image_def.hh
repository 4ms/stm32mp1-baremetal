#pragma once
#include <cstdint>

// This is the "legacy" (ie non-FIT) U-Boot image header
namespace BootImageDef
{

// U-Boot uses these values for the location of the next boot stage, so we will also:
constexpr uint32_t NorFlashSSBLAddr = 0x80000;
constexpr uint32_t SDCardSSBLPartition = 3;

constexpr uint32_t IH_MAGIC = 0x27051956; /* Image Magic Number		*/
constexpr uint32_t IH_NMLEN = 32;		  /* Image Name Length		*/

//  Legacy format image header,
//  all data in network byte order (aka natural aka bigendian).
//  Taken from u-boot include/image.h
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

} // namespace BootImageDef
