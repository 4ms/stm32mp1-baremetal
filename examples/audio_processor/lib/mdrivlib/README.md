# mdrivlib

## C++17/20 Embedded driver library

This is a work in progress, the API is in flux. Contributions, feedback, and issues are welcome.

### Devices supported:

*Cortex M7*

  * STM32H745/55/47/57 (dual-core M7/M4): full support, except built-in ADC
  * STM32F7xx (especially STM32F746, F76x): partial support

*Cortex A7*

  * STM32MP157 (dual-core A7 + M4): partial support, but adding features daily.

## Goals:

  * Easy to use, difficult to misuse.
  * Provide modern C++20 classes for setting up and accessing internal peripherals and common external chips.
  * Provide high-level functionality for common use cases in embedded projects (debouncing switches, wear-leveling, RGB color blending, filtering ADC readings).
  * Allow use of lambdas as callbacks (e.g. interrupt request handlers) with minimal overhead
  * Does not depend on dynamic memory (but provides some structures if you use it)
  * Bare-metal: does not depend on a RTOS
  * Priortize peripheral access efficiency over one-time cost of peripheral setup.
  * Unit tests (using doctest)
  * Example code (TODO: needs work!)
  * Emphasis on higher-end microcontrollers and microprocessors

## Status of Drivers:


#### STM32 Internal Peripheral Drivers:

