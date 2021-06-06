## Copro (rproc technique)

This project demonstrates how the Cortex-M4 core of the MP1 chip (aka "coprocessor" or "MCU") can be used to run in parallel with the A7 core.

Only resetting and starting the M4 core, and loading of firmware is demonstrated, inter-core communication will be a in a different project.

The technique used here is to build each core's firmware separately, generating an .elf file for the M4, and a .uimg file for the A7.
Then, you manually copy these two files onto the 4th partition of the SD Card (if that doesn't make any sense, read the root-level README!).
Using a debugger/flasher to copy these files doesn't work very well (I would love to figure out why, but I haven't had consistant success).

Then, you use u-boot's rproc feature to load the M4 firmware. The A7 firmware can start the firmware when it's ready.

This technique has pros and cons:

Pros:

  - Very simple build process.

  - Firmware files are separate, can be swapped-out or re-compiled separately.

Cons:

  - Must copy firmware files to SD Card each time they change.

  - Debugging startup of either core is difficult


#### Running the demo


Like the other projects in this repository, this is designed for the OSD32MP1 BRK board, but any STM32MP157-based board with header pins for UART4 exposed will suffice.

To use, first make sure u-boot is installed on an SD-card, as usual (see README in the project root directory). 

Then, in this directory run:

```
make
```

In the build/ dir, you should see two directories: corea7/ and corem4/. Inside corea7/ is the a7-main.uimg file. Copy it to the 4th partition of the SD-card, as usual.
Inside corem4/ is the main.elf file, which you should also copy the the 4th partition of the SD-card.

Reboot your board with a UART-to-USB cable connected, and watch u-boot's startup messages scroll by in a terminal.

Press a key when u-boot says "Hit any key to stop autoboot".

At the UBOOT> prompt, enter this:

```
UBOOT> rproc init
UBOOT> fatload mmc 0:4 0xC2000000 main.elf
UBOOT> rproc load 0 0xC2000000 ${filesize}
UBOOT> run bootcmd
```

This will initialize the rproc system, load the m4 .elf file from the SD-card into RAM, and then load it into the proper places in memory.
The final command `run bootcmd` just tells U-boot to resume booting up.

Of course, this is very tedious to do with every reboot. You can add these commands to u-boot's boot command by adding them to the text at the bottom of `u-boot/include/config_distro_bootcmd.h`
(search for `bootcmd_baremetal_arm`). Change these lines:

```
"bootcmd_baremetal_arm="                   \
"mmc dev 0;"                               \
"fatload mmc 0:4 0xC2000040 a7-main.uimg;" \
"bootm 0xC2000040;"                        \
```

To this:

```
"bootcmd_baremetal_arm="                    \
"mmc dev 0;"                                \
"rproc init;"                               \
"fatload mmc 0:4 0xC2000000 main.elf;"      \
"rproc load 0 0xC2000000 ${filesize};"      \
"fatload mmc 0:4 0xC2000040 a7-main.uimg;"  \
"bootm 0xC2000040;"                         \ 
```

Then re-compile and re-install u-boot.

