## FSBL (First Stage Bootloader)
### Fast, lightweight replacement for U-Boot

** Work in Progress: see notes **

This project aims to replace U-Boot with my own hand-rolled bootloader.
While U-Boot excels at being portable (the U is for Universal), it comes at a cost of complexity,
which means debugging and customizing is difficult. Also, sometimes booting quickly is more important than having bootloader features.

This project replaces U-Boot's SPL loader (the FSBL) and U-Boot proper (the SSBL). 

It does a minimal number of tasks required to load an application into RAM and then boot into it:

  - Initialize the PLL clocks for the MPU and DDR RAM
  - Initialize the DDR RAM
  - Detect the boot method (NOR Flash, SD Card, EMMC, etc.)
  - Load the application image from the boot medium into DDR RAM
  - Jump into the application 

Also there are two steps that are not strictly necessary but are useful:

  - Initialize the UART to allow for printf()
  - Test the RAM

With the optional steps enabled, first-stage boot time is 30ms from power-on 
signal (valid Vdd) to the start of reading the app image.
Disabling the UART saves about 2ms. RAM Tests will likely change before I'm
done with this project. For now it just writes one word and verifies it.

After the initial 30ms, the remaining boot time is spent reading the app image from the boot media
(SD card, flash chip, etc). The duration of that of course depends on the
size of the image and the speed of the transfer.


#### Project status

Currently this only works on custom hardware which uses discrete LDOs and buck
converters instead of a PMIC chip. I am extending it to work on the Discovery
and OSD32 boards.

Here is the TODO list:

  * Add a step to initialize I2C, detect if PMIC is present, and configure voltage supplies if so.

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



