# STM32MP1 Cortex-A7 baremetal example projects

  * **[Minimal Boot](https:github.com/4ms/stm32mp1-baremetal/tree/master/examples/minimal_boot)**: Hello World project to prove the bootloader and your hardware is working.
  * **[Ctest](https:github.com/4ms/stm32mp1-baremetal/tree/master/examples/ctest)**: Demonstrates the startup code for a C/C++ project: setting up the stack, initializing globals, etc.
  * **[Basic IRQ](https:github.com/4ms/stm32mp1-baremetal/tree/master/examples/basic_irq)**: Basic interrupt handling with the A7's Generic Interrupt Controller.
  * **[Nested IRQ](https:github.com/4ms/stm32mp1-baremetal/tree/master/examples/nested_irq)**: More sophisticed interrupt handling: interrupts interrupting interrupts! (and using lambdas as handlers!)
  * **[Multicore_a7](https:github.com/4ms/stm32mp1-baremetal/tree/master/examples/multicore_a7)**: Demonstrates running both A7 cores in parallel.
  * **[Copro_rproc](https:github.com/4ms/stm32mp1-baremetal/tree/master/examples/copro_rproc)**: Using the rproc feature of U-Boot to load and run firmware on the M4 core in parallel with the A7 core.
  * **[Copro_embedded](https:github.com/4ms/stm32mp1-baremetal/tree/master/examples/copro_embedded)**: Embedding the M4 firmware binary into the A7's firmware binary, and loading it on demand. Wacky, but cool.
  * **[[Audio Processor]](https:github.com/4ms/stm32mp1-baremetal/tree/master/examples/audio_processor)**: A fun practical project that lets you select one of
	several audio synths to play. Requires STM32MP1 Discovery board. Uses
	STM32-HAL, some DaisySP example projects, and some Faust algorithms. TODO: use multi-core A7.
  * **[USB MSC Device](https://github.com/4ms/stm32mp1-baremetal/tree/master/examples/usb_msc_device)**: Simple example that creates a USB Mass Storage Class device (aka "USB thumb drive").

See the README in the parent directory for more information.
