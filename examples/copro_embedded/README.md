## Copro (Embedded technique)

This project demonstrates how the Cortex-M4 core of the MP1 chip (aka "coprocessor" or "MCU") can be used to run in parallel with the A7 core.

Only resetting and starting the M4 core, and loading of firmware is demonstrated, inter-core communication will be a in a different project.

The technique used here is embed the M4's firmware binary into the A7 core's
firmware. The Makefile handles this by building M4 firmware into an .elf file,
then splitting it up into separate binarty files, and then re-combining those
binaries wherever there's a contigious block of memory. Finally, the binaries
are converted to normal C arrays that live in header files. The A7 firmware
\#includes these header files and copies the data in the arrays to the proper
place in memory (0x38000000 for the vector table, and 0x10000000 for the code).
Then, the A7 starts the M4 processor.  

This technique has pros and cons:

Pros:

  - The entire appication firmware lives in one file which can be read by u-boot.

  - Debugging is easy because you have complete control over the M4 core (stop/start/reset).

  - Reloading new code is fast and easy because one file contains everything.

Cons:

  - Somewhat round-about way of combining firmware and must be manually updated
	if M4's section names or memory layout changes.

  - The entire M4 firmware must live in the A7's memory as well as the M4's
	memory (in SRAM1, mostly). So some data is duplicated, and memory is
	wasted. But, even complex M4 firmware probably won't exceed 128kB, so it's
	a drop in the bucket considering we have 512MB available. And, you could in
	theory re-use the memory space once the firmware has been copied.


#### Running the demo

Like the other projects in this repository, this is designed for the OSD32MP1 BRK board, but any STM32MP157-based board with header pins for UART4 exposed will suffice.

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

