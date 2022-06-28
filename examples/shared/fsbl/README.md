## FSBL (First Stage Bootloader)
### Fast, lightweight replacement for U-Boot

** Work in Progress: see notes **

This project aims to replace U-Boot with my own hand-rolled bootloader.
While U-Boot excels at being portable (the U is for Universal), it comes at a cost of complexity,
which means debugging and customizing is difficult. Also, sometimes booting quickly is more important than having bootloader features.

This project replaces U-Boot's SPL loader (the FSBL) and U-Boot proper (the SSBL). 

It does a minimal number of tasks required to load an application into RAM and then boot into it:

  - Initialize the PLL clocks for the MPU and DDR RAM
 
  - Initialize the UART to allow for printf()

  - Initialize the DDR RAM

  - Test the RAM

  - Detect the boot method (NOR Flash, SD Card, EMMC, etc.)

  - Read the application image header and determine the image size, load address, and entry address

  - Load the application image from the boot medium into DDR RAM

  - Run the application by jumping to its entry address

Of these, the UART initialization and the RAM tests are optional and could be
omitted if boot time was critical. With these enabled, first-stage boot time is
30ms from power-on signal (valid Vdd) to the start of reading the app image.
Disabling the UART saves about 2ms.

The remaining boot time is spent reading the app image from the boot media
(SD card, flash chip, etc). The duration of that of course depends on the
size of the image.


#### Project status

Currently this only works on custom hardware which uses discrete LDOs and buck
converters instead of a PMIC chip. I am extending it to work on the Discovery
and OSD32 boards.

Here is the TODO list:

  * Add step to initialize I2C, detect if PMIC is present, and configure voltage supplies if so.

  * Add driver for SD Card reading, including converting partition# to sector/block #.

  * Add other driver for EMMC 

  * Make NOR Flash driver faster by using faster clock speed


#### Using this

Unlike the other projects in this repository, you do not need to have U-boot installed on the SD Card or NOR flash.

To use, first make sure u-boot is installed on an SD-card, as usual (see README in the project root directory). 

Then, in this directory run:

```
make
```

In the build/ dir, you should see the a7-main.uimg file. Copy it to the 4th partition of the SD-card, as usual (again, see README in the project root dir).

Reboot your board with a UART-to-USB cable connected, and watch u-boot's startup messages scroll by in a terminal.

Then you should see:

```
A7: Hello from Cortex-A7!
A7: Resetting M4 core
A7: Starting M4 core after LED1 goes off in 3 2 1 now
M4: * yawn *
M4: Hello from Cortex-M4!
M4: I will be flashing LED2 slowly about 0.8Hz (if compiled at -O0)
```

And the LEDs should be flashing: LED2 slowly (M4 core), and LED1 rapidly (A7 core).


