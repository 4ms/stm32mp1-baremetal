#pragma once
#include "lib/CoreModules/djembeCore_neon.h"
#include "util/countzip.hh"
#include "util/math.hh"
#include "util/math_tables.hh"
#include "util/oscs.hh"
#include <array>

template<typename AudioStreamConf>
class DjembeDual {
	static constexpr size_t NumDjembes = 2;
	using Djembe = DjembeCoreNeon<AudioStreamConf::SampleRate>;

	std::array<Djembe, NumDjembes> djembes;

	uint32_t trigger_counter[NumDjembes]{0, 0};
	const float trigger_period_sec[NumDjembes]{0.4f, 0.3f};

	using AudioInBuffer = typename AudioStreamConf::AudioInBuffer;
	using AudioOutBuffer = typename AudioStreamConf::AudioOutBuffer;

public:
	DjembeDual()
	{
		for (auto &djembe : djembes) {
			djembe.set_param(Djembe::Freq, 0.4f);
			djembe.set_param(Djembe::Gain, 0.9f);
			djembe.set_param(Djembe::Sharpness, 0.8f);
			djembe.set_param(Djembe::Strike, 0.75f);
		}
	}

	void process(AudioInBuffer &in_buffer, AudioOutBuffer &out_buffer)
	{
		for (auto &out : out_buffer) {
			for (auto [i, djembe] : countzip(djembes)) {
				trigger_counter[i]--;
				if (trigger_counter[i] == 1)
					djembe.set_input(Djembe::Trigger, 1.f);
				if (trigger_counter[i] == 0) {
					djembe.set_input(Djembe::Trigger, 0.f);
					trigger_counter[i] = trigger_period_sec[i] * Djembe::SAMPLERATE;
				}
			}

			out.chan[0] = AudioStreamConf::AudioOutFrame::scaleOutput(djembes[0].get_output(0));
			out.chan[1] = AudioStreamConf::AudioOutFrame::scaleOutput(djembes[1].get_output(0));
		}
	};
};
