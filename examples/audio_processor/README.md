## Audio Processor project

This project runs on the STM32MP157 Discovery board, using the board's codec
and mic/line jack. The OSD32BRK board does not have a Codec, so you would need
to connect the appropriate pins to a board with a codec for it to run (I've
done this and it works... you also need to change the button and LED pins to
match your board). See `audio_codec_conf.h` for the pinout. 

Three different effects (actually synths) can be selected by pressing the User1
button. Pressing the User2 button will print the current processor load
consumed by the current effect.

To use, first make sure U-Boot is installed on an SD card, as usual (see README
in the project root directory). 

Then, in this directory run:

```
make
```

In the `build/` dir, you should see the `a7-main.uimg` file. Copy it to the 4th
partition of the SD-card, as usual (again, see README in the project root
dir). Power up and play!

Press the buttons to change effect.  You'll see a message like:

```
Starting Audio Processor
Using Passthrough FX
Current load: 0%
Using DualFMOsc FX
Current load: 1%
Using Dual Osc FX
Current load: 0%
```

(I'm adding more effects that use more processor load, to showcase that!)

*Passthrough*: Just copies the inputs to the outputs. Unfortunately the Disco
board has a TRRS jack and I haven't yet found a mic that works well with it.
You may need to adjust the codec's biasing and boost settings to get it to
work. With a scope you can verify it's passing the input signal to the
output, it's just usually getting noise from the mic.

*DualFMOsc*: Ramp Osc frequency modulatates (FM's) a triangle oscillator which
is heard on the left output. The triangle oscillator FM's a sine oscillator,
which is heard on the right output.

*Dual Osc*: Left output has a 400Hz triangle wave. Right output has a 600Hz
sine wave.

#### lib/mdrivlib

`mdrivlib` is an eMbedded DRIVer LIBrary written by me. I intend to release it
fully after cleaning up a few more things, but for now it's here as part of
this project under the MIT license.  Here we're using a few portions of
mdrivlib:

  - `DebouncePin`: debounce a GPIO pin and provide high-level state polling
	like `is_just_pressed()`. Uses mdrivlib::Debouncer and mdrivlib::Toggler

  - `CodecCS42L51`: driver for the codec chip found on the STM32MP1 Discovery
	board. Uses mdrivlib::SaiTdmPeriph (multi-channel SAI DMA driver) and
	mdrivlib::I2CPeriph (basic wrapper over STM32-HAL's I2C driver). Also
	handles the DMA callbacks.

  - `CycleCounter`: cross-architecture processor cycle counter. The
	MP1-specific implementation file is compiled into this project, which uses
	the Cortex-A PL1 Physical Timer (see the CMSIS file `core_ca.h`). The timer
	runs at 24MHZ, so our resolution is something like 27 cycles. Good enough
	for measuring load percentage.

#### lib/util

`util` are some general-purpose utilities I've picked up over the years (not
specific to ARM or anything). Most have unit tests. I need a better name
before I can release it.

- `zip`: Iterate over two or more containers. No overhead: compiles to the same
  assembly that you get from the normal way `for (int i=0; i<BUFFER_SIZE; i++)
  { out[i] = in[i]...` Also has a cousin `countzip` which is like Python's
  `enumerate`

- `AudioFrame`: multi-channel audio frame struct. Has some nice conversion
  utilities (24=>32 bit sign extension)

- `oscs`: Simple phase accum ramp and triangle oscillators.

- `math`: various math stuff

- `math\_tables`: Trig and expo tables. Uses InterpArray, which provides a
  interpolation for both periodic and non-periodic tables.

### Program flow and potential optimizations 
There is some degree of indirection in the audio stream method used here,
in order to provide a flexible interface for setting and changing the audio
processing function on the fly. It could easily be made more efficient if your
application required it. Here's what happens in this project:

  1) The SAI DMA transfer/half-transfer complete interrupt is triggered, which calls the
	`IRQ_Handler` asm code in lib/mdrivlib/target/stm32mp1_ca7/drivers/interrupt_handler.cc
  
  2) The `IRQ_Handler` calls a lambda from `Interrupt::ISRs[]`. This lambda was
     created by SaiTdmPeriph 

  3) The SaiTdmPeriph lambda checks and clears the DMA flags and calls one of
  two callback functions, depending on if it was a DMA TX-Complete IRQ or DMA
  TX-Half-Complete IRQ

  4) This callback function (which is a lambda created by the AudioStream
  object) calls the function that you set using `set_process_function()`. It
  also measures the processor load that the processing function takes.
