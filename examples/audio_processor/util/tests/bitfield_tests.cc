#include "../bitfield.hh"
#include "doctest.h"
#include <cstdint>

BitFieldBase<uint8_t> bf8;
BitFieldBase<uint16_t> bf16;
BitFieldBase<uint32_t> bf32;

enum bits8 : uint8_t {
	BIT8_0 = 1,
	BIT8_1 = 2,
	BIT8_2 = 4,
	BIT8_3 = 8,

	BIT8_7 = 128,
};

enum bitsDefault {
	BITdef_0 = 1,
	BITdef_1 = 0xFFFFFFF0,
};

TEST_CASE("bitfield_tests: single_bit_equals_itself")
{
	CHECK(bf8.bitfield(BIT8_0) == BIT8_0);
	CHECK(bf16.bitfield(BIT8_1) == BIT8_1);
	CHECK(bf32.bitfield(BIT8_2) == BIT8_2);
}

TEST_CASE("bitfield_tests: two_single_bits_equals_OR")
{
	CHECK(bf8.bitfield(BIT8_0, BIT8_3) == (BIT8_0 | BIT8_3));
	CHECK(bf8.bitfield(BIT8_3, BIT8_7) == (BIT8_3 | BIT8_7));
}

TEST_CASE("bitfield_tests: three_parameters_equals_OR")
{
	CHECK(bf8.bitfield(BIT8_0, BIT8_2, BIT8_3) == (BIT8_0 | BIT8_2 | BIT8_3));
}

TEST_CASE("bitfield_tests: overlapping_bits_equals_OR")
{
	uint8_t num1 = 0b1100;
	uint8_t num2 = 0b1010;
	CHECK(bf8.bitfield(num1, num2) == (num1 | num2));
}

TEST_CASE("bitfield_tests: overflow_ignores_higher_bits")
{
	uint32_t big_num = 0x89ABCD5A;
	uint32_t big_num2 = 0x12345678;
	uint8_t bottom_byte = static_cast<uint8_t>(0x000000FF & big_num);
	uint8_t bottom_byte2 = static_cast<uint8_t>(0x000000FF & big_num2);
	CHECK(bf8.bitfield(big_num, big_num2) == (bottom_byte | bottom_byte2));
}

TEST_CASE("bitfield_tests: underflow_zeroes_top_bits")
{
	uint8_t little_num = 0x12;
	uint8_t little_num2 = 0x2D;
	uint32_t big_little_num = 0x00000012;
	uint32_t big_little_num2 = 0x0000002D;
	CHECK(bf32.bitfield(little_num, little_num2) == (big_little_num | big_little_num2));
}

