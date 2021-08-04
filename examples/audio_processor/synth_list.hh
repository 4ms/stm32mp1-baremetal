#include "audio_stream_conf.hh"
#include <array>
#include <functional>

using AudioInBuffer = AudioStreamConf::AudioInBuffer;
using AudioOutBuffer = AudioStreamConf::AudioOutBuffer;

struct SynthList {
	using AudioProcessFunction = std::function<void(AudioInBuffer &, AudioOutBuffer &)>;
	std::array<AudioProcessFunction, 6> synth_list;
};
