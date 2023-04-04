# USB DFU Bootloader

This project is still in early development.

USB DFU Bootloader is a second-stage bootloader (SSBL) that acts as a Device
Firmware Update (DFU) USB device. It allows you to load application firmware
onto an on-board Flash chip via a USB connection to a host computer. Planned
improvements are to support loading firmware onto an SDMMC card.

The host computer must run a DFU host such as dfu-util. The user runs a
terminal command to transfer a uimg file from the computer to the flash chip on
a custom board. The command is something like this:

``` 
dfu-util -a 0 -s 0x70080000 -D myfirmware.uimg
```

The given address (0x70080000) is an address on the flash chip plus 0x70000000.
So an address of 0x70090000 means flash address 0x90000, etc. The address must
be aligned to 4k sectors. The flash driver assumes the chip has uniform 4k
sectors. This can be easily changed to accomodate other chips with different
sector layouts.

A recent version of MP1-Boot allows for setting a pin to determine which
application to boot. If the pin is detected as pulled-down, firmware from
0x60000 on the flash chip will be loaded and executed. Otherwise, if the pin is
detected as pulled-up, firmware from 0x80000 on the flash chip will be loaded
an executed.

This was done with USB DFU in mind. A typical use would be to load the USB DFU
bootloader firmware to address 0x60000 and the main application to 0x80000
(using an SPI flash loader, or some other means such as J-Link or TRACE32). A
button or jumper can be setup so that MP1-Boot jumps into the main application
by default. Holding the button down will make it jump to the USB DFU
bootloader. From there, the user can flash new application firmware.


