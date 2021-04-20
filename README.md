# STM32MP1 Cortex-A7 baremetal basic project

This is a template project for a baremetal application on the Cortex A7 core of an STM32MP1 microprocessor. It also is a useful introduction to what's needed for a baremetal Cortex-A7 application.
Presently, all only a single A7 core is used; the M4 core or second A7 core are not used (yet).

I welcome any issues, questions, bug reports or pull requests! Please note that this project assumes you are familiar with embedded programming, toolchains, and support hardware (programmers, debuggers, etc).

This will build and run on an OSD32MP1 BRK board. To adapt to a different STM32MP15x chip, some minor changes to the u-boot build will be required.

Debugging with gdb can be done via the SWD header on the OSD board (using a 10-pin pogo adaptor, such as [this one](https://www.tag-connect.com/product/tc2050-idc-nl-10-pin-no-legs-cable-with-ribbon-connector) from Tag-Connect [or Digikey](https://www.digikey.com/en/products/detail/tag-connect-llc/TC2050-IDC-NL/2605367) and these helpful accessories [clip](https://www.tag-connect.com/product/tc2050-clip-3pack-retaining-clip) [adaptor](https://www.tag-connect.com/product/tc2050-arm2010-arm-20-pin-to-tc2050-adapter) and an SWD programmer like the SEGGER J-link).

The project has two parts: bootloaders in `u-boot-stm32mp-2020/` and application in `ctest/`

The bootloader must be built once, and loaded once onto an SD Card, which is inserted into the OSD board. 

The application ultimately needs to live on the SD Card as well, but it can be flashed into RAM using a J-Link flasher, making debugging much easier than having to copy files to an SD Card each time the code is changed.

You can easily include this repo as a submodule into your project, or just copy it in.

## 1) Setup:

Make sure to clone the submodule (4ms's u-boot fork):
```
git submodule update --init
```

On macOS, you may need to install gsed and set your PATH to use it instead of sed. See Caveats section in `brew info gnu-sed` for details.
```
brew install gnu-sed
export PATH="/usr/local/opt/gnu-sed/libexec/gnubin:$PATH"`
```

## 2) Build and load U-boot:

Build u-boot, putting the files in the build/ dir:
```
scripts/build.sh
```
Ignore warnings about "format string is not a string literal" (TODO: patch u-boot to fix this)

Verify the output files were created:
```
ls -l u-boot/build/u-boot-spl.stm32
ls -l u-boot/build/u-boot.img
```

Now you need to format and partition an SD Card.  Insert a card and do:
(TODO: Is this neccessary or does sgdisk -o wipe it?)
```
## Linux:
mkfs.fat32 /dev/sdX

## MacOS:
diskutil eraseDisk FAT32 BAREMETA /dev/disk#
```
Where /dev/sdX or /dev/disk# is the actual device name, such as /dev/sdc or /dev/disk3. 

///If you need to find out what the device is, you can type `ls -l /dev/sd` or `ls -l /dev/disk` and then hit Tab.
Or, on macOS you can type `mount` instead of `ls -l /dev/disk<TAB>`
Take note of what it lists. Then remove (or insert) the SD Card, and repeat the command. Whatever changed is the SD Card's device name(s). Use the base name, e.g. /dev/sdc, not /dev/sdc3.
///

You also could use the script `format-sdcard.sh`, though it's just a heavy wrapper around the above commands.


Then run the script to partition the drive 
```
scripts/partition-sdcard.sh /dev/XXX 
```
...where /dev/XXX is the SD Card device name such as /dev/sdc or /dev/disk2
This script will create four partitions, and format the fourth to FAT32.

Then run the script to copy the bootloader (u-boot and spl) to the first three partitions:
```
scripts/copy-bootloader.sh u-boot/build/
```
You may need to change the path `u-boot/build/` if the u-boot-spl.stm32 and u-boot.img files are somewhere else.

## 3) Power up the OSD board

This is a good moment to test your hardware setup. You can skip this step if you've done this before.
Remove the SD card from the computer and insert into the OSD board.
Attach a USB-to-UART device to the UART pins on the OSD board (use UART4 if you've got a custom board-- only TX/RX and GND are needed).
Start a terminal session that connects to the USB driver (I use minicom; there are many fine alternatives).

Insert the card into the OSD board and power it on. You should see boot messages, and then finally an error when it can't find
bare-arm.uimg. Now it's time to build that file. 

## 4) Build the application
```
cd ctest    # or the application directory if you're using this as a template in another app
make 
ls -l build/*.elf
ls -l build/*.uimg
```
You should see the elf and the uimg files. Each of these is the compiled application, and either one must be loaded
in SDRAM at 0xC2000040. You can load the elf file by using a debugger/programmer such as J-link connected to the SWD pins.
Or, you can copy uimg file to the SD Card in the fourth partition the same way you would copy any file. 
The former method is only temporary, requires a debugger to be attached, and will not persist after power down. However, it's much more convenient so it's preferred for Debug builds.
In the latter method, the application firmware is stored on the SD Card, so this is the method for production or long-term testing. With this method, the bootloader will load the application into 0xC2000040 on boot.

## 5) Copy application to SD card

If you've never loaded the app onto the SD Card, you have to do this before you can use a debugger on the application (step #6).
Or, if you want to have the application load even without a debugger attached, use this method.

There are two ways to do this:

The easiest way is just to physically insert the SD card into your computer. If the SD Card is still in the OSD board, power down the OSD board, remove the SD Card, and put it back into the computer.

The more convenient way (if it works) is to use the USB gadget function. If you have the UART connected and your particular OS happens to be compatible with usb-gadget, then you might be able to mount the SD Card directly over UART without having to physically touch the card. Do this:
1) When the board is booting, up look for the message `Hit any key to stop autoboot`, and press a key.
2) You will see a UBOOT> prompt. Type the command: `ums 0 mmc 0`
3) In another terminal window, look to see that there's a new device, e.g. /dev/sdX or /dev/disk#

Regardless of whether you physically move the card or use USB gadget, the next step is make sure the sd card is mounted and then copy the application uimg file to it, just like you would copy a normal file to a normal USB stick. You have to re-name the file on the sd card to be `bare-arm.uimg`. 

```
cp build/main.uimg /Volumes/MYAPP/bare-arm.uimg
```
 Of course, adjust the command above so that you're using the actual uimg file that your application build created (might be app.uimg or something), and the actual path to the mounted sd card.
Or you can use drag-and-drop within your OS's GUI, and then rename it.

If your OS didn't automatically mount the drive, do:
`sudo mount -o user /dev/sdX /tmp/sdcard_root`
or use some other path where you mount things. Then copy the file as above.

**Make sure the file is named `bare-arm.uimg` on the SD Card. The bootloader looks for a file with this exact name, in the root dir of a FAT32 filesystem on partition 4.**

There's also a handy script to automatically mount and copy and unmount the drive. 
```
scripts/copy-app-to-sdcard.sh build/myapp.uimg /dev/sdXX
```
Where `build/myapp.uimg` is the path to the app uimg file (perhaps ctest/bare-arm.uimg) and `/dev/sdXX` is the SD Card's fourth partition, e.g. `/dev/sdc4`.
The script will mount the SD Card partition, remove the old bare-arm.uimg file, and copy the one you provided onto the correct place. It's not a fool-proof script,
you may need to view the source and run commands manually if it's not working on your OS.

## 6) Debug application

This is completely optional, but is very convenient when developing. You must have a working bootloader and application uimg file, and the board must boot into the application.
Once that's established, you can use a J-link programer and Ozone to load a new application image and debug it. 

This README isn't a tutorial on using gdb or debuggers or SEGGER Ozone and J-link, so I won't go into detail here. The process is no different than debugging on any other STM32 device: you have an elf file and you use gdb (or Ozone) to load it.
If you use Ozone, create a new Ozone project for the STM32MP157C Core A7, and load the elf file created back in step 4. That's it.

Remember, any file you load using a debugger will only get loaded into RAM. As soon as you power down, it will be lost.

# Resources:

This guide is very helpful, although geared for a different platform:
http://umanovskis.se/files/arm-baremetal-ebook.pdf

