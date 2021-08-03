#include "rotary.hh"

namespace mdrivlib
{

enum HalfStepRotaryState : uint8_t {
	HS_R_START = 0x0,
	HS_R_CCW_BEGIN = 0x01,
	HS_R_CW_BEGIN = 0x02,
	HS_R_START_M = 0x03,
	HS_R_CW_BEGIN_M = 0x04,
	HS_R_CCW_BEGIN_M = 0x05,
	HS_DIR_CW = 0x10,
	HS_DIR_CCW = 0x20,
};

enum FullStepRotaryState : uint8_t {
	FS_R_START = 0x0,
	FS_R_CW_FINAL = 0x01,
	FS_R_CW_BEGIN = 0x02,
	FS_R_CW_NEXT = 0x03,
	FS_R_CCW_BEGIN = 0x04,
	FS_R_CCW_FINAL = 0x05,
	FS_R_CCW_NEXT = 0x06,
	FS_DIR_CW = 0x10,
	FS_DIR_CCW = 0x20,
};

enum RotaryDir : uint8_t {
	DIR_NONE = 0x00,
	DIR_CW = 0x10,
	DIR_CCW = 0x20,
};

RotaryBase::RotaryBase(GPIO portA, uint8_t pin_numA, GPIO portB, uint8_t pin_numB)
	: pinA{portA, pin_numA, PinMode::Input, PinAF::AFNone, PinPull::Up, PinPolarity::Inverted}
	, pinB{portB, pin_numB, PinMode::Input, PinAF::AFNone, PinPull::Up, PinPolarity::Inverted}
	, state_{0x00}
	, position_{0} {
}

void RotaryBase::update(const uint32_t table_select) {
	uint8_t pinstate = pinA.is_on() ? 0b00 : 0b10;
	pinstate += pinB.is_on() ? 0b00 : 0b01;
	state_ = StateTable[(state_ & 0xf) + table_select][pinstate];
	uint8_t motion = state_ & (DIR_CW | DIR_CCW);
	if (motion == DIR_CW) {
		position_++;
	}
	if (motion == DIR_CCW) {
		position_--;
	}
}

int32_t RotaryBase::read_position() {
	auto tmp = position_;
	position_ = 0;
	return tmp;
}

const uint8_t RotaryBase::StateTable[13][4] = {
	// HalfStep
	// R_START (00)
	{HS_R_START_M, HS_R_CW_BEGIN, HS_R_CCW_BEGIN, HS_R_START},

	// R_CCW_BEGIN
	{HS_R_START_M | HS_DIR_CCW, HS_R_START, HS_R_CCW_BEGIN, HS_R_START},

	// HS_R_CW_BEGIN
	{HS_R_START_M | HS_DIR_CW, HS_R_CW_BEGIN, HS_R_START, HS_R_START},

	// HS_R_START_M (11)
	{HS_R_START_M, HS_R_CCW_BEGIN_M, HS_R_CW_BEGIN_M, HS_R_START},

	// R_CW_BEGIN_M
	{HS_R_START_M, HS_R_START_M, HS_R_CW_BEGIN_M, HS_R_START | HS_DIR_CW},

	// R_CCW_BEGIN_M
	{HS_R_START_M, HS_R_CCW_BEGIN_M, HS_R_START_M, HS_R_START | HS_DIR_CCW},

	// FullStep
	// FS_R_START
	{FS_R_START, FS_R_CW_BEGIN, FS_R_CCW_BEGIN, FS_R_START},

	// R_CW_FINAL
	{FS_R_CW_NEXT, FS_R_START, FS_R_CW_FINAL, FS_R_START | FS_DIR_CW},

	// R_CW_BEGIN
	{FS_R_CW_NEXT, FS_R_CW_BEGIN, FS_R_START, FS_R_START},

	// R_CW_NEXT
	{FS_R_CW_NEXT, FS_R_CW_BEGIN, FS_R_CW_FINAL, FS_R_START | FS_DIR_CW},

	// R_CCW_BEGIN
	{FS_R_CCW_NEXT, FS_R_START, FS_R_CCW_BEGIN, FS_R_START},

	// R_CCW_FINAL
	{FS_R_CCW_NEXT, FS_R_CCW_FINAL, FS_R_START, FS_R_START | FS_DIR_CCW},

	// R_CCW_NEXT
	{FS_R_CCW_NEXT, FS_R_CCW_FINAL, FS_R_CCW_BEGIN, FS_R_START | FS_DIR_CCW},
};
} // namespace mdrivlib
