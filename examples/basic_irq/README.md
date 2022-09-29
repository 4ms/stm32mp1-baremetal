## Basic IRQ project

This project tests using the GIC on the Cortex-A7 to respond to an interrupt.

### Cortex-A GIC
When an interrupt happens, the core will immediately start executing code from
a specific place in the vector table. If you don't know about vector tables,
it's the same concept as in the Cortex-M, and there is lots of documentation
and tutorials online: [try this
guide](https://interrupt.memfault.com/blog/arm-cortex-m-exceptions-and-nvic).

Unlike the Cortex-M NVIC, we only have 6 entries in our vector table, and just one of them is dedicated to interrupts from hardware peripherals (UART, SPI, I2C, GPIO, SAI, etc.)

The vector table in our project has the assembly instruction `b IRQ_Handler` at
the IRQ handler location, which says to branch to the function named
`IRQ_Handler`. This function is defined in main.cc. 

The `IRQ_Handler` is marked with the interrupt("IRQ") attribute, which tells
the compiler to stash all registers that we use in the function, and then unstash
them before exiting. This ensures that the state of the registers is returned to 
the exact same state so that the interrupted code will not notice that it was
interrupted. If you want to know more about this, look up AAPC calling convention.
We also mark this function with the general_regs_only attribute, which tells
the compiler to not use any FPU registers. We also have to be careful to not call
any external functions that use FPU registers. The reason is that the interrupt("IRQ")
attribute tells the compiler to save and restore the non-FPU registers only. I
don't know why there is no attribute to make the compile save and restore ALL registers,
but there isn't, so we have to do it this way. Ensuring we don't clobber the FPU
registers is somewhat tedious if the IRQ Handler were to be more complex,
so we take a different approach in the nested_irq project.

The first thing the IRQ_Handler does is to tell the GIC that the IRQ is about
to be handled, by reading the GICDistributor register named Interrupt
Acknowledge Register. This also serves the purpose of telling us what the IRQ
number is. Each interrupt event has a unique IRQ number, and you can see the
list in the STM32MP157 Technical Reference Manual in the "Interrupt List"
section (Table 117 in TRM Rev 4). When compileing for the OSD32BRK board, we
use IRQ numbers 98 and 99, which are EXTI8 and EXTI9. These corresponds to a
pin-change interrupt for GPIO pin 8 (red LED) and GPIO pin 9 (green LED). When
compiling for the STM32MP1Disco board, we use IRQs 97 and 74, which are EXTI7
and EXTI11, corresponding to GPIO pin 7 (red LED) and pin 11 (green LED).

Once we have the IRQ number (`irqnum`), we record how many times each IRQ has
been called (red or green LED IRQ) and print a message saying which one was called.
We also make sure to clear the EXTI peripheral's interrupt flag, which tells the EXTI
peripheral that we handled the interrupt.

If you're used to using the NVIC on Cortex-M series, this all seems like a lot
of extra work. It is. The M series are more suited for "real-time", and
hardware-based IRQ response is part of why. However, the overhead of a software
NVIC on top of a GIC is somewhere in the ballpark of 200ns - 500ns for an ISR call
(depends on if you need FPU registers preserved), which includes branching and
the overhead of calling lambdas.  I've still found this quite useable for
real-time systems. 

For faster response, the GIC also has a FIQ (fast interrupt) which needs less
register preservation and one less branch instruction. So if you needed a fast
interrupt for a special purpose, you could define that handler in FIQ_Handler,
and use IRQ_Handler as a more flexible handler system.

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

Testing interrupts
Triggering ISR: Red LED 1 turning off...
5 4 3 2 1
>>> Entering red LED IRQ handler
<<< Exiting red LED IRQ handler
Triggering ISR: Green LED 1 turning off...
5 4 3 2 1
>>> Entering green LED IRQ handler
<<< Exiting green LED IRQ handler
Red LED ISR was called once, hooray!
Green LED ISR was called once, hooray!
```


