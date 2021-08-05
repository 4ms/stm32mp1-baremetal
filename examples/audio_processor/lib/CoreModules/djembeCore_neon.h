#pragma once
#include "gcem.hpp"
#include "iirneon.hh"
#include "util/math.hh"
#include "util/math_tables.hh"

using namespace MathTools;

template<uint32_t SampleRate>
class DjembeCoreNeon {

public:
	static constexpr float SAMPLERATE = float{SampleRate};
	enum Params { Freq = 0, Gain = 1, Sharpness = 2, Strike = 3, Trigger = 4 };

	DjembeCoreNeon()
	{
		for (int i = 0; i < 2; i++) {
			noise[i] = 0;
		}
		for (int i = 0; i < 3; i++) {
			noise_hp[i] = 0.0f;
			noise_hp_lp[i] = 0.0f;
		}

		for (int i = 0; i < 2; i++) {
			fVecTrig[i] = 0.0f;
			iRec4[i] = 0;
		}

		for (int iir_group = 0; iir_group < 5; iir_group++) {
			iirs[iir_group].set_consts(&(iir_consts[iir_group * 4]));
		}

		for (int iir_group = 0; iir_group < 5; iir_group++) {
			float n = iir_group * 4;
			float __attribute__((aligned(16))) weights[4] = {
				1.f / ((n + 1) * (n + 1)),
				1.f / ((n + 2) * (n + 2)),
				1.f / ((n + 3) * (n + 3)),
				1.f / ((n + 4) * (n + 4)),
			};
			iirs[iir_group].set_outmix(weights);
		}

		// UI
		gainCV = 0.0f;
		gainKnob = 1.0f;
		strikeCV = 0.0f;
		strikeKnob = float(0.29999999999999999f);
		sharpCV = 0.0f;
		sharpnessKnob = 0.5f;
		trigIn = 0.0f;
		freqCV = 1.0f;
		freqKnob = float(60.0f);

		set_freq_coef(freqKnob * freqCV);

		paramsNeedUpdating = true;
	}

	void update()
	{
		if (freqNeedsUpdating || paramsNeedUpdating) {
			update_params();
			paramsNeedUpdating = false;
		}

		// 90ns:
		// StrikeModel:
		// PsuedoRandom:
		noise[0] = (1103515245 * noise[1]) + 12345;
		noise_hp[0] = (4.65661287e-10f * (float)noise[0]) -
					  (fSlowStrike1 * ((fSlowStrike3 * noise_hp[2]) + (fSlowStrike4 * noise_hp[1])));

		noise_hp_lp[0] = (fSlowStrike1 * (((fSlowStrike2 * noise_hp[0]) + (fSlowStrike5 * noise_hp[1])) +
										  (fSlowStrike2 * noise_hp[2]))) -
						 (fSlowStrike6 * ((fSlowStrike7 * noise_hp_lp[2]) + (fSlowStrike8 * noise_hp_lp[1])));
		// Trigger signal
		fVecTrig[0] = trigIn;
		// iRec4[0] is reset 1 on a rising edge, then goes +1 until reset again
		iRec4[0] = (((iRec4[1] + (iRec4[1] > 0)) * (fVecTrig[0] <= fVecTrig[1])) + (fVecTrig[0] > fVecTrig[1]));
		float fTemp0 = adEnvRate * iRec4[0];
		float adEnv = MathTools::max<float>(0.0f, MathTools::min<float>(fTemp0, (2.0f - fTemp0)));
		float noiseBurst = fSlowGainStrike * (noise_hp_lp[2] + (noise_hp_lp[0] + (2.0f * noise_hp_lp[1]))) * adEnv;

		noise[1] = noise[0];
		noise_hp[2] = noise_hp[1];
		noise_hp[1] = noise_hp[0];
		noise_hp_lp[2] = noise_hp_lp[1];
		noise_hp_lp[1] = noise_hp_lp[0];
		fVecTrig[1] = fVecTrig[0];
		iRec4[1] = iRec4[0];

		// IIRs:
		signalOut = 0.f;
		signalOut += iirs[0].calc_4iir(noiseBurst);
		signalOut += iirs[1].calc_4iir(noiseBurst);
		signalOut += iirs[2].calc_4iir(noiseBurst);
		signalOut += iirs[3].calc_4iir(noiseBurst);
		signalOut += iirs[4].calc_4iir(noiseBurst);
		signalOut *= 0.05f;
	}

