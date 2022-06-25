#pragma once
#include <cstdint>
#include <string_view>

struct BootDetect {
	/* boot interface from Bootrom
	 * - boot instance = bit 31:16
	 * - boot device = bit 15:0
	 */
	static constexpr uint32_t BOOTROM_PARAM_ADDR = 0x2FFC0078;
	static constexpr uint32_t BOOTROM_MODE_MASK = 0x0000FFFF;
	static constexpr uint32_t BOOTROM_MODE_SHIFT = 0;
	static constexpr uint32_t BOOTROM_INSTANCE_MASK = 0xFFFF0000;
	static constexpr uint32_t BOOTROM_INSTANCE_SHIFT = 16;
	static constexpr uint32_t BOOT_TYPE_MASK = 0xF0;
	static constexpr uint32_t BOOT_TYPE_SHIFT = 4;
	static constexpr uint32_t BOOT_INSTANCE_MASK = 0x0F;
	static constexpr uint32_t BOOT_INSTANCE_SHIFT = 0;

	enum boot_device {
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
	};

	//
	static uint32_t read_bootmode()
	{
		uint32_t bootrom_itf = *reinterpret_cast<volatile uint32_t *>(BOOTROM_PARAM_ADDR);
		uint32_t bootrom_device = (bootrom_itf & BOOTROM_MODE_MASK) >> BOOTROM_MODE_SHIFT;
		uint32_t bootrom_instance = (bootrom_itf & BOOTROM_INSTANCE_MASK) >> BOOTROM_INSTANCE_SHIFT;
		uint32_t boot_mode = ((bootrom_device << BOOT_TYPE_SHIFT) & BOOT_TYPE_MASK) |
							 ((bootrom_instance << BOOT_INSTANCE_SHIFT) & BOOT_INSTANCE_MASK);

		return boot_mode;
	}

	static constexpr std::string_view bootmode_string(uint32_t mode)
	{
		switch (mode & BOOT_TYPE_MASK) {
			case BOOT_FLASH_SD:
				return "SD Card";
				break;
			case BOOT_FLASH_EMMC:
				return "EMMC";
				break;
			case BOOT_FLASH_NAND:
				return "NAND";
				break;
			case BOOT_FLASH_NOR:
				return "NOR";
				break;
			case BOOT_SERIAL_UART:
				return "UART";
				break;
			case BOOT_SERIAL_USB:
				return "USB";
				break;
			case BOOT_FLASH_SPINAND:
				return "SPI NAND";
				break;
			default:
				return "Unknown";
		}
	}
};
