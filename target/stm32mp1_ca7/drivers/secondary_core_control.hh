#pragma once
#include "stm32xx.h"

extern uint32_t aux_core_start; // defined in startup.s

// Controls MPU1 from MPU0
struct SecondaryCoreController {

	enum { MagicNumberValue = 0xCA7FACE1 };
	static inline volatile uint32_t *MagicNumberRegister = &(TAMP->BKP4R);
	static inline volatile uint32_t *BranchAddressRegister = &(TAMP->BKP5R);

	static constexpr auto irq0 = SGI0_IRQn;

	// Start MPU1 (aux core A7) from the _Reset function
	// From https://wiki.st.com/stm32mpu/wiki/STM32MP15_ROM_code_overview#Secondary_core_boot
	static void start() {
		setup_sgi();
		unlock_backup_registers();

		// Todo: some way to reload new firmware while debugging without a hard reset
		// This mimicks what u-boot does, but it doesn't work.
		// stop();
		// reset();
		// *MagicNumberRegister = 0xFFFFFFFF;
		// while (*MagicNumberRegister != 0)
		// 	send_sgi();

		*BranchAddressRegister = reinterpret_cast<uint32_t>(&aux_core_start);
		*MagicNumberRegister = MagicNumberValue;
		send_sgi(irq0);
	}

	static void reset() {
		// Reset MPU1
		RCC->MP_GRSTCSETR = RCC_MP_GRSTCSETR_MPUP1RST;
		__DSB();
		__ISB();
	}

	static void stop() {
		// Stop request for core 1
		// Todo: Does this work?
		RCC->MP_SREQSETR = RCC_MP_SREQSETR_STPREQ_P1;
		__DSB();
		__ISB();
	}

	static void send_sgi(IRQn_Type irq) {
		constexpr uint32_t filter_use_cpu_sel_bits = 0b00;
		constexpr uint32_t cpu1 = 1 << 1;
		GIC_SendSGI(irq, cpu1, filter_use_cpu_sel_bits);
	}

private:
	static void unlock_backup_registers() {
		// Turn on the Disable Backup Protection bit, to allow us to write to the TAMP backup registers
		PWR->CR1 = PWR->CR1 | PWR_CR1_DBP;
		while (!(PWR->CR1 & PWR_CR1_DBP))
			;

		// Turn off Write protection on backup registers (BOOTROM seems to turn it on during MPU1 boot-up)
		TAMP->SMCR = (0 << TAMP_SMCR_BKPRWDPROT_Pos) | (0 << TAMP_SMCR_BKPWDPROT_Pos) | (1 << TAMP_SMCR_TAMPDPROT_Pos);
	}

	static void setup_sgi() {
		GIC_EnableIRQ(irq0);
	}
};

using SecondaryCore = SecondaryCoreController;