| Peripheral | HAL      | Config\*          | Notes |
|:-----------|:---------|:----------------|:------|
| ADC        | STM32-LL | template params | Circular DMA mode. F7/H7 only. _Todo: split off target-specific code. Needs modernizing_ |
| BDMA       | CMSIS    | template struct | Most configurations supported (H7 only, not present on F7 or MP1) |
| DMA2D      | CMSIS    |  -              | Supports FillRect with Rgb565 on H7, no other modes yet. (Not present on F7 or MP1) |
| I2C        | STM32-HAL| config struct   | Blocking, IT, DMA modes |
| Interrupt  | Cortex   | -               | Assign a lambda or any callable as any IRQ Handler at run-time. Supports NVIC (Cortex-M) and GIC (Cortex-A) |
| MDMA       | CMSIS    | template struct | Supports mem-to-mem only, with byte/word-swapping. |
| MPU        | CMSIS    |  -           	  | Only supports disabling Data Cache on a given memory region. MPU is only present on Cortex-M7 |
| GPIO (Pin) | STM32-LL | init/ctor params| Supports all GPIO functions (except Locking?). Run-time values. |
| GPIO (FPin)| CMSIS    | template params | "Fast Pin". Constexpr-configured, maximum efficiency. Supports high(), low(), read(). |
| EXTI (Pin change interrupt) | CMSIS | template params | Assign ISR callback for rising and/or falling edge for any pin. _Todo: Finish migration from config struct to template params_|
| HSEM (Semaphores) | CMSIS |  | Safer wrappers over raw register access for HSEM (hardware semaphore). Supports managing multiple channel interrupt callbacks. Only for dual-core devices (H7 and MP1) |
| QSPI       | STM32-HAL| config struct   | Supports blocking and IT modes. Some chip-specific setup may be required in `qspi_flash_registers.h` file. Todo: use mdrivlib::Interrupt class instead of HAL Callbacks|
| RCC        | CMSIS    |  -    	 	  | Safer wrapper over raw register access to enable/disable/reset peripherals |
| SAI        | STM32-HAL | config struct  | For streaming audio. DMA mode only. Supports RX- or TX-master. Caller provides tx/rx buffers and callbacks (where audio processing is done) |
| SDRAM      | STM32-HAL | config struct | Setup/init FMC for SDRAM. Single bank supported only. Includes self-test. H7 only (MP1 SDRAM is init by U-boot) |
| SPI (direct peripheral) | CMSIS     | template struct | SpiPeriph: efficient wrapper over SPI registers (Currently onle for ST's SPI v2.x.x, found in H7 and MP1). Supports multi-chip buses (CS0 - CS3) |
| SPI (transfer) | CMSIS       | template struct | SpiTransfer: wraps SpiPeriph. Supports blocking, polling, FIFO, fixed-size transmissions in IT mode, multi-chip buses. |
| TIM        | STM32-LL | init/ctor params | Simple class to initialize a TIM peripheral. Can skip initialization if periph has already been init. |
| UART       | STM32-HAL | -    | WIP |

#### External Chip Drivers:

| Peripheral          | Config\*          | Notes |
|:--------------------|:----------------|:------|
| ADC, I2C (MAX11645) | config struct   | Blocking and IT mode |
| ADC, SPI (MAX11666) | template struct | Heavily optimized. Takes FilterType template param |
| Codec, I2C/I2S (WM8731) | config struct | Virtual class, ties I2C init with SAI+DMA init. Todo: make template class|
| DAC, SPI (MCP48FVBxx)| template struct| Supports streaming and blocking modes |
| GPIO Expander, I2C (TCA9535) | config struct | Supports inputs only (so far), I2C in IT mode |
| LED Driver, I2C (PCA9685) | ctor/init params | Uses STM32-HAL, heavily (HAL callbacks). Supports DMA, IT, and blocking modes. Supports mono and RGB LEDs. Works well with Frame Buffer LED class. Needs modernizing |
| Screen, SPI/DMA (ST77XX) | template struct | Use DMA to dump a framebuffer to SPI. Wraps SpiPeriph and a DMA driver (which is passed in as part of the config), and calls a callback when done. |


#### High-level helpers (target-agnostic):

| Peripheral   | Config\*          | Notes |
|:-------------|:----------------|:------|
| AnalogIn     | template params | Generic class to combine multi-channel ADC sources with post-filtering. WIP |
| Clocks       |     -           | enable/disable/reset peripheral clocks |
| CycleCounter | -               | Measures cycles spent between a start and stop point |
| DAC Stream   | template struct | Allows circular buffer to be filled in bursts, and output to DAC a steady rate |
| GPIO Stream  | template struct | Same as DAC Stream, but for a GPIO Pin output |
| Frame Buffer LED| -             | Allows random access to a contigious frame buffer, used with LED DMA or IT drivers |
| Peripheral Utils | -            | Utilities for peripherals, such as finding the maximum frequency of a timer. WIP: many more helper functions to be added |
| Pin debouncing | template params |  Basic debouncing of physical pins (for buttons and other inputs). Requires `util` lib
| PWM analog output | ctor params | Sets up, starts, adjusts, and stops PWM output on pins with TIM OC functionality. (F7 only, H7 and MP1 may work but not tested) |
| QSPI Flash Cell Block | template params | Store and recall any data struct in a block or sector or FLASH memory, 
| RGB LED      | -              | Supports color blending 24-bit color, floats, RGB565 mode, flashing and fading animations. Supports different LED element types (e.g. Red and Blue are PWM-controlled, but Green is GPIO-controlled)
| Rotary Encoder | ctor params  | Supports half-step and full-step rotaries. Accumulates net position until read. |
| Timekeeper | config struct | Sets up a timer to call a given callback at regular intervals (lambda or any callable object).|

\*See 'Configuring a Driver' section for description of Config types.

#### Boot/Startup/Low Level:

| File       | Notes     |
|:-----------|:----------|
| `startup_*.s` | Assembly for Cortex startup (vector table, Reset handler, libc/cpp and static object initialization, calls SystemInit and main |
| `*.ld` | Linker script for memory layout. You may want to customize this for advanced applications (allocate more or less memory for DMA buffers, for example). |
| `cache.hh` | Wrappers for target-specific cache maintanance |
| `system_startup.cc/hh` | C++ startup code for F7 and H7 (M4 and M7 cores): managing multiple cores at startup, loading RCC clock configuration |


## Usage

Add the following directories as INCLUDE directories for your build system:
(e.g. add `-Ipath` to your compile commands for each of the following paths):

  * `mdrivlib/drivers` 
  * `mdrivlib/util`
  * `mdrivlib/target/XXX/drivers/`, where XXX is the target architecture (look in the target dir to see what the options are)
  * CMSIS device header: if you've downloaded STM32Cube, then include the path something like this (for H7xx): `STM32Cube_FW_H7_V1.8.0/Drivers/CMSIS/Device/ST/STM32H7xx/Include/` 
  * If you're using any drivers that require STM32-HAL or STM32-LL: STM32 HAL Include directory, e.g. `STM32Cube_FW_H7_V1/Drivers/STM32H7xx_HAL_Driver/Inc`

Compile and link any `mdrivlib/drivers/*.cc` and `mdrivlib/target/XXX/drivers/*.cc` that you wish to use.



Build with the proper symbols defined for your target: (TODO: specify these clearly in one place)
example: `-DSTM32H7x5 -DCORE_CM7`

Use it:

```
#include "pin.hh"
#include "timekeeper.hh"

//Values for an STM32H745 Discovery board. Adjust these values to suit your chip/board:
Pin userLED {GPIO::J, 2, PinMode::Output};
Pin pwmPin {GPIO::A, 6, PinMode::Alt, LL_GPIO_AF2};
TimPwmChannel myAnalogOut{{.TIMx = TIM3, .channel = 1, .period = 10000}};

userLED.high();
myAnalogOut.start_output();
uint8_t ramp_up = 0;

Timekeeper lightFlasher({.TIMx=TIM2, .period=100000000/*ns*/}, [&ramp_up](){
	myRedLED.toggle(); 
    myAnalogOut.set(ramp_up);
});
lightFlasher.start();


```



## Common themes throughout drivers:


### STM32-HAL vs STM32-LL vs CMSIS

mdrivlib contains a mix of three methods of hardware access:

   - STM32-HAL (`stm32xxxxx_XXX_hal.c/.h` files): The STM32 HAL library is used only when the peripheral is complex enough that re-writing it would be non-trivial **and** would offer very little benefit in terms of efficiency, code space, or additional features.
 
   - STM32-LL (`stm32xxxxx_XXX_ll.c/.h` files): In some drivers, ST's LL drivers are used, which are thin wrappers over direct register control via CMSIS headers. I am mostly migrating the drivers that use LL to use CMSIS since the LL drivers aren't as portable as I originally believed.
 
   - CMSIS (`stm32xxxxx.h` files): These are huge (2MB+) header files provided by the vendor (ST) that map all the registers to C-style structs and macros. The benefit of these is that the datasheet (Reference Manual) can be used along-side to setup and debug peripherals. Many drivers in mdrivlib use `RegisterBits` and `RegisterSection` which wrap the CMSIS macros. These can be specified as ReadWrite, ReadOnly, or WriteOnly, making this a bit safer to use than raw CMSIS register macros without sacrificing any efficiency.



### Configuring a driver

The tables above mentions different types of configuration:

  * **Template params**: e.g. 
  
  ```
  AdcChan<AdcPeriphNum::_1, AdcChanNum::_13, uint16_t> myADC;
  ```
  
  * **Template struct**: wraps multiple params into a struct passed as a template argument:
  
  ```
  struct MySpiAdcConfig : DefaultSpiConfig { 
      static constexpr uint16_t NumChips = 2;
      //...
  };
  
  AdcSpi_MAX11666<MySpiAdcConfig> myADC;
  ```
  
  * **Config struct**: wraps multiple params into a struct passed as a constructor argument:

  ```
  I2CPeriph MyI2C { 
      .I2Cx = I2C3, 
	   .SCL = {GPIO::B, 8, LL_GPIO_AF4},
	   .SDA = {GPIO::B, 9, LL_GPIO_AF4},
	   // ...
  };

  MyI2C.write(deviceAddr, data, dataSize);
  ```

  * **Ctor/init params**: normal function parameters passed to the constructor or an init function. e.g.:

  ```
  	TIM_TypeDef *const TIMx = TIM2;
	TimChannelNum const channelNum = TimChannelNum::2;
	uint32_t period = 256;
	uint16_t prescaler = 1;
	uint32_t clock_division = 1;

	TimPwmChannel myAnalogOut{TIMx, channelNum, period, prescaler, clock_division};
	myAnalogOut.start_output();
	for (int i = 0; i<256; i++) 
		myAnalogOut.set(i);
  ```
