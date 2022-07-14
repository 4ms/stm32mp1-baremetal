# MP1-Boot 

## Minimal, Performant Bootloader for the STM32MP15x

This project replaces U-Boot with a simple single-stage bootloader.

Normally U-Boot's First Stage Bootloader (SPL) loads the Second Stage
Bootloader (U-Boot proper), which in turn loads the application. When using
this bootloader, it loads the application immediately.

### Why not U-Boot?

Why re-invent the wheel? Primarily for educational purposes.

Secondarily, for a particular project I required fast booting with a somewhat
unorthodox boot algorithm, and had no need for all the features of U-Boot
(command line, USB gadget, etc).

U-Boot is excellent and solid, it should be your first choice for a project,
especially when developing. U-Boot excels at being portable (the "U" is for
Universal), which means you can port to another platform easily. However, the
complexity means debugging and customizing can be difficult, requiring a
combination of searching through and modifying KConfig files, device trees
(DTS), C code, and sometimes even linker outputs (`ll_entry_start()` etc).

### How it works

A bit of background/review: on power-up, the STM32MP15x BOOTROM attempts to
read a 256-byte header from whatever interface the BOOT0/1/2 pins select (NOR
Flash, SDMMC, etc). If the header has the right signature, then it will load
the entire image into SYSRAM at a designed address, 0x2FFC2400. Then it jumps
to start executing code at 0x2FFC2500. Details about the boot procedure can be
found on the [st
wiki](https://wiki.st.com/stm32mpu/wiki/STM32_MPU_ROM_code_overview).

The linker script for this project links the binary to the correct addresses in
SYSRAM, and pads 256 bytes for the header. A separate python script (taken from
[here](https://github.com/WerWolv/STM32MP1OS)) generates the header. The resulting
file is `fsbl.stm32`. We copy this image file to the SD Card using `dd` like we
do for the other example projects. The image gets copied to the sectors that
the BOOTROM will be looking: LBA0 (address 0), and LBA512 (address 0x40000).

Once loaded, MP1-Boot does the minimal tasks required to load an application into
RAM and then boot into it:

  - Initialize the PLL clocks for the MPU and DDR RAM
  - Initialize security settings (allowing the application full access)
  - Configure the external PMIC chip, if present, to power the DDR RAM
  - Initialize the DDR RAM peripheral
  - Detect the boot method (NOR Flash, SD Card, EMMC, etc.)
  - Verify the application image header, and load the image from the boot
	medium into DDR RAM
  - Jump into the application 
  - Indicate an error on failure

Also there is one step that is not strictly necessary, but useful:

  - Initialize the UART to allow for printing messages to the console

### Dependencies, attribution, and inspriation

The python script that adds the stm32 header is taken from
[here](https://github.com/WerWolv/STM32MP1OS/blob/master/fsbl/fix_header.py).

The DDR init code ported from U-Boot [ram
drivers](https://github.com/u-boot/u-boot/tree/master/drivers/ram/stm32mp1). I
also studied the port by @ua1arn in the hftrx project
[here](https://github.com/ua1arn/hftrx/blob/master/src/sdram/sdram.c).

The GPT and CRC32 code (for determining the block number on an SD Card based on
the partition number), is also ported from U-Boot
[disk drivers](https://github.com/u-boot/u-boot/blob/master/disk/part_efi.c).
The vast majority of code was removed, but the basic GPT structure and how to
read it remains.

The SDMCC driver is the STM32 HAL driver. The other drivers (QSPI NOR Flash,
I2C, RCC, GPIO, UART) are mine. Some leverage the register access macros ST
provides in the LL drivers.

### Boot time

**Summary**
	
Boot time varies widely depending on the board and the boot medium (SD Card or
NOR Flash), and even the particular brand/speed of SD Card.

With NOR Flash (requires a custom board), it takes about 65ms from power-on
until a 1MByte program is loaded and running. This breaks down to about 36ms
from power-on to app loading. Then the app is loaded at about 44MBytes/sec, so
a 1MB program loads in about 24ms.

With an SD Card on the OSD32-BRK, it takes about 500ms from power-on until a
1MB program is running. The times vary depending the brand/type of SD Card. It
takes around 380ms to until the the start of copying the app from the SD Card,
and another 100ms or so to load a 1MByte program.

**Details**

The main reason for the large difference in boot time is that when using an SD Card,
there is a ~300-350ms pause shortly after boot. Probing the SDMMC data/cmd/clk lines, 
you can see that the BOOTROM makes contact with the SD Card at about 11ms after power-on.
It communicates with a clock speed of 175kHz using only the CMD and CLK lines. Then
after a long pause (300-350ms) communication is resumed at 16MHz, using the D0-D3 lines
as well. I presume this pause is due the SD Card preparing itself, though I'm not certain.
In any case, with NOR Flash, this pause is not present. Also, when booting from NOR Flash,
the BOOTROM uses a clock speed of 32MHz, according to ST's site, but I measure
21MHz.

The MP1-Boot image is about 30kB. It takes the BOOTROM about 20-30ms to load it from
the SD Card. When using NOR Flash it takes about 11ms.

Once loaded, MP1-Boot takes about 13ms to run. If there is no PMIC or if the UART
is disabled, then it's a few ms faster.

The remaining boot time is spent copying the app image from the boot media into
DDR RAM. The duration of that depends on the size of the image and the speed of
the transfer. Since we are no longer dependent on BOOTROM's hard-coded speeds
and bus widths, we can set our own transfer protocol rates. NOR Flash can run
with an 88MHz clock, 4-bit wide data (Quad mode) takes about 25ms to transfer a
1MB application image. It probably could be pushed further, depending on the limits
of the Flash chip, but I've found this value to be reliable and fast enough.


### Project status

MP1-Boot works with booting from an SD Card or NOR Flash (QSPI). It will work with
a PMIC system or a discrete LDO system. 

TODO:

  * Faster SDMMC loading (Debug OSD32-BRK speed limit of 16MHz)
  * Add extensive RAM tests (run optionally)
  * Add a driver for EMMC (SDMMC2)
  * Add a driver for UART booting
  * Reduce binary size by using our own SDMMC config instead of HAL (10kB)


### Using this

A simple config header defines such things as the console UART pins, whether or not the
board has a PMIC, etc. These are the same config files for OSD32-BRK and
Discovery boards used in other projects. See examples of these files
[here](examples/shared/osd32brk_conf.hh) and [here](stm32disco_conf.hh). Make
sure you select the right board namespace at the top of main.cc.

Unlike the other projects in this repository, you do not need to have U-boot
installed on the SD Card or NOR flash.

In this directory run one of these commands:

```
make load                           # To be prompted for the SD card device

make load SD_DISK_DEV=/dev/diskX    # To use /dev/diskX
```

If you don't specify `SD_DISK_DEV`, it will prompt you for the disk device name
of the SD Card. Make sure a partitioned SD Card is inserted into your computer,
and enter the device name (such as `/dev/disk4`). 
(Alternatively, you can load the image yourself by running `make` followed by
`dd` commands to load `build/fsbl.stm32` onto partitions 1 and 2 of the SD
Card.)

The card must be partitioned the same way it's done in the other example
projects. Use the `scripts/partition-sdcard.sh` script to do this. 

Finally, copy the application uimg file to the 3rd partition like this:

```
sudo dd if=../../appfolder/build/a7-main.uimg of=/dev/sdXX3
```

Reboot your board with a UART-to-USB cable connected, and watch the startup
messages scroll by in a terminal.

You should see:

```
MP1-Boot

MPU clock: 800000000 Hz
Initializing RAM
Testing RAM.
Booted from NOR
Loading app image...
Jumping to app
```

...followed by the application running.



