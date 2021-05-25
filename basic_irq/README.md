## Basic IRQ project

This project tests using the GIC on the Cortex-A7 to respond to nested
interrupts. A nested interrupt happens when an interrupt interrupts an
interrupt. (Got that?) The interrupts should "nest" and after the second one
finishes being handles, it'll return to the first one. It's not a simple thing
to implement as it must be done in assembly (the C compiler cannot generate the
instructions needed to switch modes). And if not done properly everything will
seem to work except every once in a while a variable might change values
mysteriously at random times (ask me how I know).

When an interrupt happens, the core will immediately start executing code from
a specific place in the vector table. If you don't know about vector tables,
it's the same concept as in the Cortex-M, and there is lots of documentation
and tutorials online: [try this
guide](https://interrupt.memfault.com/blog/arm-cortex-m-exceptions-and-nvic).
The vector table in our project has a `b IRQ_Handler` at that location, and we
define an IRQ_Handler C function in interrupt.cc, which is really just a bunch
of asmembly code.

Like the other projects in this repository, this is designed for the OSD32MP1
BRK board, but any STM32MP157-based board with header pins for UART4 exposed
will suffice.

To use, first make sure u-boot is installed on an SD-card, as usual (see README
in the project root directory). 

Then, in this directory run:

```
make
```

In the build/ dir, you should see the a7-main.uimg file. Copy it to the 4th
partition of the SD-card, as usualy (again, see README in the project root
dir).

Reboot your board with a UART-to-USB cable connected, and watch u-boot's
startup messages scroll by in a terminal.

```
ABC

Testing nested interrupts
Make sure this is compiled with -O1 to test register clobbering (Makefile, OPTFLAG on line 41)
To see the floating-point register test fail, comment out the #define STASH_FPU_REGS in interrupt.cc
To see the integer register test fail, comment out the #define STASH_R5_R11_LR in interrupt.cc

You should see steps 1-6 occur in order:
1) Triggering outer ISR: Red LED 1 turning off (PI8 rising edge)
 2) Entering outer ISR (red LED 1)
  3) Triggering Inner ISR: Green LED 1 turning off (PI9 rising edge)
   4) Entering inner ISR (green LED 1)
    5) Inner ISR: both sums are OK. ...Exiting inner ISR
     6) Outer ISR: both sums are OK. ...Exiting ISR
Reached main context: both sums are OK
```

That's it! Notice the inner ISR starts and stops before the outer ISR returns.
This is the essential feature of nesting. The trick is making the IRQ Handler
be re-entrant: so it can be interrupted and ran again from the start multiple
times without causing any problems.



