#pragma once
#include "util/math.hh"
#include "util/math_tables.hh"
#include "util/oscs.hh"

template<typename AudioStreamConf>
class DualFMOsc {
	TriangleOscillator<48000> modulator;
	TriangleOscillator<48000> carrier;

	using AudioInBuffer = typename AudioStreamConf::AudioInBuffer;
	using AudioOutBuffer = typename AudioStreamConf::AudioOutBuffer;

public:
	void process(AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer)
	{
		uint32_t freq_control_sum = 0;
		for (auto in : in_buffer)
			freq_control_sum += AudioStreamConf::AudioInFrame::scaleInput(in.chan[0]);
		float freq_control_avg = freq_control_sum / (float)AudioStreamConf::BlockSize;

		float freq_mult = exp5Table.interp(MathTools::constrain(freq_control_avg / 2.f + 0.5f, 0.f, 1.0f));
		modulator.set_frequency(80 * freq_mult);

		for (auto &out : out_buffer) {
			float mod_output = modulator.process_float();
			out.chan[0] = AudioStreamConf::AudioOutFrame::scaleOutput(mod_output);

			carrier.set_frequency(20 * mod_output);
			out.chan[1] = AudioStreamConf::AudioOutFrame::scaleOutput(carrier.process_float());
		}
	};
};
