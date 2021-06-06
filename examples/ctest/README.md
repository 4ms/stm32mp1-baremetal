## CTest project

This project tests running C code on the Cortex-A7. C requires a few things like a stack and valid stack pointer in order to work. Our startup code has to provide this, or else C code will compile but will do weird things when it runs (or won't run at all).

#### MMU

The startup code calls SystemInit, which initializes the MMU by enabling the caches for the instruction and data memory. Without the MMU, the Cortex-A7 runs incredibly slow (comparable to a 20MHz ATTINY chip, except with half a gigabyte of RAM). 

Like the other projects in this repository, this is designed for the OSD32MP1 BRK board, but any STM32MP157-based board with header pins for UART4 exposed and maybe some LEDs to blink will suffice.

To use, first make sure u-boot is installed on an SD-card, as usual (see README in the project root directory). 

Then, in this directory run:

```
make
```

In the build/ dir, you should see the a7-main.uimg file. Copy it to the 4th partition of the SD-card, as usual (again, see README in the project root dir).

Reboot your board with a UART-to-USB cable connected, and watch u-boot's startup messages scroll by in a terminal.

Then you should see the UART output:
```
ABCXYZ
```

The ABC is output by the assembly code in startup.s.
The XYZ is output immediately in main(). So if you see that, C code is running!

Next, the LEDs will be initialized, and then you should see

```
Hello world from bare-metal!
```

Which means a function was called successfully.

And then you'll also see:

```
And hi to you too!
```

which means the function returned and also global constant data was copied correctly during startup.

And what embedded Hello World project would be complete without blinking lights?
So watch the four LED elements blinking... forever... it's so satisfying...

And also useful. The MMU I mentioned above must be setup correctly or else the lights will flash very slowly -- about every ten seconds. If everything is working, the LED animation sequence will be about half a second long.

#### References/Credit

Parts of the startup.s code is based-on/inspired-by the `cenv` project for a Cortex-A9 simulator, from [umanovskis/baremetal-arm](https://github.com/umanovskis/baremetal-arm/tree/master/src/04_cenv), which is Copyright (c) 2018 Daniels Umanovskis under the MIT license. Other parts of the startup.s code are copied directly from the [ARM Cortex-A Series Programmer's Guide v4](https://developer.arm.com/documentation/den0013/d), and some parts I wrote myself.

