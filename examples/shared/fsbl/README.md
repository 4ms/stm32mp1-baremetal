## MP1-Boot 

### Minimal, Performant Bootloader for the STM32MP15x

This project replaces U-Boot with a simple single-stage bootloader.

Normally U-Boot's First Stage Bootloader (SPL) loads the Second Stage
Bootloader (U-Boot proper), which in turn loads the application. When using
this bootloader, it loads the application immediately.

#### Why not U-Boot?

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

#### How it works

A bit of background/review: on power-up, the STM32MP15x BOOTROM attempts to
read a 256-byte header from whatever interface the BOOT0/1/2 pins select (NOR
Flash, SDMMC, etc). If the header has the right signature, then it will load
the entire image into SYSRAM at a designed address, 0x2FFC2400. Then it jumps
to start executing code at 0x2FFC2500. Details about the boot procedure can be
found on the [st
wiki](https://wiki.st.com/stm32mpu/wiki/STM32_MPU_ROM_code_overview).

The linker script for this project links the binary to the correct addresses in
SYSRAM, and pads 256 bytes for the header. A separate python script generates
the header. The resulting file is `fsbl.stm32`. We copy this file to the boot
medium (SD Card, for example) in the same way we copied the U-Boot images to
the SD Card (using dd). The image gets copied to the sectors that the BOOTROM
will be looking: LBA0 (address 0), and LBA512 (address 0x40000).

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

  - Initialize the UART to allow for printf() messages

#### Boot time

Boot time varies widely depending on the board and the boot medium (SD Card or
NOR Flash).

With an SD Card on the OSD32-BRK, it takes about 500ms from power-on to loading
the application binary.

With NOR Flash on a custom board of mine, it takes about 70ms. 

The main reason for the large difference in boot time is that the BOOTROM uses
a clock speed of 21MHz for the NOR Flash, and 175kHz for SDMMC.

There also seems to be a board-specific difference in timing before BOOTROM
begins loading from the boot medium. From the Reset pulse on the OSD32-BRK
board, it takes about 120ms - 150ms for the BOOTROM to begin reading from the
SD Card. On a custom board of mine, it takes only 11ms. I don't know the reason
for this.

The MP1-Boot image is about 48kB. It takes the BOOTROM about 350ms to load it from
the SD Card. When using NOR Flash it takes about 18ms.

MP1-Boot takes about 33ms to run. If there is no PMIC, it's 2.5ms less. If UART
output is disabled, it's another 2ms faster.

The remaining boot time is spent reading the app image from the boot media. The
duration of that depends on the size of the image and the speed of the
transfer. Since we are no longer dependent on BOOTROM's hard-coded speeds and bus
widths, we can set our own transfer protocol rates. NOR Flash at 100MHz clock,
4-bit wide data path would take about 20ms to transfer a 1MB application image.

All-in-all, a decent sized application can be running in under 100ms.

#### Project status

This currently works with SD Card or NOR Flash booting. It also works with
a PMIC system or a discrete LDO system. 

  * Test with all example projects, ensuring they work with this

  * Faster NOR loading (use Quad mode)

  * Faster SDMMC loading (Debug OSD32-BRK speed limit of 16MHz)

  * Add extensive RAM tests (run optionally)

  * Add a driver for EMMC (SDMMC2)

  * Add a driver for UART booting

  * Reduce binary size by using our own RCC config instead of HAL (9kB)

  * Reduce binary size by using our own SDMMC config instead of HAL (10kB)

  * Reduce binary size by omitting printf library (9kB)


#### Using this

A simple config header defines such things as the console UART pins, whether or not the
board has a PMIC, etc. These are the same config files for OSD32-BRK and
Discovery boards used in other projects. Make sure you select the right
board namespace at the top of main.cc.

Unlike the other projects in this repository, you do not need to have U-boot
installed on the SD Card or NOR flash.

In this directory run:

```
make load
```

After building, it will prompt you for the disk device name of the SD Card.
Make sure a partitioned SD Card is inserted into your computer, and enter the
device name (such as `/dev/disk4`). The card must be partitioned the same way
it's done in the other example projects. Use the `scripts/partition-sdcard.sh`
script to do this.

Alternatively, you can load the image yourself:

```
make image
sudo dd if=build/fsbl.stm32 of=/dev/sdXX1
sudo dd if=build/fsbl.stm32 of=/dev/sdXX2
```

Finally, copy the application uimg file to the 3rd partition like this:

```
sudo dd if=../../appfolder/build/a7-main.uimg of=/dev/sdXX3
```

Reboot your board with a UART-to-USB cable connected, and watch the startup
messages scroll by in a terminal.

You should see:

```
MP1-Boot

Initializing RAM
Testing RAM
Booted from SD Card
Loading app image...
Jumping to app 
```



