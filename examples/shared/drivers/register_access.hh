// Based on github.com/kensmith/cortex-from-scratch
// Extended and modified by Dan Green.
// Todo: Seek license/permission?
#pragma once
#include <cstdint>
#include <limits>

namespace mdrivlib
{
using regsize_t = uint32_t;

// RegisterBits<>: Bits in register specified by mask
// todo: Require AccessPolicyT has read() write() set() clear()
template<typename AccessPolicyT, regsize_t address, regsize_t mask>
struct RegisterBits {
	static constexpr regsize_t BaseAddress = address;
	static constexpr regsize_t Mask = mask;

	static regsize_t read() {
		return AccessPolicyT::read(reinterpret_cast<volatile regsize_t *>(address), mask);
	}
	static void write(regsize_t val) {
		AccessPolicyT::write(reinterpret_cast<volatile regsize_t *>(address), mask, val);
	}
	static void set() {
		AccessPolicyT::set(reinterpret_cast<volatile regsize_t *>(address), mask);
	}
	static void clear() {
		AccessPolicyT::clear(reinterpret_cast<volatile regsize_t *>(address), mask);
	}
};

// target::RCC_Reset::SAI4_::set();
// target::RCC_Reset_Release::SAI4_::set();
// Null register (periph does not exist)
struct NonexistantRegister {
	static regsize_t read() {
		return 0;
	}
	static void write(regsize_t val) {
	}
	static void set() {
	}
	static void clear() {
	}
};

// RegisterSection<>: Bits in register specified by offset and width
template<regsize_t width>
struct generate_unshifted_mask_t {
	enum { value = (generate_unshifted_mask_t<width - 1>::value << 1) | 1 };
};
template<>
struct generate_unshifted_mask_t<0> {
	enum { value = 0 };
};

template<regsize_t offset, regsize_t width>
struct generate_mask_t {
	enum { value = generate_unshifted_mask_t<width>::value << offset };
};

template<regsize_t offset, regsize_t width>
constexpr regsize_t mask_v = generate_mask_t<offset, width>::value;

template<typename AccessPolicyT, regsize_t address, regsize_t offset, regsize_t width>
struct RegisterSection {
	static_assert(width > 0, "Register must be at least 1 bit wide");
	static_assert(width + offset <= std::numeric_limits<regsize_t>::digits,
				  "Register bits specified by width+offset exceeds size of register");

	static regsize_t read() {
		return AccessPolicyT::read(reinterpret_cast<volatile regsize_t *>(address), offset, mask_v<offset, width>);
	}

	static void write(regsize_t val) {
		return AccessPolicyT::write(
			reinterpret_cast<volatile regsize_t *>(address), offset, mask_v<offset, width>, val);
	}

	static void set() {
		AccessPolicyT::set(reinterpret_cast<volatile regsize_t *>(address), mask_v<offset, width>);
	}

	static void clear() {
		AccessPolicyT::clear(reinterpret_cast<volatile regsize_t *>(address), mask_v<offset, width>);
	}
};

// Access Policies:

struct ReadOnly {
	static regsize_t read(volatile regsize_t *address, regsize_t offset, regsize_t mask) {
		return (*address & mask) >> offset;
	}
	static regsize_t read(volatile regsize_t *address, regsize_t mask) {
		return *address & mask;
	}
};

struct WriteOnly {
	static void write(volatile regsize_t *address, regsize_t offset, regsize_t mask, regsize_t val) {
		*address = ((val << offset) & mask);
	}
	static void write(volatile regsize_t *address, regsize_t mask, regsize_t val) {
		*address = (val & mask);
	}
	static void set(volatile regsize_t *address, regsize_t mask) {
		*address = mask;
	}
};

struct ReadWrite : ReadOnly {
	static void write(volatile regsize_t *address, regsize_t offset, regsize_t mask, regsize_t val) {
		*address = (*address & ~mask) | ((val << offset) & mask);
	}
	static void write(volatile regsize_t *address, regsize_t mask, regsize_t val) {
		*address = (*address & ~mask) | (val & mask);
	}
	static void set(volatile regsize_t *address, regsize_t mask) {
		*address = *address | mask;
	}
	static void clear(volatile regsize_t *address, regsize_t mask) {
		*address = *address & ~mask;
	}
};

// RegisterDualSetClear<>: set() sets the Set bit(s), clear() sets the Clear bit(s).
// Access to Set/Clear is always WriteOnly.
template<regsize_t SetAddress, regsize_t SetMask, regsize_t ClearAddress, regsize_t ClearMask>
struct RegisterDualSetClear {
	static void set() {
		WriteOnly::set(reinterpret_cast<volatile regsize_t *>(SetAddress), SetMask);
	}
	static void clear() {
		WriteOnly::set(reinterpret_cast<volatile regsize_t *>(ClearAddress), ClearMask);
	}
};

template<typename SetT, typename ClearT>
using RegisterSetClear = RegisterDualSetClear<SetT::BaseAddress, SetT::Mask, ClearT::BaseAddress, ClearT::Mask>;
} // namespace mdrivlib
