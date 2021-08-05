#pragma once
#include "daisysp.h"

// Taken from https://github.com/electro-smith/DaisyExamples/blob/master/seed/reverbsc/reverbsc.cpp
// and adapted for 4ms/stm32mp1-baremetal project
// Also added a sequence of notes to the osc, sharper attack on the AdEnv, and optional dry output for right channel

template<typename AudioStreamConf>
class DaisyReverbExample {
	daisysp::ReverbSc verb;
	daisysp::Oscillator osc;
	daisysp::AdEnv env;
	daisysp::Metro tick;

	const float scale[6] = {110.f, 130.82f, 146.84f, 329.64f, 392.f, 440.f};
	int note = 0;

	using AudioInBuffer = typename AudioStreamConf::AudioInBuffer;
	using AudioOutBuffer = typename AudioStreamConf::AudioOutBuffer;

public:
	void process(AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer)
	{
		for (auto &out : out_buffer) {
			if (tick.Process()) {
				env.Trigger();
				osc.SetFreq(scale[note]);
				note = (note + 1) % 6;
			}

			float sig = env.Process() * osc.Process();
			float out_left = 0.f, out_right = 0.f;
			verb.Process(sig, sig, &out_left, &out_right);

			out.chan[0] = AudioStreamConf::AudioOutFrame::scaleOutput(out_left);
			out.chan[1] = AudioStreamConf::AudioOutFrame::scaleOutput(out_right);

			// Uncomment this to have dry signal on right output
			// out.chan[1] = AudioStreamConf::AudioOutFrame::scaleOutput(sig);
		}
	}

	DaisyReverbExample()
	{
		// setup reverb
		verb.Init(AudioStreamConf::SampleRate);
		verb.SetFeedback(0.9f);
		verb.SetLpFreq(18000.0f);

		// setup metro
		tick.Init(1.f, AudioStreamConf::SampleRate);

		// setup envelope
		env.Init(AudioStreamConf::SampleRate);
		env.SetTime(daisysp::ADENV_SEG_ATTACK, .01f);
		env.SetTime(daisysp::ADENV_SEG_DECAY, .1f);
		env.SetMax(1.f);
		env.SetMin(0.f);
		env.SetCurve(0.f); // linear

		// setup oscillator
		osc.Init(AudioStreamConf::SampleRate);
		osc.SetFreq(440.f);
		osc.SetWaveform(daisysp::Oscillator::WAVE_TRI);
	}
};
