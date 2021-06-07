#pragma once
#include "stm32mp157cxx_ca7.h"

extern uint32_t aux_core_start; // defined in startup.s

// Controls MPU1 from MPU0
struct SecondaryCore {

	static inline volatile uint32_t *MagicNumberRegister = &(TAMP->BKP4R);
	static inline volatile uint32_t *BranchAddressRegister = &(TAMP->BKP5R);
	enum { MagicNumberValue = 0xCA7FACE1 };

	// Start MPU1 (aux core A7) from the _Reset function
	// From https://wiki.st.com/stm32mpu/wiki/STM32MP15_ROM_code_overview#Secondary_core_boot
	static void start()
	{
		setup_sgi();
		unlock_backup_registers();

		uint32_t aux_core_starting_address = reinterpret_cast<uint32_t>(&aux_core_start);
		*BranchAddressRegister = aux_core_starting_address;
		*MagicNumberRegister = MagicNumberValue;
		send_sgi();
	}

	static void reset()
	{
		// Reset MPU1
		RCC->MP_GRSTCSETR = RCC_MP_GRSTCSETR_MPUP1RST;
		__DSB();
		__ISB();
	}

private:
	static constexpr auto irq = SGI0_IRQn;

	static void unlock_backup_registers()
	{
		// Turn on the Disable Backup Protection bit, to allow us to write to
		// the TAMP backup registers This is already on if we just booted from
		// U-boot, but if we are debugging and manually reset the code, we'll
		// want to be sure.
		PWR->CR1 = PWR->CR1 | PWR_CR1_DBP;
		while (!(PWR->CR1 & PWR_CR1_DBP))
			;

		// Turn off Write protection on backup registers (BOOTROM seems to turn
		// it on for Backup registers 0-4 during MPU1 boot-up)
		TAMP->SMCR = (0 << TAMP_SMCR_BKPRWDPROT_Pos) | (0 << TAMP_SMCR_BKPWDPROT_Pos) | (1 << TAMP_SMCR_TAMPDPROT_Pos);
	}

	static void setup_sgi()
	{
		GIC_EnableIRQ(irq);
	}

	static void send_sgi()
	{
		constexpr uint32_t filter_use_cpu_sel_bits = 0b00;
		constexpr uint32_t cpu1 = 1 << 1;
		GIC_SendSGI(irq, cpu1, filter_use_cpu_sel_bits);
	}
};
