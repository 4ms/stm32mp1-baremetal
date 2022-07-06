## FSBL (First Stage Bootloader)
### Fast, lightweight and opinionated replacement for U-Boot

** Work in Progress: see notes **

This project replaces U-Boot with a simple single-stage bootloader.

It replaces both of U-Boot's stages (SPL and U-Boot proper) with one stage.
Normally U-Boot's SPL loads the Second Stage Bootloader (SSBL), which in turn loads the application.
When using this bootloader, it loads the application immediately.

#### Why not U-Boot?

Why re-invent the wheel? Primarily for educational purposes.
Secondarily, for a particular project I required fast booting with a somewhat
unorthodox boot sequence, and had no need for all the features of U-Boot
(command line, USB gadget, etc).

U-Boot is excellent and solid, it should be your first choice for a project.
U-Boot excels at being portable (the "U" is for Universal), which means you can
port to another platform easily. However, the complexity means debugging and
customizing can be difficult, requiring a combination of modifying KConfig
files, device trees (DTS), C code, and sometimes even linker outputs. 


It does the minimal tasks required to load an application into RAM and then boot into it:

  - Initialize the PLL clocks for the MPU and DDR RAM
  - Configure the PMIC, if present, to power the DDR RAM
  - Initialize the DDR RAM peripheral
  - Detect the boot method (NOR Flash, SD Card, EMMC, etc.)
  - Load the application image from the boot medium into DDR RAM
  - Jump into the application 
  - Indicate an error on failure

Also there are two steps that are not strictly necessary, but useful:

  - Initialize the UART to allow for printf() messages
  - Test the RAM

With the optional steps enabled, first-stage boot time is 30ms from power-on 
signal (valid Vdd) to the start of reading the app image.
Disabling the UART saves about 2ms. 

After the initial 30ms, the remaining boot time is spent reading the app image
from the boot media (SD card, flash chip, etc). The duration of that of course
depends on the size of the image and the speed of the transfer.

RAM Tests will likely change before I'm done with this project. For now it just
writes one word and verifies it.

#### Project status

This currently only works with SD Card and NOR Flash booting. It can work with a PMIC system or a
discrete LDO system. 

A simple config file defines the board (same config files for OSD32-BRK and Discovery boards used in other projects)

  * Instructions for creating an app img (create a python script?)

  * Add a driver for EMMC (SDMMC2)

  * Add extensive RAM tests (run optionally)

  * Re-visit other example projects, ensuring they work with this FSBL

  * Rename this project: lwFSBL (lightweight FSBL)? M-Boot (minimal boot)? MP1-Boot?

  * Faster NOR loading (use Quad mode)

  * Faster SDMMC loading (debug why faster clock causes RXOVERR)


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



