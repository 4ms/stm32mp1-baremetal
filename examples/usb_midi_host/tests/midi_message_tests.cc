#include "doctest.h"
#include "midi_message.hh"

TEST_CASE("MidiStatusByte conversions")
{
	auto m = MidiStatusByte::make(0xB2);
	CHECK(m.command == ControlChange);
	CHECK(m.channel == 2);

	MidiStatusByte n{.command = NoteOn, .channel = 4};
	CHECK(((uint8_t)n) == 0x94);
}

TEST_CASE("MidiDataBytes conversions")
{
	MidiDataBytes d;
	d.byte[0] = 0x40;
	d.byte[1] = 0x7F;

	CHECK((uint16_t)d == 0x407F);
}

TEST_CASE("MidiMessage")
{
	MidiMessage m(0x92, 0x40, 0x7F);
	CHECK(m.is_command<NoteOn>());
	CHECK_FALSE(m.is_command<NoteOff>());
	CHECK_FALSE(m.is_system_common<SongSelect>());
	CHECK_FALSE(m.is_system_realtime<TimingClock>());
	CHECK_FALSE(m.is_sysex());
	CHECK(m.raw() == 0x92407F);

	MidiMessage tim(0xF8);
	CHECK(tim.is_system_realtime<TimingClock>());

	MidiMessage start(0xFA);
	CHECK(start.is_system_realtime<Start>());

	MidiMessage sysex(0xF0);
	CHECK(sysex.is_sysex());

	MidiMessage endsysex(0xF7);
	CHECK(endsysex.is_system_common<EndExclusive>());

	MidiMessage timecode(0xF1);
	CHECK(timecode.is_system_common<TimeCodeQuarterFrame>());
}