	void update_params()
	{
		// 460ns to set_freq_coef
		// if strike:
		float strike0 = MathTools::min<float>(strikeCV + strikeKnob, 1.0f);
		float strike1 = MathTools::tan_close(fConst1 * ((15000.0f * strike0) + 500.0f));
		float strike2 = (1.0f / strike1);
		float strike3 = (((strike2 + 1.41421354f) / strike1) + 1.0f);

		// if gain || strike:
		fSlowGainStrike = MathTools::min<float>(gainCV + gainKnob, 1.0f) / strike3;

		// if strike:
		float strike4 = MathTools::tan_close(fConst1 * ((500.0f * strike0) + 40.0f));
		float strike5 = (1.0f / strike4);
		fSlowStrike1 = (1.0f / (((strike5 + 1.41421354f) / strike4) + 1.0f));
		float strike6 = (strike4 * strike4);
		fSlowStrike2 = (1.0f / strike6);
		fSlowStrike3 = (((strike5 + -1.41421354f) / strike4) + 1.0f);
		fSlowStrike4 = (2.0f * (1.0f - fSlowStrike2));
		fSlowStrike5 = (0.0f - (2.0f / strike6));
		fSlowStrike6 = (1.0f / strike3);
		fSlowStrike7 = (((strike2 + -1.41421354f) / strike1) + 1.0f);
		fSlowStrike8 = (2.0f * (1.0f - (1.0f / (strike1 * strike1))));

		// if sharp:
		adEnvRate =
			1.0f / MathTools::max<float>(1.0f, (fConst2 * MathTools::min<float>(sharpCV + sharpnessKnob, 1.0f)));

		// 640ns
		// if freq
		set_freq_coef(freqCV * freqKnob);
	}

	void update_freq()
	{
		set_freq_coef(freqCV * freqKnob);
	}

	void set_freq_coef(float freq)
	{
		// Coef: a1
		for (int iir_group = 0; iir_group < 5; iir_group++) {
			float __attribute__((aligned(16))) slows[4];
			for (int i = 0; i < 4; i++) {
				int n = i + iir_group * 4;
				slows[i] = iir_slow_consts[n] * MathTools::cos_close((fConst5 * (freq + 200.f * n)));
			}
			iirs[iir_group].set_slows(slows);
		}
	}

	void set_param(int const param_id, const float val)
	{
		switch (param_id) {
			case Freq:
				freqKnob = MathTools::map_value(val, 0.f, 1.f, 20.f, 500.f);
				freqNeedsUpdating = true;
				break;

			case Gain:
				gainKnob = val;
				paramsNeedUpdating = true;
				break;

			case Sharpness:
				sharpnessKnob = val;
				paramsNeedUpdating = true;
				break;

			case Strike:
				strikeKnob = val;
				paramsNeedUpdating = true;
				break;
		}
	}

	void set_samplerate(const float sr)
	{
		// Todo!
	}

	void set_input(const int input_id, const float val)
	{
		switch (input_id) {
			case Freq:
				freqCV = exp5Table.interp(MathTools::constrain(val, 0.f, 1.0f));
				freqNeedsUpdating = true;
				break;

			case Gain:
				gainCV = val;
				paramsNeedUpdating = true;
				break;

			case Sharpness:
				sharpCV = val;
				paramsNeedUpdating = true;
				break;

			case Strike:
				strikeCV = val;
				paramsNeedUpdating = true;
				break;

			case Trigger:
				trigIn = val > 0.f ? 1.f : 0.f;
				paramsNeedUpdating = true;
				break;
		}
	}

