#pragma once
#include "daisysp.h"

// Taken from https://github.com/electro-smith/DaisyExamples/blob/master/seed/harmonic_osc/harmonic_osc.cpp
// and adapted for 4ms/stm32mp1-baremetal project

template<typename AudioStreamConf>
class DaisyHarmonicExample {
	static constexpr int NumOscs = 16;

	daisysp::HarmonicOscillator<NumOscs> harm;
	daisysp::AdEnv env;

	const float scale[6] = {55.f, 65.41f, 73.42f, 82.41f, 98.f, 110.f};
	int note = 0;

	using AudioInBuffer = typename AudioStreamConf::AudioInBuffer;
	using AudioOutBuffer = typename AudioStreamConf::AudioOutBuffer;

public:
	void process(AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer)
	{
		for (auto &out : out_buffer) {
			// retrig env on EOC and go to next note
			if (!env.GetCurrentSegment()) {
				env.Trigger();
				harm.SetFreq(scale[note]);
				note = (note + 1) % 6;
			}

			// calculate the new amplitudes based on env value
			float center = env.Process();
			for (int i = 0; i < NumOscs; i++) {
				float dist = fabsf(center - (float)i) + 1.f;
				harm.SetSingleAmp(1.f / ((float)dist * 10.f), i);
			}

			out.chan[0] = AudioStreamConf::AudioOutFrame::scaleOutput(harm.Process());
			out.chan[1] = out.chan[0];
		}
	}

	DaisyHarmonicExample()
	{
		harm.Init(AudioStreamConf::SampleRate);
		harm.SetFirstHarmIdx(1);

		// init envelope
		env.Init(AudioStreamConf::SampleRate);
		env.SetTime(daisysp::ADENV_SEG_ATTACK, 0.05f);
		env.SetTime(daisysp::ADENV_SEG_DECAY, 0.35f);
		env.SetMin(0.0);
		env.SetMax(15.f);
		env.SetCurve(0); // linear
	}
};
