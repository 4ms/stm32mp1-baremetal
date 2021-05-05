## CTest project

This project tests running C code on the Cortex-A7. C requires a few things like a stack and valid stack pointer in order to work. Our startup code has to provide this, or else C code will compile but will do weird things when it runs (or won't run at all).

This project is based-on/inspired-by the `cenv` project for a Cortex-A9 simulator, from [umanovskis/baremetal-arm](https://github.com/umanovskis/baremetal-arm/tree/master/src/04_cenv), which is Copyright (c) 2018 Daniels Umanovskis under the MIT license.

Like the other projects in this repository, this is designed for the OSD32MP1 BRK board, but any STM32MP157-based board with header pins for UART4 exposed and maybe some LEDs to blink will suffice.

To use, first make sure u-boot is installed on an SD-card, as usual (see README in the project root directory). 

Then, in this directory run:

```
make
```

In the build/ dir, you should see the bare-arm.uimg file. Copy it to the 4th partition of the SD-card, as usualy (again, see README in the project root dir).

Reboot your board with a UART-to-USB cable connected, and watch u-boot's startup messages scroll by in a terminal.

Then you should see the UART output:
```
ABCXYZ
```

The ABC is output by the assembly code in startup.s.
The XYZ is output immediately in main(). So if you see that, C code is running!

Next, the LEDs will be initialized and should all turn on. Both LEDs will look orange because that's what red + green looks like.

Next, you should see

```
Hello world from bare-metal!
```

Which means a function was called successfully.

And then for no real reason, you'll also see:

```
And hi to you too!
```

And the grand finale of every embedded Hello World project: Blinking Lights!
Yes, the four LED elements will blink... forever... it's so satisfying...

