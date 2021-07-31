
These are structs found in files named `*_config_struct.hh`. They let you define board-specific things like the pins to use, peripheral number, DMA settings, etc. Your project should pass them to the constructor of the driver (either as a template parameter or constructor parameter).
  
There are currently two types of structs: I'm calling them `const` and `constexpr`. Both are simple to use.
I'm moving towards using more and more constexpr structs because it allows for compile-time values to be used in the drivers (hence less RAM lookups and better performance). However many early drivers perform just fine and use the const style.

### const struct configuration:

For an example, see the I2C config struct in `drivers/i2c_config_struct.hh`:

```
struct I2CConfig {
	I2C_TypeDef *I2Cx;
	PinNoInit SCL;
	PinNoInit SDA;
	// etc..
};
```

To use the I2C peripheral driver, you can specify the struct values inline in the driver constructor:

```
mdrivlib::I2CPeriph MyI2C { 
	.I2Cx = I2C3, 
	.SCL = {GPIO::B, 8, LL_GPIO_AF4},
	.SDA = {GPIO::B, 9, LL_GPIO_AF4},
	// etc..
};

MyI2C.write(deviceAddr, data, dataSize);

```

Or you can define all your conf structs in one or more header files like this:

```
//.hh file:
const mdrivlib::I2CConfig MyI2Cconf {
	.I2Cx = I2C3,
	.SCL = {GPIO::B, 8, LL_GPIO_AF4},
	.SDA = {GPIO::B, 9, LL_GPIO_AF4},
	// etc..
};

//.cc file:
mdrivlib::I2CPeriph MyI2C {MyI2Cconf};
MyI2C.write(deviceAddr, data, dataSize);
```

### constexpr struct configuration: ###

For an example, see the SPI config struct in `drivers/spi_config_struct.hh`

```
struct DefaultSpiConf {
	static constexpr uint16_t PeriphNum = 1; // 1 ==> SPI1, 2 ==> SPI2, ...
	static constexpr uint16_t NumChips = 1;
	static constexpr bool is_controller = true; // aka "master"
	static constexpr IRQType IRQn = SPI1_IRQn;
	static constexpr uint16_t priority1 = 3;
	static constexpr uint16_t priority2 = 3;
	static constexpr PinNoInit SCLK = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit COPI = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit CIPO = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit CS0 = {GPIO::A, 0, LL_GPIO_AF_0};
	//...
};
```

To use these types of structs, create your own struct type that derives from the DefaultSpiConf type, and override whatever values you don't want to be default:

```
using namespace mdrivlib;

struct MyDACConf : DefaultSpiConf {
	static constexpr uint16_t PeriphNum = 2; // SPI2
	static constexpr IRQType IRQn = SPI2_IRQn;
	static constexpr PinNoInit SCLK = {GPIO::A, 9, LL_GPIO_AF_5};
	static constexpr PinNoInit COPI = {GPIO::B, 15, LL_GPIO_AF_5};
	static constexpr PinNoInit CIPO = {GPIO::A, 0};
	static constexpr PinNoInit CS0 = {GPIO::B, 9, 0};
	static constexpr PinNoInit CS1 = {GPIO::B, 8, 0};
	static constexpr uint16_t clock_division = 1;
	//...
};


DacSpi_MCP48FVBxx<MM_DACConf> MyDAC;
MyDAC.init();
```

These `constexpr` structs are used when configuration values are required during the peripheral's normal operation (as opposed to just during the peripheral's setup), and using values stored in RAM effects performance. For instance, a high-speed SPI DAC driver with multiple chips on a single SPI bus requires manually toggling the Chip Select pins in the SPI interrupt, because the STM32 SPI peripheral does not support multiple CS lines. Using a constexpr struct allows for the CS pins' GPIO register values to be hard-coded into the interrupt's assembly, saving valuable cycles.



  - Todo: move all `config_struct.hh` files to a `conf_struct/` dir
  - Todo: create conf structs for adc_builtin, ... (others)

