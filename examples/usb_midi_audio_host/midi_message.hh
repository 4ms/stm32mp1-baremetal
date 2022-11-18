#pragma once
#include <array>
#include <cstdint>
#include <cstdio>

// Specifications from
// https://www.midi.org/specifications/midi1-specifications/m1-v4-2-1-midi-1-0-detailed-specification-96-1-4

enum MidiCommand : uint8_t {
	NoteOff = 0x8,
	NoteOn = 0x9,
	PolyKeyPressue = 0xA,
	ControlChange = 0xB,
	ProgramChange = 0xC,
	ChannelPressure = 0xD,
	PitchBend = 0xE,
	Sys = 0xF,
};

struct MidiStatusByte {
	MidiCommand command : 4;
	uint8_t channel : 4;

	static MidiStatusByte make(uint8_t raw)
	{
		return MidiStatusByte{
			.command = MidiCommand(raw >> 4),
			.channel = uint8_t(raw & 0x0F),
		};
	}

	operator uint8_t() const { return (uint8_t)command << 4 | channel; }
};

struct MidiDataBytes {
	uint8_t byte[2];

	operator uint16_t() const { return ((uint16_t)byte[0] << 8) | byte[1]; }
};

enum MidiSystemCommonCommand : uint8_t {
	TimeCodeQuarterFrame = 0xF1,
	SongPositionPtr = 0xF2,
	SongSelect = 0xF3,
	TuneRequest = 0xF6,
	EndExclusive = 0xF7,
};

enum MidiSystemRealTimeCommand : uint8_t {
	TimingClock = 0xF8,
	Start = 0xFA,
	Continue = 0xFB,
	Stop = 0xFC,
	ActiveSending = 0xFE,
	SystemReset = 0xFF,
};

enum MidiSystemExclusiveCommand : uint8_t {
	SysEx = 0xF0,
};

struct MidiMessage {
	MidiStatusByte status;
	MidiDataBytes data;

	MidiMessage() = default;

	MidiMessage(uint8_t status_byte, uint8_t data_byte0 = 0, uint8_t data_byte1 = 0)
		: status{MidiStatusByte::make(status_byte)}
		, data{data_byte0, data_byte1}
	{}

	template<MidiCommand cmd>
	bool is_command() const
	{
		return (status.command == cmd);
	}

	template<MidiSystemCommonCommand cmd>
	bool is_system_common() const
	{
		return status == cmd;
	}

	template<MidiSystemRealTimeCommand cmd>
	bool is_system_realtime() const
	{
		return status == cmd;
	}

	bool is_sysex() const { return status == SysEx; }

	uint32_t raw() const { return (status << 16) | data; }

	static void print(MidiMessage msg)
	{
		if (msg.is_command<NoteOn>()) {
			printf("Note: %d Vel: %d\n", msg.data.byte[0], msg.data.byte[1]);
		} else if (msg.is_command<NoteOff>()) {
			printf("Note: %d OFF\n", msg.data.byte[0]);
		} else if (msg.is_command<PolyKeyPressue>()) {
			printf("Poly Key Pressure: %d %d\n", msg.data.byte[0], msg.data.byte[1]);
		} else if (msg.is_command<ControlChange>()) {
			printf("CC: #%d = %d\n", msg.data.byte[0], msg.data.byte[1]);
		} else if (msg.is_command<ProgramChange>()) {
			printf("PC: #%d\n", msg.data.byte[0]);
		} else if (msg.is_command<ChannelPressure>()) {
			printf("CP: #%d\n", msg.data.byte[0]);
		} else if (msg.is_command<ChannelPressure>()) {
			printf("Bend: #%d\n", (msg.data.byte[0] | (msg.data.byte[1] << 7)) - 8192);
		} else if (msg.is_system_realtime<TimingClock>()) {
			// printf("Clk\n");
		} else if (msg.is_sysex()) {
			printf("SYSEX: 0x%02x%02x\n", msg.data.byte[0], msg.data.byte[1]);
		} else if (msg.raw()) {
			printf("Raw: %06x\n", msg.raw());
		}
	}

	void print() const { MidiMessage::print(*this); }
};
