# From https://github.com/WerWolv/STM32MP1OS
# 

import struct
import sys

with open(sys.argv[1], "rb") as fsbl_file:
    fsbl = fsbl_file.read();

if fsbl[:4] == b"STM2":
    exit("Header already fixed")

payload = bytearray(fsbl[0x100:])

header = struct.pack("<ccccQQQQQQQQIIIIIIIIIIQQQQQQQQ83xb",
    bytes('S', "ascii"), bytes('T', "ascii"), bytes('M', "ascii"), bytes('2', "ascii"), # Header magic
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                     # ECDSA signature, unsigned here
    sum(payload),                                                                       # Checksum of payload, sum of all bytes
    0x00010000,                                                                         # Header version 1.0
    len(payload),                                                                       # Length of payload
    0x2FFC0000 + 0x2400 + 0x100,                                                        # Entrypoint address. SYSRAM + 0x2400 to skip past boot ROM data + 0x100 to skip past this header
    0x00,                                                                               # Reserved
    0x2FFC0000 + 0x2400,                                                                # Load address of image, unused
    0x00,                                                                               # Reserved
    0x00,                                                                               # Image version                                                                         
    0x01,                                                                               # Option flags, disable signature verification
    0x01,                                                                               # ECDSA algorithm set to P-256 NIST, unused
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                     # ECDSA signature, unsigned here
    0x00                                                                                # Binary type: U-Boot
    )

with open(sys.argv[2], "wb") as fsbl_file:
    fsbl_file.write(header + payload)
