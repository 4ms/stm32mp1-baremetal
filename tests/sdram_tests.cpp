#include "doctest.h"
#include "drivers/sdram.hh"

SDRAMPeriph::SDRAMPeriph(const SDRAMConfig &defs) noexcept
	: defs{defs} {
}

TEST_CASE("SDRAM math tests") {

	CHECK(SDRAMPeriphMath::freq_to_clockdiv(480000000, 480) == 2);
	CHECK(SDRAMPeriphMath::freq_to_clockdiv(480000000, 240) == 2);
	CHECK(SDRAMPeriphMath::freq_to_clockdiv(480000000, 239) == 3);
	CHECK(SDRAMPeriphMath::freq_to_clockdiv(480000000, 167) == 3);
	CHECK(SDRAMPeriphMath::freq_to_clockdiv(480000000, 143) == 3);

	CHECK(SDRAMPeriphMath::freq_to_clockdiv(216000000, 240) == 2);
	CHECK(SDRAMPeriphMath::freq_to_clockdiv(216000000, 167) == 2);
	CHECK(SDRAMPeriphMath::freq_to_clockdiv(216000000, 143) == 2);
	CHECK(SDRAMPeriphMath::freq_to_clockdiv(216000000, 107) == 3);

	CHECK(SDRAMPeriphMath::ns_to_hclks(160000000, 6) == 1);
	CHECK(SDRAMPeriphMath::ns_to_hclks(160000000, 60) == 10);
	CHECK(SDRAMPeriphMath::ns_to_hclks(160000000, 18) == 3);
}

