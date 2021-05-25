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
`IRQ_Handler`. This function is defined in interrupt.cc.  The `IRQ_Handler`
function stashes the address we'll eventually return to (as well as other
registers and stack information that we'll need to restore the context that was
interrupted). It then tells the GIC that the IRQ is about to be handled, by
reading the GICDistributor register named Interrupt Acknowledge Register. This
also serves the purpose of telling us what the IRQ number is. Each interrupt
event has a unique IRQ number, and you can see the list in the STM32MP157
Technical Reference Manual in the "Interrupt List" section (Table 117 in TRM
Rev 4). We're using IRQ number 98 in this example, which is EXTI8. This
corresponds to a pin-change interrupt for GPIO pin 8. The pin we're changing to
cause an interrupt is the red LED (GPIO I, Pin 8).

Once we have the IRQ number, we call `ISRHandler` which is a wrapper over our
static C++ function InterruptManager::callISR(). The compiler will optimize
out this wrapper when compiled for a release, so it costs nothing.  The
callISR() function looks up a function object from a static ISRs[] array (using
the IRQ number as the index) and calls it.  When it returns, `IRQ_Handler`
restores the original context (registers, stack, mode flags) and jumps back to
the next line of code that was about to be executed before it got interrupted.
If you're used to using the NVIC on Cortex-M series, this all seems like a lot
of extra work. It is. The M series are more suited for "real-time", and
hardware-based IRQ response is part of why. However, the overhead of a software
NVIC on top of a GIC is somewhere in the ballpark of 200ns - 1us for an ISR call
(depends on if you need FPU registers preserved), which I've still found
useable even for "real-time" systems. Compared to a 180MHz M4 or M7, that's not unreasonable.

For faster response, the GIC also has a FIQ (fast interrupt) which needs less
register preservation and one less branch instruction. So if you needed a fast
interrupt for a special purpose, you could define that handler in FIQ_Handler,
and use IRQ_Handler as a more flexible handler system.

Previous to all this, we created a function object (a lambda) in main(), and
used InterruptManager::registerISR() to assign it to the proper IRQ number.
The use of lambdas as ISRs is not a typical approach in embedded systems (at
least as far as I've seen), but it's immensely useful. I've borrowed the code
for this from another project I work on, mdrivlib (see
lib/mdrivlib/interrupt.hh). Being able to change an ISR on the fly with minimal
performance penalty makes life easier. Local variables and member variables of
C++ objects can be captured in a lambda, so as long as you respect lifetimes,
you don't need to have static C-style functions as IRQ handlers.

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
>>> Entering IRQ handler
<<< Exiting IRQ handler
ISR was called once, hooray!
```


