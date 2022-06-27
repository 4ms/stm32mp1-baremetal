#pragma once
#include <cstdint>
#include <string_view>

struct BootDetect {
	// boot interface from Bootrom
	// - boot instance = bit 31:16
	// - boot device = bit 15:0
	//
	static constexpr uint32_t BOOTROM_PARAM_ADDR = 0x2FFC0078;
	static constexpr uint32_t BOOTROM_MODE_MASK = 0x0000FFFF;
	static constexpr uint32_t BOOTROM_MODE_SHIFT = 0;
	static constexpr uint32_t BOOTROM_INSTANCE_MASK = 0xFFFF0000;
	static constexpr uint32_t BOOTROM_INSTANCE_SHIFT = 16;
	static constexpr uint32_t BOOT_TYPE_MASK = 0xF0;
	static constexpr uint32_t BOOT_TYPE_SHIFT = 4;
	static constexpr uint32_t BOOT_INSTANCE_MASK = 0x0F;
	static constexpr uint32_t BOOT_INSTANCE_SHIFT = 0;

	enum BootMethod {
		BOOT_UNKNOWN = 0,
		BOOT_SDCARD = 1,
		BOOT_EMMC = 2,
		BOOT_NAND = 3,
		BOOT_NOR = 4,
		BOOT_UART = 5,
		BOOT_USB = 6,
		BOOT_SPINAND = 7,

		NUM_BOOT_METHODS,
	};

	enum BootDeviceInstance {
		BOOT_DEVICE_UNKNOWN = 0x0F,

		BOOT_FLASH_SD = 0x10,
		BOOT_FLASH_SD_1 = 0x11,
		BOOT_FLASH_SD_2 = 0x12,
		BOOT_FLASH_SD_3 = 0x13,

		BOOT_FLASH_EMMC = 0x20,
		BOOT_FLASH_EMMC_1 = 0x21,
		BOOT_FLASH_EMMC_2 = 0x22,
		BOOT_FLASH_EMMC_3 = 0x23,

		BOOT_FLASH_NAND = 0x30,
		BOOT_FLASH_NAND_FMC = 0x31,

		BOOT_FLASH_NOR = 0x40,
		BOOT_FLASH_NOR_QSPI = 0x41,

		BOOT_SERIAL_UART = 0x50,
		BOOT_SERIAL_UART_1 = 0x51,
		BOOT_SERIAL_UART_2 = 0x52,
		BOOT_SERIAL_UART_3 = 0x53,
		BOOT_SERIAL_UART_4 = 0x54,
		BOOT_SERIAL_UART_5 = 0x55,
		BOOT_SERIAL_UART_6 = 0x56,
		BOOT_SERIAL_UART_7 = 0x57,
		BOOT_SERIAL_UART_8 = 0x58,

		BOOT_SERIAL_USB = 0x60,
		BOOT_SERIAL_USB_OTG = 0x62,

		BOOT_FLASH_SPINAND = 0x70,
		BOOT_FLASH_SPINAND_1 = 0x71,

		NUM_BOOT_DEVICE_INSTANCES,
	};

	static BootMethod read_boot_method()
	{
		uint32_t bootrom_itf = read_raw_bootrom_itf();
		uint32_t bootrom_device = (bootrom_itf & BOOTROM_MODE_MASK) >> BOOTROM_MODE_SHIFT;

		auto boot_method = static_cast<BootMethod>(bootrom_device & 0x0F);
		return (boot_method < NUM_BOOT_METHODS) ? boot_method : BOOT_UNKNOWN;
	}

	static BootDeviceInstance read_boot_device()
	{
		uint32_t bootrom_itf = read_raw_bootrom_itf();
		uint32_t bootrom_device = (bootrom_itf & BOOTROM_MODE_MASK) >> BOOTROM_MODE_SHIFT;
		uint32_t bootrom_instance = (bootrom_itf & BOOTROM_INSTANCE_MASK) >> BOOTROM_INSTANCE_SHIFT;
		uint32_t boot_device = ((bootrom_device << BOOT_TYPE_SHIFT) & BOOT_TYPE_MASK) |
							   ((bootrom_instance << BOOT_INSTANCE_SHIFT) & BOOT_INSTANCE_MASK);

		auto boot_device_instance = static_cast<BootDeviceInstance>(boot_device);
		return (boot_device < NUM_BOOT_DEVICE_INSTANCES && boot_device > BOOT_DEVICE_UNKNOWN) ? boot_device_instance :
																								  BOOT_DEVICE_UNKNOWN;
	}

	static uint32_t read_raw_bootrom_itf()
	{
		return *reinterpret_cast<volatile uint32_t *>(BOOTROM_PARAM_ADDR);
	}

	static constexpr std::string_view bootmethod_string(BootMethod method)
	{
		switch (method) {
			case BOOT_SDCARD:
				return "SD Card";
				break;
			case BOOT_EMMC:
				return "EMMC";
				break;
			case BOOT_NAND:
				return "NAND";
				break;
			case BOOT_NOR:
				return "NOR";
				break;
			case BOOT_UART:
				return "UART";
				break;
			case BOOT_USB:
				return "USB";
				break;
			case BOOT_SPINAND:
				return "SPI NAND";
				break;
			default:
				return "Unknown";
		}
	}
};
