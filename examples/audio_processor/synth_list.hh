#include "util/zip.hh"
#include <array>
#include <functional>

#include "audio_stream_conf.hh"
#include "daisy_harm_osc.hh"
#include "daisy_reverb_osc.hh"
#include "djembe_example.hh"
#include "dual_fm_osc.hh"

using AudioInBuffer = AudioStreamConf::AudioInBuffer;
using AudioOutBuffer = AudioStreamConf::AudioOutBuffer;
using AudioProcessFunction = std::function<void(AudioInBuffer &, AudioOutBuffer &)>;

struct SynthList {
	enum Synths : int {
		Passthrough,
		DualFMOscillators,
		MonoTriOsc,
		HarmonicOsc,
		ReverbOsc,
		DualDjembe,

		NumSynths,
	};
	Synths current_synth = DualFMOscillators;

	std::array<const char[20], NumSynths> name{
		"Passthrough",
		"Dual FM Osc",
		"Sine and Tri Osc",
		"Harmonic Osc",
		"Reverb Osc",
		"Eight Djembes",
	};

	DualFMOsc<AudioStreamConf> dual_fm_osc;

	TriangleOscillator<AudioStreamConf::SampleRate> tri_osc{400}; // 400Hz
	SineOscillator<AudioStreamConf::SampleRate> sine_osc{600};	  // 600Hz

	DaisyHarmonicExample<AudioStreamConf> harmonic_sequencer;

	DaisyReverbExample<AudioStreamConf> *reverb_example;

	DjembeExample<AudioStreamConf, 8> djembes;

	std::array<AudioProcessFunction, NumSynths> process_func;

	SynthList()
	{
		process_func[Passthrough] = [this](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
			for (auto [in, out] : zip(in_buffer, out_buffer)) {
				out.chan[0] = in.chan[0];
				out.chan[1] = in.chan[1];
			}
		};

		process_func[DualFMOscillators] = [this](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
			dual_fm_osc.process(in_buffer, out_buffer);
		};

		process_func[MonoTriOsc] = [this](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
			for (auto &out : out_buffer) {
				out.chan[0] = tri_osc.process() >> 8;
				out.chan[1] = sine_osc.process() >> 8;
			}
		};

		process_func[HarmonicOsc] = [this](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
			harmonic_sequencer.process(in_buffer, out_buffer);
		};

		// Put reverb object on the heap because it's large (~385kB) and our linker script sets aside ~256MB of heap
		reverb_example = new DaisyReverbExample<AudioStreamConf>;
		process_func[ReverbOsc] = [this](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
			reverb_example->process(in_buffer, out_buffer);
		};

		process_func[DualDjembe] = [this](AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer) {
			djembes.process(in_buffer, out_buffer);
		};
	}

	void next_synth()
	{
		int cur = static_cast<int>(current_synth);
		int next = cur + 1;
		if (next >= static_cast<int>(NumSynths))
			next = 0;
		current_synth = static_cast<Synths>(next);
	}

	int current()
	{
		return static_cast<int>(current_synth);
	}
};
