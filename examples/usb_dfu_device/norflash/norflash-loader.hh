#pragma once
#include "qspi_flash_driver.hh"
#include "stm32mp1xx.h"

struct NorFlashLoader {
	using QSpiFlash = mdrivlib::QSpiFlash;
	QSpiFlash flash;

	void erase_write_and_verify(uint8_t *data, uint32_t start_block_num, uint32_t len_bytes)
	{
		if (!len_bytes)
			return;

		uint32_t num_blocks = (len_bytes / QSPI_64KBLOCK_SIZE) + 1;
		if ((len_bytes % QSPI_64KBLOCK_SIZE) == 0)
			num_blocks--;
		erase_blocks(start_block_num, start_block_num + num_blocks);

		uint32_t start_addr = start_block_num * QSPI_64KBLOCK_SIZE;
		write(data, start_addr, len_bytes);

		verify(data, start_addr, len_bytes);
	}

	void erase_blocks(int range_start, int range_end)
	{
		for (int i = range_start; i < range_end; i++) {
			// Debug::red_LED1::low();
			printf("Erasing Block#%d @ 0x%x\n\r", i, i * QSPI_64KBLOCK_SIZE);
			bool ok = flash.erase(QSpiFlash::BLOCK_64K, i * QSPI_64KBLOCK_SIZE, QSpiFlash::EXECUTE_FOREGROUND);
			if (!ok) {
				printf("Error erasing block #%d\n\r", i);
				while (true) {
					// Debug::green_LED1::low();
					HAL_Delay(250);
					// Debug::green_LED1::high();
					HAL_Delay(250);
				}
			}
			// Debug::red_LED1::high();
		}
	}

	void write(uint8_t *data, uint32_t addr, uint32_t len)
	{
		// Debug::blue_LED1::low();
		printf("Writing %d bytes to 0x%x\n\r", len, addr);
		bool ok = flash.write(data, addr, len);
		if (!ok) {
			printf("Error writing\n\r");
			while (true) {
				// Debug::red_LED1::low();
				// HAL_Delay(250);
				// Debug::red_LED1::high();
				// HAL_Delay(250);
			}
		}
		// Debug::blue_LED1::high();
	}

	void verify(uint8_t *data, uint32_t addr, uint32_t len)
	{
		uint8_t read_data[4096];
		if (len > 4096) {
			printf("Cannot verify more than 4kB\n");
			// TODO: run verify in smaller blocks
		}

		printf("Reading %d bytes to 0x%x\n\r", len, addr);
		bool ok = flash.read(read_data, addr, len, mdrivlib::QSpiFlash::EXECUTE_FOREGROUND);
		if (!ok) {
			printf("Error reading\n\r");
			while (true) {
			}
		}
		for (int i = 0; i < len; i++) {
			if (read_data[i] != data[i]) {
				printf("Data read back does not match: [%d] read: 0x%x, wrote: 0x%x\n\r", i, read_data[i], data[i]);
				while (true) {
				}
			}
		}
	}

	NorFlashLoader(const mdrivlib::QSPIFlashConfig &conf)
		: flash{conf}
	{
		printf("QSPI is initialized.\n\r");

		// Write FSBL1 @ first block: Success = red turns on
		// erase_write_and_verify(u_boot_spl_stm32, 0, u_boot_spl_stm32_len);
		// Write FSBL2 @ block 4 = 0x40000: Success =  blue turns on (magenta)
		// erase_write_and_verify(u_boot_spl_stm32, 4, u_boot_spl_stm32_len);
		// Write app data @ block 8 = 0x80000: Sucess = green turns on (white)
		// erase_write_and_verify(main_uimg, 8, main_uimg_len);

		// printf("Success!\n");
	}
};