	float get_output(const int output_id) const
	{
		switch (output_id) {
			case 0:
				return signalOut;
				break;
		}
		return 0;
	}

private:
	bool paramsNeedUpdating = false;
	bool freqNeedsUpdating = false;
	float signalOut = 0;

	ParallelBPIIR iirs[5];

	float gainCV;
	float gainKnob;
	float strikeCV;
	float strikeKnob;
	int noise[2];
	float noise_hp[3];
	float noise_hp_lp[3];
	float sharpCV;
	float sharpnessKnob;
	float trigIn;
	float fVecTrig[2];
	int iRec4[2];
	float freqCV;
	float freqKnob;
	float fSlowGainStrike;
	float fSlowStrike1;
	float fSlowStrike2;
	float fSlowStrike3;
	float fSlowStrike4;
	float fSlowStrike5;
	float fSlowStrike6;
	float fSlowStrike7;
	float fSlowStrike8;
	float adEnvRate;
	static constexpr float fConst1 = (3.14159274f / SAMPLERATE);
	static constexpr float fConst2 = (0.00200000009f * SAMPLERATE);
	static constexpr float fConst3 = gcem::pow(0.00100000005f, (1.66666663f / SAMPLERATE));
	static constexpr float fConst4 = (0.0f - (2.0f * fConst3));
	static constexpr float fConst5 = (6.28318548f / SAMPLERATE);
	static constexpr float fConst6 = (fConst3 * fConst3);
	static constexpr float fConst7 = gcem::pow(0.00100000005f, (1.75438595f / SAMPLERATE));
	static constexpr float fConst8 = (0.0f - (2.0f * fConst7));
	static constexpr float fConst9 = (fConst7 * fConst7);
	static constexpr float fConst10 = gcem::pow(0.00100000005f, (1.85185182f / SAMPLERATE));
	static constexpr float fConst11 = (0.0f - (2.0f * fConst10));
	static constexpr float fConst12 = (fConst10 * fConst10);
	static constexpr float fConst13 = gcem::pow(0.00100000005f, (1.96078432f / SAMPLERATE));
	static constexpr float fConst14 = (0.0f - (2.0f * fConst13));
	static constexpr float fConst15 = (fConst13 * fConst13);
	static constexpr float fConst16 = gcem::pow(0.00100000005f, (2.08333325f / SAMPLERATE));
	static constexpr float fConst17 = (0.0f - (2.0f * fConst16));
	static constexpr float fConst18 = (fConst16 * fConst16);
	static constexpr float fConst19 = gcem::pow(0.00100000005f, (2.22222233f / SAMPLERATE));
	static constexpr float fConst20 = (0.0f - (2.0f * fConst19));
	static constexpr float fConst21 = (fConst19 * fConst19);
	static constexpr float fConst22 = gcem::pow(0.00100000005f, (2.38095236f / SAMPLERATE));
	static constexpr float fConst23 = (0.0f - (2.0f * fConst22));
	static constexpr float fConst24 = (fConst22 * fConst22);
	static constexpr float fConst25 = gcem::pow(0.00100000005f, (2.56410265f / SAMPLERATE));
	static constexpr float fConst26 = (0.0f - (2.0f * fConst25));
	static constexpr float fConst27 = (fConst25 * fConst25);
	static constexpr float fConst28 = gcem::pow(0.00100000005f, (2.77777767f / SAMPLERATE));
	static constexpr float fConst29 = (0.0f - (2.0f * fConst28));
	static constexpr float fConst30 = (fConst28 * fConst28);
	static constexpr float fConst31 = gcem::pow(0.00100000005f, (3.030303f / SAMPLERATE));
	static constexpr float fConst32 = (0.0f - (2.0f * fConst31));
	static constexpr float fConst33 = (fConst31 * fConst31);
	static constexpr float fConst34 = gcem::pow(0.00100000005f, (3.33333325f / SAMPLERATE));
	static constexpr float fConst35 = (0.0f - (2.0f * fConst34));
	static constexpr float fConst36 = (fConst34 * fConst34);
	static constexpr float fConst37 = gcem::pow(0.00100000005f, (3.70370364f / SAMPLERATE));
	static constexpr float fConst38 = (0.0f - (2.0f * fConst37));
	static constexpr float fConst39 = (fConst37 * fConst37);
	static constexpr float fConst40 = gcem::pow(0.00100000005f, (4.16666651f / SAMPLERATE));
	static constexpr float fConst41 = (0.0f - (2.0f * fConst40));
	static constexpr float fConst42 = (fConst40 * fConst40);
	static constexpr float fConst43 = gcem::pow(0.00100000005f, (4.76190472f / SAMPLERATE));
	static constexpr float fConst44 = (0.0f - (2.0f * fConst43));
	static constexpr float fConst45 = (fConst43 * fConst43);
	static constexpr float fConst46 = gcem::pow(0.00100000005f, (5.55555534f / SAMPLERATE));
	static constexpr float fConst47 = (0.0f - (2.0f * fConst46));
	static constexpr float fConst48 = (fConst46 * fConst46);
	static constexpr float fConst49 = gcem::pow(0.00100000005f, (6.66666651f / SAMPLERATE));
	static constexpr float fConst50 = (0.0f - (2.0f * fConst49));
	static constexpr float fConst51 = (fConst49 * fConst49);
	static constexpr float fConst52 = gcem::pow(0.00100000005f, (8.33333302f / SAMPLERATE));
	static constexpr float fConst53 = (0.0f - (2.0f * fConst52));
	static constexpr float fConst54 = (fConst52 * fConst52);
	static constexpr float fConst55 = gcem::pow(0.00100000005f, (11.1111107f / SAMPLERATE));
	static constexpr float fConst56 = (0.0f - (2.0f * fConst55));
	static constexpr float fConst57 = (fConst55 * fConst55);
	static constexpr float fConst58 = gcem::pow(0.00100000005f, (16.666666f / SAMPLERATE));
	static constexpr float fConst59 = (0.0f - (2.0f * fConst58));
	static constexpr float fConst60 = (fConst58 * fConst58);
	static constexpr float fConst61 = gcem::pow(0.00100000005f, (33.3333321f / SAMPLERATE));
	static constexpr float fConst62 = (0.0f - (2.0f * fConst61));
	static constexpr float fConst63 = (fConst61 * fConst61);
	static constexpr float iir_consts[20] = {
		fConst6,  fConst9,	fConst12, fConst15, fConst18, fConst21, fConst24, fConst27, fConst30, fConst33,
		fConst36, fConst39, fConst42, fConst45, fConst48, fConst51, fConst54, fConst57, fConst60, fConst63,
	};
	static constexpr float iir_slow_consts[20] = {
		fConst4,  fConst8,	fConst11, fConst14, fConst17, fConst20, fConst23, fConst26, fConst29, fConst32,
		fConst35, fConst38, fConst41, fConst44, fConst47, fConst50, fConst53, fConst56, fConst59, fConst62,
	};
};

/*
import("stdfaust.lib");
freqknob = hslider("v: djembe/freq", 60, 20, 500, 1);
freqcv = nentry("freqcv [CV:2]", 1, 1, 32, 0.001);
freqtotal = freqknob * freqcv;

trigbutton = button("Trigger");
gate = button("gate [CV:1]");

strikecv = nentry("v: jacks/strikecv [CV:3]", 0, 0, 1, 0.01);
sharpcv = nentry("v: jacks/shaprcv [CV:4]", 0, 0, 1, 0.01);
gaincv = nentry("v: jacks/gaincv [CV:5]", 0, 0, 1, 0.01);

strikepos = hslider("v: djembe/strike", 0.3, 0, 1, 0.01) + strikecv, 1 :min;
strikesharpness = hslider("v: djembe/sharpness", 0.5, 0, 1, 0.01) + sharpcv, 1 :min;
strikegain = hslider("v: djembe/gain", 1, 0, 1, 0.01) + gaincv, 1 :min;
process = trigbutton + gate : pm.djembe(freqtotal, strikepos, strikesharpness, strikegain);
*/
