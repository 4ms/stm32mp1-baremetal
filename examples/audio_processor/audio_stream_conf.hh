#pragma once
#include "util/audio_frame.hh"

struct AudioStreamConf {

	// BlockSize: Number of Frames processed each time AudioStream::process() is called
	static constexpr int BlockSize = 32;

	// Oddly, the codec on the MP1 Disco board does not seem to work with two's-complement
	// data, despite the datasheet saying it does. So we use unsigned data. TODO: Figure out why
	using SampleT = uint32_t;

	static constexpr int SampleRate = 48000;
	static constexpr int SampleBits = 24;
	static constexpr int NumInChans = 2;
	static constexpr int NumOutChans = 2;
	static constexpr int NumDMAHalfTransfers = 2;

	using AudioInFrame = AudioFrame<SampleT, SampleBits, NumInChans>;
	using AudioInBuffer = std::array<AudioInFrame, BlockSize>;
	using AudioInBlock = std::array<AudioInBuffer, NumDMAHalfTransfers>;

	using AudioOutFrame = AudioFrame<SampleT, SampleBits, NumOutChans>;
	using AudioOutBuffer = std::array<AudioOutFrame, BlockSize>;
	using AudioOutBlock = std::array<AudioOutBuffer, NumDMAHalfTransfers>;
};
