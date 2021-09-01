## Multicore SMP

This project demonstrates using both A7 cores. Both cores are capable of
executing simultaneously at 650MHz or 800MHz (depending on the chip), achieving
true parallel processing (not time-sharing as is used in a single-core RTOS or
OS). In this project we harness this massive potential to blink two lights ;)

Since both cores are nearly identical, this is called Symmetric Multiprocessing
or SMP (as opposed to the usage of the M4 core, which is called Asymmetric
Multiprocessing or AMP).  SMP is typically used by an OS running on a main core
to assign another core(s) to run a task and then return. That level of control
will be demonstrated in a different example project. Here, we just have two
main() functions: `main()` for the 'main' core, which is MPU0 or Core0.
`aux_core_main()` for the `aux` core, which is MPU1 or Core1.

When the STM32MP157 starts up, both A7 cores runs code in a section of memory
called BOOTROM which is at address 0x00000000. This is ROM, and thus not
modifyable.  ST is somewhat opaque about what exactly this code does, but there
is a high-level description on their [wiki](From
https://wiki.st.com/stm32mpu/wiki/STM32MP15_ROM_code_overview#Secondary_core_boot)
Many things are omitted from this wiki, so I've downloaded and disassembled the
ROM code manually to figure out what's happening with the Core1 at startup.

You can follow along if you like reading assembly, I'll put the addresses in [ ].

[0000] Both cores clear their registers, and then part ways [007C]. Let's follow Core1:

[0210] Core1 sets up some temporary stacks for each processing mode BOOTROM
uses: Abort, IRQ, Undefined, Supervisor, and Monitor.

[0264-02DC] Then it sets up the vector table address, which is used for responding to
execeptions (though I haven't seen where it uses these besides IRQ).  It
enables and disables various caches, disables the MMU, does a few other
maintenance things. It repeats this for both secure and non-secure modes.

[04A0] It also sets up the GIC (interrupt controller) to enable IRQ0, and gives it
priority of 0 (highest possible). This because the next thing it does is [04D0] to go
to sleep with a WFI instruction (wait for interrupt). In order to wake up,
Core0 has to send Core1 a Software Generated Interrupt (SGI).

But this isn't the wakeup call we perform in order to start Core1. This wakeup
call is done by U-boot (I think -- it may be done by Core0 while still in
BOOTROM. I'll update this if I discover more). Anyways, it soon wakes up,
[A25C] acknowledges and [A308] ends the IRQ.

[04E0] Then it goes back to sleep if it wasn't IRQ0.  If it was IRQ0, it [5fe4]
proceeds to [333a] check and clear some RCC flags registers and then [600E]
reads backup register 4 in the TAMP peripheral for a [6012] magic number
(0xCA7FACE0). I don't understand
under what circumstances this particular magic number is used, I suspect this
code is shared between Core0 and Core1.  In any case, Core1 doesn't ever get
this magic number, but it continues along and [6028] clears that backup
register to 0. It also [6032] clears backup register 5 to 0, and this register
is used for telling a just-woken-up core where to go (more on that later). As
an aside, the clearing of register 4 is used by the PSCI feature in U-boot to
send a signal to Core0 that Core1 has indeed woken from it's first sleep.

Now that Core1 has finished the first sleep, it proceeds to go to sleep for a
second time.  But first it [6040] loads another magic number, 0xCA7FACE1 into
register R6. It keeps it there and [060C] passes out. 

Now we've reached the part where our code comes in. While Core1 sleeps, Core0
can take its time to initialize the application, and do whatever it needs to be
done by a single core. In our example app, we just setup the LED GPIOs and send
a message out the UART. In a larger system, you might initialize external
chips, setup data structures, etc etc.

When Core0 is ready to use Core1's computing power, it has to first fill two
registers. One of these is a magic number. Yes, the very same magic number that
Core1 stored in R6 right before falling asleep (0xCA7FACE1). Core0 has to write
this to TAMP backup register 4.  Also, when Core1 awakens, it will need to know
what to do, that is, what instructions should it start executing?  Core0 must
write an address into TAMP backup register 5, and this address is called the
Branch Address. Shortly after waking, Core1 will jump to the Branch Address and
start executing whatever code is there.

You can see in `secondary_core.hh` we have a function called `start()`.  This
enables the SGI, and then makes sure that TAMP's backup protection is cleared.
Then it writes the address of the `aux_core_start` into the
BranchAddressRegister. The `aux_core_start` function is the aux core's entry
point into our app: it's written in assembly in startup.s. It just initializes
the core's caches and MMU, sets up the stack, and then jumps to our
`aux_core_main` function (which is written in C++ in main.cc).

Once the magic number and branch address are written, we can actually start
Core1. Moment of truth!  We just need to send an SGI to Core1 from Core0. As
soon as that's done, Core1 wakes up.  It reads some flags in the Boot Security
registers (BSEC), which I presume is some sort of security clearance procedure
that lets you disable booting the secondardy core by writing into some OTP
registers (One-Time-Programmable). Anyways, unless you took active measures to
prevent booting, these tests will pass.  The core now proceeds to read from
TAMP register 4 and checks for the magic number 0xCA7FACE1. If it reads that,
then it reads TAMP register 5 and branches to whatever address is in that
register. (Literally any address, even a mis-aligned number or a totally bogus
address! It's Core0's responsibility to make sure this is a proper place to
jump to).

### Building

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
Core0: Flashing an LED is hard work! I'm going to wake up Core1 to help.
Core0: I'll handle flashing LED1, and tell Core1 to flash LED2...
Core0: Starting Core1
Core1: Good morning, Core0! I'm happy to help, I can flash LED2 twice as fast as LED1!
```


