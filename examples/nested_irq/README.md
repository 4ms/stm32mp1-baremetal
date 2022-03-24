## Nested IRQ project
This project tests using the GIC on the Cortex-A7 to respond to nested
interrupts. A nested interrupt happens when an interrupt interrupts an
interrupt. (Got that?) The interrupts should "nest", meaning that after the second one
finishes being handled, it'll return to the first one. It's not a simple thing
to implement as it must be done in assembly (the C compiler cannot generate the
instructions needed to switch modes, nor insure proper stack alignment to allow
SIMD usage in interrupts). And if not done properly everything will seem to
work... except every once in a while a variable might change values mysteriously
at random times, or you get DataAbort exceptions at odd times.

When an interrupt happens, the core will immediately start executing code from
a specific place in the vector table. If you don't know about vector tables,
it's the same concept as in the Cortex-M, and there is lots of documentation
and tutorials online: [try this
guide](https://interrupt.memfault.com/blog/arm-cortex-m-exceptions-and-nvic).
Cortex-M cores have large vector tables -- sometimes several hundred entries,
one for each possible interrupt. Cortex-A cores hAve much smaller vector
tables, just eight entries. One of these entries is the IRQ handler, which is
called by the processor whenever any interrupt is triggered (actually there is
another kind of interrupt called Fast or FIQ, but they behave the same as IRQ,
so we'll not discuss them now). The vector table in our project has `b
IRQ_Handler` at the entry for the IRQ handler, and we define the `IRQ_Handler`
function in `shared/drivers/irq_handler.cc`. This function is written in
assembly and is basically a dispatcher that gets called on every interrupt. It
stores the current state (registers and status flags) and makes sure the stack
is aligned properly. boundaries, reads the IRQ number, and jumps to the
appropriate interrupt handler. When the interrupt handler returns, it tells the
GIC we're done handling the interrupt, and restores the stack alignment,
registers, and status flags.

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
Make sure this is compiled with -O1 to test register clobbering (Makefile line 4, OPTFLAG)

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
This is the essential feature of nesting. The trick is that `IRQ_Handler`
disables and enabled interrupts so that it can only be interrupted during the
call the ISRHandler (and whatever function ISRHandler calls). `ISRHandler` is a
normal function, so the compiler will automatically make sure that registers
r4-r11 and r13 (stack pointer) are restored back to their original values when
the ISRHandler returns to `IRQ_Handler`. Therefore `IRQ_Handler` only needs to
stash r0-r3, r12, and lr, and the FPU registers (in addition to the address to
return to and the orignal status flags the moment before we were interrupted).

A note on stack alignment:
The ARM standard requires that the stack is aligned to 8-byte boundaries
whenever a function starts or stops. There aren't any alignment requirements in
the middle of a function, which is likely where we'll be when an interrupt is
triggered. Since our interrupt handler is a function, we must check if the
stack is aligned to 8-byte boundaries and fix it if it isn't aligned. Then,
when the handler exits, we must restore the stack pointer exactly as it was
before, so that the function that was interrupted acts the same as if it wasn't
interrupted.
What happens if we don't align the stack? Well, one thing that can happen is
load/store instructions that operate on large chunks of data (8-bytes or more)
can crash. Instructions like `VLD1.64` requires that the source location is an
8-byte aligned address, which is often the stack pointer. This means our
interrupt handler will produce a DataAbort, but only when it happened to get
called when the stack was not on an 8-byte boundary.
