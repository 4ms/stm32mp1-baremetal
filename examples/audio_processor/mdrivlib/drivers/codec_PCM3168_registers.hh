#pragma once
#include <cstdint>

namespace mdrivlib::CodecPCM3168Register
{
constexpr uint8_t I2C_BASE_ADDR = 0b10001000; // Shifted address, PCM3168 datasheet section 9.3.14

struct ResetControl {
	static constexpr uint8_t Address = 0x40;
	enum ModeReset : uint8_t { Reset = 0, NoReset = (1 << 7) };
	enum SystemReset : uint8_t { Resync = 0, NoResync = (1 << 6) };
	enum DacSamplingMode : uint8_t { Auto = 0, Single = 0b01, Dual = 0b10, Quad = 0b11 };
};

struct DacControl1 {
	static constexpr uint8_t Address = 0x41;
	enum PowerSaveModeSelect : uint8_t { PowerSaveEnable = 0, PowerSaveDisable = 1 << 7 };
	enum MasterSlave : uint8_t {
		SlaveMode = 0b000 << 4,
		MasterMode768fs = 0b001 << 4,
		MasterMode512fs = 0b010 << 4,
		MasterMode384fs = 0b011 << 4,
		MasterMode256fs = 0b100 << 4,
		MasterMode192fs = 0b101 << 4,
		MasterMode128fs = 0b110 << 4,
	};
	enum DacAudioFormat : uint8_t {
		I2S_24bit = 0b0000,
		LeftJustified_24bit = 0b0001,
		RightJustified_24bit = 0b0010,
		RightJustified_16bit = 0b0011,
		I2S_DSP_24bit = 0b0100,
		LeftJustified_DSP_24bit = 0b0101,
		I2S_TDM_24bit = 0b0110,
		LeftJustified_TDM_24bit = 0b0111,
		I2S_HighSpeed_TDM_24bit = 0b1000,
		LeftJustified_HighSpeed_TDM_24bit = 0b1001,
	};
};

struct DacControl2 {
	static constexpr uint8_t Address = 0x42;
	static constexpr uint8_t DacAllEnable = (0b0000 << 4);
	enum Dac12Control : uint8_t { Dac12Enable = 0, Dac12Disable = 1 << 4 };
	enum Dac34Control : uint8_t { Dac34Enable = 0, Dac34Disable = 1 << 5 };
	enum Dac56Control : uint8_t { Dac56Enable = 0, Dac56Disable = 1 << 6 };
	enum Dac78Control : uint8_t { Dac78Enable = 0, Dac78Disable = 1 << 7 };
	static constexpr uint8_t DacAllRollOffSlow = (0b1111 << 0);
	enum Dac12RollOff : uint8_t { Dac12RollOffSharp = 0 << 0, Dac12RollOffSlow = 1 << 0 };
	enum Dac34RollOff : uint8_t { Dac34RollOffSharp = 0 << 1, Dac34RollOffSlow = 1 << 1 };
	enum Dac56RollOff : uint8_t { Dac56RollOffSharp = 0 << 2, Dac56RollOffSlow = 1 << 2 };
	enum Dac78RollOff : uint8_t { Dac78RollOffSharp = 0 << 3, Dac78RollOffSlow = 1 << 3 };
};

struct DacPhase {
	static constexpr uint8_t Address = 0x43;
	enum : uint8_t {
		NoDacInverted = 0,
		Dac1Invert = 1 << 0,
		Dac2Invert = 1 << 1,
		Dac3Invert = 1 << 2,
		Dac4Invert = 1 << 3,
		Dac5Invert = 1 << 4,
		Dac6Invert = 1 << 5,
		Dac7Invert = 1 << 6,
		Dac8Invert = 1 << 7,
	};
};

struct DacSoftMute {
	static constexpr uint8_t Address = 0x44;
	enum : uint8_t {
		NoDacMuted = 0,
		Dac1Mute = 1 << 0,
		Dac2Mute = 1 << 1,
		Dac3Mute = 1 << 2,
		Dac4Mute = 1 << 3,
		Dac5Mute = 1 << 4,
		Dac6Mute = 1 << 5,
		Dac7Mute = 1 << 6,
		Dac8Mute = 1 << 7,
	};
};

struct DacZeroDetect {
	static constexpr uint8_t Address = 0x45;
	// TODO
};

struct DacControl3 {
	static constexpr uint8_t Address = 0x46;
	// TODO
};

enum class DacAtten { ZeroDB = 255, N100dB = 53, MuteStart = 54, Mute = 0 };
struct DacAllAtten {
	static constexpr uint8_t Address = 0x47;
};
struct Dac1Atten {
	static constexpr uint8_t Address = 0x48;
};
struct Dac2Atten {
	static constexpr uint8_t Address = 0x49;
};
struct Dac3Atten {
	static constexpr uint8_t Address = 0x4a;
};
struct Dac4Atten {
	static constexpr uint8_t Address = 0x4b;
};
struct Dac5Atten {
	static constexpr uint8_t Address = 0x4c;
};
struct Dac6Atten {
	static constexpr uint8_t Address = 0x4d;
};
struct Dac7Atten {
	static constexpr uint8_t Address = 0x4e;
};
struct Dac8Atten {
	static constexpr uint8_t Address = 0x4f;
};

struct AdcSamplingMode {
	static constexpr uint8_t Address = 0x50;
	enum : uint8_t { Auto = 0b00, Single = 0b01, Dual = 0b10 };
};

struct AdcControl1 {
	static constexpr uint8_t Address = 0x51;
	enum MasterSlave : uint8_t {
		SlaveMode = 0b000 << 4,
		MasterMode768fs = 0b001 << 4,
		MasterMode512fs = 0b010 << 4,
		MasterMode384fs = 0b011 << 4,
		MasterMode256fs = 0b100 << 4,
	};
	enum AdcAudioFormat : uint8_t {
		I2S_24bit = 0b000,
		LeftJustified_24bit = 0b001,
		RightJustified_24bit = 0b010,
		RightJustified_16bit = 0b011,
		I2S_DSP_24bit = 0b100,
		LeftJustified_DSP_24bit = 0b101,
		I2S_TDM_24bit = 0b110,
		LeftJustified_TDM_24bit = 0b111,
	};
};

struct AdcControl2 {
	static constexpr uint8_t Address = 0x52;
	enum Adc12PowerSave : uint8_t {};
	enum Adc34PowerSave : uint8_t {};
	enum Adc56PowerSave : uint8_t {};
	enum Adc78PowerSave : uint8_t {};
	enum : uint8_t { AdcAllHPFDisabled = 0b111 };
	enum Adc12HPF : uint8_t { Adc12HPFDisabled = 0b001 };
	enum Adc34HPF : uint8_t { Adc34HPFDisabled = 0b010 };
	enum Adc56HPF : uint8_t { Adc56HPFDisabled = 0b010 };
	enum Adc78HPF : uint8_t { Adc78HPFDisabled = 0b100 };
};

struct AdcInputType {
	static constexpr uint8_t Address = 0x53;
	enum : uint8_t {
		AllAdcDifferential = 0,
		Adc1Differential = 1 << 0,
		Adc2Differential = 1 << 1,
		Adc3Differential = 1 << 2,
		Adc4Differential = 1 << 3,
		Adc5Differential = 1 << 4,
		Adc6Differential = 1 << 5,
		Adc1SingleEnded = 1 << 0,
		Adc2SingleEnded = 1 << 1,
		Adc3SingleEnded = 1 << 2,
		Adc4SingleEnded = 1 << 3,
		Adc5SingleEnded = 1 << 4,
		Adc6SingleEnded = 1 << 5,
		AllAdcSingleEnded = 0b111111,
	};
};

struct AdcInputPhase {
	static constexpr uint8_t Address = 0x54;
	// TODO
};

struct AdcSoftMute {
	static constexpr uint8_t Address = 0x55;
	enum : uint8_t {
		NoAdcMuted = 0,
		Adc1Mute = 1 << 0,
		Adc2Mute = 1 << 1,
		Adc3Mute = 1 << 2,
		Adc4Mute = 1 << 3,
		Adc5Mute = 1 << 4,
		Adc6Mute = 1 << 5,
	};
};

struct AdcOverflowRO {
	static constexpr uint8_t Address = 0x56;
	// TODO
};

struct AdcControl3 {
	static constexpr uint8_t Address = 0x57;
	// TODO
};

enum class AdcAtten { Plus20DB = 255, ZeroDB = 215, N100dB = 15, MuteStart = 14, Mute = 0 };
struct AdcAllAtten {
	static constexpr uint8_t Address = 0x47;
};
struct Adc1Atten {
	static constexpr uint8_t Address = 0x48;
};
struct Adc2Atten {
	static constexpr uint8_t Address = 0x49;
};
struct Adc3Atten {
	static constexpr uint8_t Address = 0x4a;
};
struct Adc4Atten {
	static constexpr uint8_t Address = 0x4b;
};
struct Adc5Atten {
	static constexpr uint8_t Address = 0x4c;
};
struct Adc6Atten {
	static constexpr uint8_t Address = 0x4d;
};
struct Adc7Atten {
	static constexpr uint8_t Address = 0x4e;
};
struct Adc8Atten {
	static constexpr uint8_t Address = 0x4f;
};

} // namespace mdrivlib::CodecPCM3168Register
