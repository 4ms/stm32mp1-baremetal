# STM32MP1 Cortex-A7 baremetal example projects

  * **Minimal Boot**: Hello World project to prove U-boot and your hardware is working.
  * **Ctest**: Demonstrates the startup code for a C/C++ project: setting up the stack, initializing globals, etc.
  * **Basic IRQ**: Basic interrupt handling with the A7's Generic Interrupt Controller.
  * **Nested IRQ**: More sophisticed interrupt handling: interrupts interrupting interrupts! (and using lambdas as handlers!)
  * **Multicore_a7**: Demonstrates running both A7 cores in parallel.
  * **Copro_rproc**: Using the rproc feature of U-boot to load and run firmware on the M4 core in parallel with the A7 core.
  * **Copro_embedded**: Embedding the M4 firmware binary into the A7's firmware binary, and loading it on demand. Wacky, but cool.
  * **STM32Cube HAL**: [TODO] demonstrates using ST's HAL.
  * **Audio Processor**: [TODO] not a simple example, but a real-world practical project that uses all of the above features to create an audio effect.

See the README in the parent directory for more information.
