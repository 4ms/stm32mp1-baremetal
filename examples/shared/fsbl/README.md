## FSBL (First Stage Bootloader)
### Fast, lightweight and opinionated replacement for U-Boot

This project replaces U-Boot with a simple single-stage bootloader.

Normally U-Boot's First Stage Bootloader (SPL) loads the Second Stage Bootloader (U-Boot proper), which in turn loads the application.
When using this bootloader, it loads the application immediately.

#### Why not U-Boot?

Why re-invent the wheel? Primarily for educational purposes.

Secondarily, for a particular project I required fast booting with a somewhat
unorthodox boot algorithm, and had no need for all the features of U-Boot
(command line, USB gadget, etc).

U-Boot is excellent and solid, it should be your first choice for a project, especially when developing.
U-Boot excels at being portable (the "U" is for Universal), which means you can
port to another platform easily. However, the complexity means debugging and
customizing can be difficult, requiring a combination of modifying KConfig
files, device trees (DTS), C code, and sometimes even linker outputs (`ll_entry_start()` etc).

#### How it works

A simple config file defines the board (same config files for OSD32-BRK and Discovery boards used in other projects).

A bit of background/review: on power-up, the STM32MP15x BOOTROM attempts to read a 256-byte header from whatever interface the BOOT0/1/2
pins select (NOR Flash, SDMMC, etc). If the header has the right signature, then it will load the entire
image into SYSRAM at a designed address, 0x2FFC2400. Then it jumps to start executing code at 0x2FFC2500.
Details about the boot procedure can be found on the [st wiki](https://wiki.st.com/stm32mpu/wiki/STM32_MPU_ROM_code_overview).

The linker script for this project links the binary to the correct addresses in
SYSRAM, and pads 256 bytes for the header. A separate python script generates
the header. The resulting file is `fsbl.stm32`. We copy this file to the
boot medium (SD Card, for example) in the same way we copied the U-Boot images to the SD Card (using dd).
The image gets copied to the sectors that the BOOTROM will
be looking: LBA0 (address 0), and LBA512 (address 0x40000).

Once loaded, FSBL does the minimal tasks required to load an application into RAM and then boot into it:

  - Initialize the PLL clocks for the MPU and DDR RAM
  - Initialize security settings (allowing the application full access)
  - Configure the external PMIC chip, if present, to power the DDR RAM
  - Initialize the DDR RAM peripheral
  - Detect the boot method (NOR Flash, SD Card, EMMC, etc.)
  - Verify the application image header, and load the image from the boot medium into DDR RAM
  - Jump into the application 
  - Indicate an error on failure

Also there is one step that is not strictly necessary, but useful:

  - Initialize the UART to allow for printf() messages

#### Boot time

The fsbl image is aboutr 40kB. It takes BOOTROM about <F8>
With the optional step enabled, first-stage boot time is about 30ms from
power-on signal (valid Vdd) to the start of reading the app image.
Disabling the UART saves about 2ms. 

After the initial 30ms, the remaining boot time is spent reading the app image
from the boot media (SD card, flash chip, etc). The duration of that of course
depends on the size of the image and the speed of the transfer.


#### Project status

This currently only works with SD Card and NOR Flash booting. It can work with a PMIC system or a
discrete LDO system. 


  * Instructions for creating an app img (create a python script?)

  * Re-visit other example projects, ensuring they work with this FSBL

  * Rename this project: lwFSBL (lightweight FSBL)? M-Boot (minimal boot)? MP1-Boot?

  * Faster NOR loading (use Quad mode)

  * Faster SDMMC loading (debug why faster clock causes RXOVERR)

  * Add extensive RAM tests (run optionally)

  * Add a driver for EMMC (SDMMC2)


#### Using this

Unlike the other projects in this repository, you do not need to have U-boot installed on the SD Card or NOR flash.

In this directory run:

```
make image
```

In the build/ dir, you should see the fsbl.stm32 file. Copy it to the 1st and 2nd partitions of the SD card using `dd`

Create an img file from your app's elf file. Install the img file on the 3rd partition.

**TODO: Instructions how to create an img file and install it**

Reboot your board with a UART-to-USB cable connected, and watch FSBL's startup messages scroll by in a terminal.

You should see:

```
FSBL

Initializing RAM
name = DDR3-DDR3L 16bits 533000Khz
speed = 533000 kHz
size = 0x20000000
DDR: mem_speed (533000 kHz), RCC 528000 kHz
Testing RAM.
Booted from SD Card (1)
Reading app image header
Loading app image
Jumping to app
```



