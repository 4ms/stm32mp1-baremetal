## Minimal Build project

All this project does is print an 'A' to the UART, and then hang forever. 

Why do we want this? A simple 'hello world' project like this is useful for debugging and as a starting place to verify u-boot is loading your project. 

Like the other projects in this repository, this is designed for the OSD32MP1 BRK board, but any STM32MP157-based board with header pins for UART4 exposed will suffice.

To use, first make sure u-boot is installed on an SD-card, as usual (see README in the project root directory). 

Then, in this directory run:

```
make
```

In the build/ dir, you should see the a7-main.uimg file. Copy it to the 4th partition of the SD-card, as usualy (again, see README in the project root dir).

Reboot your board with a UART-to-USB cable connected, and watch u-boot's startup messages scroll by in a terminal. Then... wait for it...

```
A
```

ITS ALIVE!!!!

