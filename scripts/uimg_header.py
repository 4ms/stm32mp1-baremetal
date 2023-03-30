#!/usr/bin/env python3

import struct
import sys
import time
import zlib

# U-Boot compatible "legacy" image header
# Defined by U-Boot in u-boot repo: https://github.com/u-boot/u-boot/blob/master/include/image.h

magic = 0x27051956       

# These are the choices for some options in the U-Boot header
# MP1-Boot ignores these, but if you're using this script with
# U-Boot, then it might matter:

os_linux = 5             # U-Boot uses this for Linux OS images
os_uboot = 17            # U-Boot uses this for SSBL images

arch_arm = 2

image_type_kernel = 2    # U-Boot uses this for Linux OS images
image_type_firmware = 5  # U-Boot uses this for SSBL images

compress_none = 0        # MP1-Boot only supports uncompressed kernels, for now
# compress_gzip = 1
# compress_bzip2 = 2


with open(sys.argv[1], "rb") as bin_file:
    payload = bin_file.read();

#TODO: these should be cmdline args:
loadaddr = 0xC2000040
entryaddr = 0xC2000040
os = os_linux
image_type = image_type_kernel
compress = compress_none
image_name = bytes("stm32mp1-baremetal image", "ascii")

# Calculate some header entries:
datalen = len(payload)
tmstamp = int(time.time())
data_crc = zlib.crc32(payload) & 0xffffffff

# To calc the header CRC, we generate a header with the CRC zero'ed out.
# Then we calc the CRC of that, add fill the CRC value back in
header_no_crc = struct.pack(">IIIIIIIbbbb32s",  # BigEndian: 4-byte ints * 7, bytes * 4, 32-byte string
    magic,                                      # Image Header Magic Number	
    0,                                          # Placeholder for header CRC
    tmstamp,                                    # Image Creation Timestamp	
    datalen,                                    # Image Data Size		
    loadaddr,                                   # Data Load Address		
    entryaddr,                                  # Entry Point Address		
    data_crc,                                   # Image Data CRC Checksum	
    os,                                         # Operating System		
    arch_arm,                                   # CPU architecture		
    image_type,                                 # Image Type			
    compress,                                   # Compression Type		
    image_name,                                 # Image Name		
    )

hcrc = (zlib.crc32(header_no_crc) & 0xffffffff).to_bytes(4, 'big')
header = bytearray(header_no_crc)
header[4] = hcrc[0]
header[5] = hcrc[1]
header[6] = hcrc[2]
header[7] = hcrc[3]

with open(sys.argv[2], "wb") as uimg_file:
    uimg_file.write(header + payload)

