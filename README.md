### STM32MP1 Cortex-A7 baremetal basic project

This is a proof-of-concept and template project for a baremetal application on the Cortex A7 core of an STM32MP1 microprocessor.
All code is loaded onto a single A7 core, the M4 core or dual A7 core is not used (yet).

This should build and run on an OSD32MP1 BRK board. To adapt to a different STM32MP15x chip, some minor changes to the u-boot build will be required.

Debugging with gdb can be done via the SWD header on the OSD board (using a 10-pin pogo adaptor, and a J-link).

The project has two parts: bootloaders in `u-boot-stm32mp-2020/` and application in `ctest/`

The bootloader must be built once, and loaded once onto an SD Card, which is installed on the OSD board. 

The application ultimately needs to live on the SD Card as well, but it can be flashed into RAM using a J-Link flasher, making debugging much easier than having to copy files to an SD Card each time the code is changed.


## Building:

# 0) Setup:

Make sure to clone the submodule (4ms's u-boot fork):
`git submodule update --init`

On macOS, you may need to install gsed and set your PATH to use it instead of sed:
See Caveats section in `brew info gnu-sed` for details.
```
brew install gnu-sed
export PATH="/usr/local/opt/gnu-sed/libexec/gnubin:$PATH"`
```

# 1) Build and load U-boot:

```
# Set CROSS_COMPILE and KBUILD_OUTPUT environment vars
./setupenv.sh

# Build u-boot :
# (Ignore warnings about "format string is not a string literal") TODO: patch u-boot to fix this
cd u-boot-stm32mp-2020.01-r0/u-boot-stm32mp1-baremetal
make stm32mp15x_baremetal_defconfig
make -j16 all      # 16 => number of processors on your computer

# Verify outputs were created:
ls -l $KBUILD_OUTPUT/u-boot-spl.stm32
ls -l $KBUILD_OUTPUT/u-boot.img
```

Now you need to format and partition an SD Card. 
All SD Card actions are hard to do on macOS, so I use a virtual machine. Vagrant and ubunutu are easy to set up.

To format and create the necessary partitions, insert a card and do (from a virtual or real linux box):
```
mkfs.ext2 /dev/sd#
./partition-sdcard.sh /dev/sd#
```
...where /dev/sd# is the SD Card device name such as /dev/sdc

# 2) Power up OSD board

Attach a USB-to-UART device to the UART pins on the OSD board (use UART4 if you've got a custom board-- only TX/RX and GND are needed).
Start a terminal session that connects to the USB driver (I use minicom; there are many fine alternatives).

Insert the card into the OSD board and power it on. You should see boot messages, and then finally an error when it can't find
bare-arm.uimg. Now it's time to build that file.

# 3) Build the application
```
cd ctest
make 
ls -l build/app.elf
ls -l bare-arm.uimg
```
You should see the app.elf and the bare-arm.uimg files after building. Each of these is the compiled application, and either one must be loaded
in SDRAM at 0xC2000040. You can load the app.elf file by using a debugger/programmer such as J-link connected to the SWD pins.
Or, you can copy bare-arm.uimg to the SD Card in the fourth partition using normal linux cp commands. 
The former method is only temporary, requires a debugger to be attached, and will not persist after power down. However, it's much more convenient so it's preferred for Debug builds.
In the latter method, the application firmware is stored on the SD Card, so this is the method for production or long-term testing. The bootloader will load the application into 0xC2000040 on boot.

//Todo: Test if you have to copy bare-arm.uimg once, in order to get the debugger to work.//

# 4) Debug application

Use a J-link programer and Ozone to load the application and debug it. Create a new Ozone project for the STM32MP157C Core A7, and select the ctest/build/app.elf
file that gets created when you run `make` in ctest.

# 5) Copy application to SD card

When you want to have a version of the application load even without a debugger attached, you can load the application onto the SD Card.

You can move the SD Card from the OSD board and into your computer, but if you have the UART connected and your particular OS happens to be compatible with usb-gadget, then you might be able to mount the SD Card directly over UART without having to physically touch the card. Do this:
1) When the board is booting, up look for the message `Hit any key to stop autoboot`, and press a key.
2) You will see a UBOOT> prompt. Type the command: `ums 0 mmc 0`
3) In another terminal window, look to see that there's a new device, e.g. /dev/sdX 

If you're using a virtual machine you'll have too fiddle with the virtual machine host settings to get it to see the new device.

The `copy_to_sdcard.sh` script takes two arguments:
```
./copy_to_sdcard.sh filname /dev/sdXX
```
Where `filename` is the path to the bare-arm.uimg file (probably ctest/bare-arm.uimg) and `/dev/sdXX` is the SD Card's fourth partition, e.g. `/dev/sdc4`.
The script will mount the SD Card partition, remove the old bare-arm.uimg file, and copy the one you provided onto the correct place.

You can also take a look at the script and just do it manually or as part of your build process.

## Resources:

This guide is very helpful, although geared for a different platform:
http://umanovskis.se/files/arm-baremetal-ebook.pdf

