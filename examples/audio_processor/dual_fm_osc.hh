#pragma once
#include "util/math.hh"
#include "util/math_tables.hh"
#include "util/oscs.hh"

template<typename AudioStreamConf>
class DualFMOsc {
	RampOscillator<48000> ramp{1};
	TriangleOscillator<48000> modulator{40};
	SineOscillator<48000> carrier{250};

	using AudioInBuffer = typename AudioStreamConf::AudioInBuffer;
	using AudioOutBuffer = typename AudioStreamConf::AudioOutBuffer;

public:
	void process(AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer)
	{
		for (auto &out : out_buffer) {
			float freq_mult_1 = exp5Table.interp(ramp.process_float());
			modulator.set_frequency(40 * freq_mult_1);

			float freq_mult_2 = exp5Table.interp(modulator.process_float());
			carrier.set_frequency(20 * freq_mult_2);
			carrier.process();

			out.chan[0] = modulator.val() >> 8;
			out.chan[1] = carrier.val() >> 8;
		}
	};
};
