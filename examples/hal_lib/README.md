## HAL_Libs project

This project adds to the CTest Project HAL Libraries and some system calls that allow printf to be used. The example initializes UART4 and some GPIOs, prints some text, then counts up using HAL_SYSTICK, and toggles the LEDs. 

#### MMU

The startup code calls SystemInit, which initializes the MMU by enabling the caches for the instruction and data memory. Without the MMU, the Cortex-A7 runs incredibly slow (comparable to a 20MHz ATTINY chip, except with half a gigabyte of RAM). 

This project was designed using a MYIR STM32MP1 board, and works without modification on the STM32MP1 Discovery board. Any STM32MP157-based board with header pins for UART4 exposed and maybe some LEDs to blink will suffice. LED port and pins are defined in main.h. 

To use, first make sure u-boot is installed on an SD-card, as usual (see README in the project root directory). 

Then, in this directory run:

```
make
```

In the build/ dir, you should see the a7-main.uimg file. Copy it to the 4th partition of the SD-card, as usual (again, see README in the project root dir).

Reboot your board with a UART-to-USB cable connected, and watch u-boot's startup messages scroll by in a terminal.

Then you should see the UART output:
```
ABC
```

The ABC is output by the assembly code in startup.s.

Next, the LEDs will toggle and you should see

```
Hello from HAL Driver!
And hi to you too!
0
1
2
```

Which means UART and GPIO has been successfuly initialized by the HAL Drivers. 

#### References/Credit

Parts of the startup.s code is based-on/inspired-by the `cenv` project for a Cortex-A9 simulator, from [umanovskis/baremetal-arm](https://github.com/umanovskis/baremetal-arm/tree/master/src/04_cenv), which is Copyright (c) 2018 Daniels Umanovskis under the MIT license. Other parts of the startup.s code are copied directly from the [ARM Cortex-A Series Programmer's Guide v4](https://developer.arm.com/documentation/den0013/d), and some parts I wrote myself.

