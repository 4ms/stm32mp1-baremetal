// clang-format off
#include "doctest.h"
#include "stubs/scb_fake.hh"
#include "stubs/mpu_fake.hh"
#include "target/stm32h7x5/drivers/mpu.hh"
// clang-format on

static void reset_MPU() {
	// See Arm Cortex-M7 Devices Generic User Guide, version r1p2
	// https://developer.arm.com/documentation/dui0646/c/cortex-m7-peripherals/optional-memory-protection-unit
	MPU->TYPE = 0x00001000;
	MPU->RNR = 0xFFFFFFFF;
	MPU->RBAR = 0xFFFFFFFF;
	MPU->RASR = 0xFFFFFFFF;
	MPU->CTRL = 0;
	mdrivlib::stm32h7x5::MPU_::reset();
}

TEST_CASE("Fake MPU can be read/written") {
	reset_MPU();

	MPU->CTRL = 0;
	CHECK(MPU->CTRL == 0);
	MPU->CTRL |= MPU_CTRL_ENABLE_Msk;
	CHECK(MPU->CTRL == MPU_CTRL_ENABLE_Msk);

	SCB->SHCSR = 0;
	CHECK(SCB->SHCSR == 0);
	SCB->SHCSR = 0x98765432;
	CHECK(SCB->SHCSR == 0x98765432);
	SCB->SHCSR |= 0b1;
	CHECK(SCB->SHCSR == 0x98765433);
}

TEST_CASE("Disable cache in a region and check MPU registers are correct") {
	reset_MPU();

	uint32_t fake_region_base_addr = 0x1234C7A5;
	uint32_t fake_region_size = 5178;
	uint32_t smallest_base2_region_that_fits_over_fake = 8192;
	uint32_t base2_size = log2(smallest_base2_region_that_fits_over_fake);
	uint32_t expected_SIZE_val = base2_size - 1;

	auto region_id = mdrivlib::stm32h7x5::MPU_::disable_cache_for_dma_buffer(fake_region_base_addr, fake_region_size);

	CHECK(region_id >= 0);

	CHECK(MPU->RNR == 0);
	CHECK((MPU->RBAR & MPU_RBAR_ADDR_Msk) == (fake_region_base_addr & MPU_RBAR_ADDR_Msk));
	// Method 1:
	CHECK((MPU->RBAR & ~MPU_RBAR_ADDR_Msk) == 0);
	// Method 2:
	// CHECK((MPU->RBAR & MPU_RBAR_REGION_Msk) == 0);
	// CHECK((MPU->RBAR & MPU_RBAR_VALID_Msk) == 1);

	CHECK((MPU->RASR & MPU_RASR_SIZE_Msk) == (expected_SIZE_val << MPU_RASR_SIZE_Pos));
	CHECK((MPU->RASR & ~MPU_RASR_SIZE_Msk) == 0x030C0001); // just calculated this from ARM manual, TODO: show my work

	CHECK((MPU->CTRL & MPU_CTRL_ENABLE_Msk) == MPU_CTRL_ENABLE_Msk);
	CHECK((MPU->CTRL & MPU_CTRL_PRIVDEFENA_Msk) == MPU_CTRL_PRIVDEFENA_Msk);
}

TEST_CASE("Region sizes are correct") {
	reset_MPU();

	SUBCASE("Min size region is 32B") {
		uint32_t fake_region_base_addr = 0x1234C7A5;
		uint32_t fake_region_size = 31;
		uint32_t expected_SIZE_val = 4;
		MPU->RASR = 0xFFFFFFFF;
		mdrivlib::stm32h7x5::MPU_::disable_cache_for_dma_buffer(fake_region_base_addr, fake_region_size);
		CHECK((MPU->RASR & MPU_RASR_SIZE_Msk) == (expected_SIZE_val << MPU_RASR_SIZE_Pos));
	}
}

TEST_CASE("Enabling too many regions returns invalid region id") {
	reset_MPU();

	uint32_t fake_region_base_addr = 0x12345678;
	uint32_t fake_region_size = 128;

	for (int i = 0; i < 34; i++) {
		fake_region_base_addr += 0x01000000;

		auto region_id =
			mdrivlib::stm32h7x5::MPU_::disable_cache_for_dma_buffer(fake_region_base_addr, fake_region_size);

		if (i < 16)
			CHECK(region_id >= 0); // valid
		else
			CHECK(region_id < 0); // invalid
	}
}
