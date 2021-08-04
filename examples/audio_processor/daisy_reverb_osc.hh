#include "daisysp.h"

// Taken from https://github.com/electro-smith/DaisyExamples/blob/master/seed/reverbsc/reverbsc.cpp
// and adapted for 4ms/stm32mp1-baremetal project

template<typename AudioStreamConf>
class DaisyReverbExample {
	daisysp::ReverbSc verb;
	daisysp::Oscillator osc;
	daisysp::AdEnv env;
	daisysp::Metro tick;

	using AudioInBuffer = typename AudioStreamConf::AudioInBuffer;
	using AudioOutBuffer = typename AudioStreamConf::AudioOutBuffer;

public:
	void process(AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer)
	{
		for (auto &out : out_buffer) {
			if (tick.Process()) {
				env.Trigger();
			}

			float sig = env.Process() * osc.Process();
			float out_left, out_right;
			verb.Process(sig, sig, &out_left, &out_right);

			out.chan[0] = AudioStreamConf::AudioOutFrame::scaleOutput(out_left);
			out.chan[1] = AudioStreamConf::AudioOutFrame::scaleOutput(out_right);
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
		env.SetTime(daisysp::ADENV_SEG_ATTACK, .1f);
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
