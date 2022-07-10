#include "print_messages.hh"
#include <cstdint>

namespace RamTests
{

bool test_first_word()
{
	volatile uint32_t *addr = reinterpret_cast<uint32_t *>(0xC0000000);
	*addr = 0x12345678;
	if (*addr == 0x12345678)
		return true;

	panic("RAM Test Fail: test_first_word(): Wrote 0x12345678, read 0x", Hex{*addr});
	return false;
}

bool run_all(uint32_t ram_start, uint32_t ram_size)
{
	bool ok = test_first_word();

	return ok;
}

} // namespace RamTests
