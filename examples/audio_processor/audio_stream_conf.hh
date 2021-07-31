#include "util/audio_frame.hh"

struct AudioStreamConf {

	// BlockSize: Number of Frames processed each time AudioStream::process() is called
	static constexpr int BlockSize = 64;
	using SampleT = int32_t;
	static constexpr int SampleBits = 24;
	static constexpr int NumInChans = 2;
	static constexpr int NumOutChans = 2;
	static constexpr int NumCodecs = 1;
	static constexpr int NumDMAHalfTransfers = 2;

	using AudioInFrame = AudioFrame<SampleT, SampleBits, NumInChans>;
	using AudioInBuffer = std::array<AudioInFrame, BlockSize>;
	using AudioInBlock = std::array<AudioInBuffer, NumDMAHalfTransfers>;

	using AudioOutFrame = AudioFrame<SampleT, SampleBits, NumOutChans>;
	using AudioOutBuffer = std::array<AudioOutFrame, BlockSize>;
	using AudioOutBlock = std::array<AudioOutBuffer, NumDMAHalfTransfers>;

	// CombinedAudioBlock is what gets processed in the AudioStream::process()
	struct CombinedAudioBlock {
		AudioInBuffer &in_codec;
		AudioOutBuffer &out_codec;
	};
};
