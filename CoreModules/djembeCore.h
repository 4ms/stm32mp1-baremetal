#pragma once
#include "gcem.hpp"
#include "util/math.hh"
#include "util/math_tables.hh"
using namespace MathTools;

template<uint32_t SAMPLERATE = 48000>
class DjembeCore {

public:
	enum Params { Freq = 0, Gain = 1, Sharpness = 2, Strike = 3, Trigger = 4 };

	DjembeCore()
	{
		IOTA = 0;

		// Todo: Combine these loops
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			noise[l0] = 0;
		}
		for (int l1 = 0; (l1 < 3); l1 = (l1 + 1)) {
			noise_hp[l1] = 0.0f;
		}
		for (int l2 = 0; (l2 < 3); l2 = (l2 + 1)) {
			noise_hp_lp[l2] = 0.0f;
		}
		for (int i = 0; i < 2; i++) {
			iRec4[i] = 0;
			fVecTrig[i] = float{0};
		}
		for (int l5 = 0; (l5 < 3); l5 = (l5 + 1)) {
			fRec0[l5] = 0.0f;
		}
		for (int l6 = 0; (l6 < 3); l6 = (l6 + 1)) {
			fRec5[l6] = 0.0f;
		}
		for (int l7 = 0; (l7 < 3); l7 = (l7 + 1)) {
			fRec6[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 3); l8 = (l8 + 1)) {
			fRec7[l8] = 0.0f;
		}
		for (int l9 = 0; (l9 < 3); l9 = (l9 + 1)) {
			fRec8[l9] = 0.0f;
		}
		for (int l10 = 0; (l10 < 3); l10 = (l10 + 1)) {
			fRec9[l10] = 0.0f;
		}
		for (int l11 = 0; (l11 < 3); l11 = (l11 + 1)) {
			fRec10[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 3); l12 = (l12 + 1)) {
			fRec11[l12] = 0.0f;
		}
		for (int l13 = 0; (l13 < 3); l13 = (l13 + 1)) {
			fRec12[l13] = 0.0f;
		}
		for (int l14 = 0; (l14 < 3); l14 = (l14 + 1)) {
			fRec13[l14] = 0.0f;
		}
		for (int l15 = 0; (l15 < 3); l15 = (l15 + 1)) {
			fRec14[l15] = 0.0f;
		}
		for (int l16 = 0; (l16 < 3); l16 = (l16 + 1)) {
			fRec15[l16] = 0.0f;
		}
		for (int l17 = 0; (l17 < 3); l17 = (l17 + 1)) {
			fRec16[l17] = 0.0f;
		}
		for (int l18 = 0; (l18 < 3); l18 = (l18 + 1)) {
			fRec17[l18] = 0.0f;
		}
		for (int l19 = 0; (l19 < 3); l19 = (l19 + 1)) {
			fRec18[l19] = 0.0f;
		}
		for (int l20 = 0; (l20 < 3); l20 = (l20 + 1)) {
			fRec19[l20] = 0.0f;
		}
		for (int l21 = 0; (l21 < 3); l21 = (l21 + 1)) {
			fRec20[l21] = 0.0f;
		}
		for (int l22 = 0; (l22 < 3); l22 = (l22 + 1)) {
			fRec21[l22] = 0.0f;
		}
		for (int l23 = 0; (l23 < 3); l23 = (l23 + 1)) {
			fRec22[l23] = 0.0f;
		}
		for (int l24 = 0; (l24 < 3); l24 = (l24 + 1)) {
			fRec23[l24] = 0.0f;
		}

		// UI
		gainCV = float(0.0f);
		gainKnob = float(1.0f);
		strikeCV = float(0.0f);
		strikeKnob = float(0.29999999999999999f);
		sharpCV = float(0.0f);
		sharpnessKnob = float(0.5f);
		trigIn = float(0.0f);
		freqCV = float(1.0f);
		freqKnob = float(60.0f);
		paramsNeedUpdating = true;
	}

	void update()
	{
		if (paramsNeedUpdating)
			update_params();
		paramsNeedUpdating = false;

		// StrikeModel:
		noise[0] = (1103515245 * noise[1]) + 12345;
		noise_hp[0] =
			(4.65661287e-10f * float(noise[0])) - (fSlow7 * ((fSlow10 * noise_hp[2]) + (fSlow11 * noise_hp[1])));

		noise_hp_lp[0] = (fSlow7 * (((fSlow9 * noise_hp[0]) + (fSlow12 * noise_hp[1])) + (fSlow9 * noise_hp[2]))) -
						 (fSlow13 * ((fSlow14 * noise_hp_lp[2]) + (fSlow15 * noise_hp_lp[1])));
		fVecTrig[0] = slowTrig;
		iRec4[0] = (((iRec4[1] + (iRec4[1] > 0)) * (slowTrig <= fVecTrig[1])) + (slowTrig > fVecTrig[1]));
		float fTemp0 = (adEnvRate * float(iRec4[0]));
		float adEnv = MathTools::max<float>(0.0f, MathTools::min<float>(fTemp0, (2.0f - fTemp0)));
		float noiseBurst = fSlow4 * (noise_hp_lp[2] + (noise_hp_lp[0] + (2.0f * noise_hp_lp[1]))) * adEnv;

		noise[1] = noise[0];
		noise_hp[2] = noise_hp[1];
		noise_hp[1] = noise_hp[0];
		noise_hp_lp[2] = noise_hp_lp[1];
		noise_hp_lp[1] = noise_hp_lp[0];
		fVecTrig[1] = fVecTrig[0];
		iRec4[1] = iRec4[0];

		fRec0[0] = (noiseBurst - ((fSlow19 * fRec0[1]) + (fConst6 * fRec0[2])));
		fRec5[0] = (noiseBurst - ((fSlow20 * fRec5[1]) + (fConst9 * fRec5[2])));
		fRec6[0] = (noiseBurst - ((fSlow21 * fRec6[1]) + (fConst12 * fRec6[2])));
		fRec7[0] = (noiseBurst - ((fSlow22 * fRec7[1]) + (fConst15 * fRec7[2])));
		fRec8[0] = (noiseBurst - ((fSlow23 * fRec8[1]) + (fConst18 * fRec8[2])));
		fRec9[0] = (noiseBurst - ((fSlow24 * fRec9[1]) + (fConst21 * fRec9[2])));
		fRec10[0] = (noiseBurst - ((fSlow25 * fRec10[1]) + (fConst24 * fRec10[2])));
		fRec11[0] = (noiseBurst - ((fSlow26 * fRec11[1]) + (fConst27 * fRec11[2])));
		fRec12[0] = (noiseBurst - ((fSlow27 * fRec12[1]) + (fConst30 * fRec12[2])));
		fRec13[0] = (noiseBurst - ((fSlow28 * fRec13[1]) + (fConst33 * fRec13[2])));
		fRec14[0] = (noiseBurst - ((fSlow29 * fRec14[1]) + (fConst36 * fRec14[2])));
		fRec15[0] = (noiseBurst - ((fSlow30 * fRec15[1]) + (fConst39 * fRec15[2])));
		fRec16[0] = (noiseBurst - ((fSlow31 * fRec16[1]) + (fConst42 * fRec16[2])));
		fRec17[0] = (noiseBurst - ((fSlow32 * fRec17[1]) + (fConst45 * fRec17[2])));
		fRec18[0] = (noiseBurst - ((fSlow33 * fRec18[1]) + (fConst48 * fRec18[2])));
		fRec19[0] = (noiseBurst - ((fSlow34 * fRec19[1]) + (fConst51 * fRec19[2])));
		fRec20[0] = (noiseBurst - ((fSlow35 * fRec20[1]) + (fConst54 * fRec20[2])));
		fRec21[0] = (noiseBurst - ((fSlow36 * fRec21[1]) + (fConst57 * fRec21[2])));
		fRec22[0] = (noiseBurst - ((fSlow37 * fRec22[1]) + (fConst60 * fRec22[2])));
		fRec23[0] = (noiseBurst - ((fSlow38 * fRec23[1]) + (fConst63 * fRec23[2])));

		signalOut = 0.f;
		signalOut += 1.0f * (fRec0[0] - fRec0[2]);
		signalOut += 0.25f * (fRec5[0] - fRec5[2]);
		signalOut += 0.111111112f * (fRec6[0] - fRec6[2]);
		signalOut += 0.0625f * (fRec7[0] - fRec7[2]);
		signalOut += 0.0399999991f * (fRec8[0] - fRec8[2]);
		signalOut += 0.027777778f * (fRec9[0] - fRec9[2]);
		signalOut += 0.0204081628f * (fRec10[0] - fRec10[2]);
		signalOut += 0.015625f * (fRec11[0] - fRec11[2]);
		signalOut += 0.0123456791f * (fRec12[0] - fRec12[2]);
		signalOut += 0.00999999978f * (fRec13[0] - fRec13[2]);
		signalOut += 0.00826446246f * (fRec14[0] - fRec14[2]);
		signalOut += 0.0069444445f * (fRec15[0] - fRec15[2]);
		signalOut += 0.00591715984f * (fRec16[0] - fRec16[2]);
		signalOut += 0.00510204071f * (fRec17[0] - fRec17[2]);
		signalOut += 0.00444444455f * (fRec18[0] - fRec18[2]);
		signalOut += 0.00390625f * (fRec19[0] - fRec19[2]);
		signalOut += 0.00346020772f * (fRec20[0] - fRec20[2]);
		signalOut += 0.00308641978f * (fRec21[0] - fRec21[2]);
		signalOut += 0.00277008303f * (fRec22[0] - fRec22[2]);
		signalOut += 0.00249999994f * (fRec23[0] - fRec23[2]);
		signalOut *= 0.05f;

		fRec0[2] = fRec0[1];
		fRec0[1] = fRec0[0];
		fRec5[2] = fRec5[1];
		fRec5[1] = fRec5[0];
		fRec6[2] = fRec6[1];
		fRec6[1] = fRec6[0];
		fRec7[2] = fRec7[1];
		fRec7[1] = fRec7[0];
		fRec8[2] = fRec8[1];
		fRec8[1] = fRec8[0];
		fRec9[2] = fRec9[1];
		fRec9[1] = fRec9[0];
		fRec10[2] = fRec10[1];
		fRec10[1] = fRec10[0];
		fRec11[2] = fRec11[1];
		fRec11[1] = fRec11[0];
		fRec12[2] = fRec12[1];
		fRec12[1] = fRec12[0];
		fRec13[2] = fRec13[1];
		fRec13[1] = fRec13[0];
		fRec14[2] = fRec14[1];
		fRec14[1] = fRec14[0];
		fRec15[2] = fRec15[1];
		fRec15[1] = fRec15[0];
		fRec16[2] = fRec16[1];
		fRec16[1] = fRec16[0];
		fRec17[2] = fRec17[1];
		fRec17[1] = fRec17[0];
		fRec18[2] = fRec18[1];
		fRec18[1] = fRec18[0];
		fRec19[2] = fRec19[1];
		fRec19[1] = fRec19[0];
		fRec20[2] = fRec20[1];
		fRec20[1] = fRec20[0];
		fRec21[2] = fRec21[1];
		fRec21[1] = fRec21[0];
		fRec22[2] = fRec22[1];
		fRec22[1] = fRec22[0];
		fRec23[2] = fRec23[1];
		fRec23[1] = fRec23[0];
	}

	void update_params()
	{
		strike0 = MathTools::min<float>((float(strikeCV) + float(strikeKnob)), 1.0f);
		strike1 = MathTools::tan_close(fConst1 * ((15000.0f * strike0) + 500.0f));
		strike2 = (1.0f / strike1);
		strike3 = (((strike2 + 1.41421354f) / strike1) + 1.0f);
		fSlow4 = (MathTools::min<float>((float(gainCV) + float(gainKnob)), 1.0f) / strike3);
		fSlow5 = MathTools::tan_close(fConst1 * ((500.0f * strike0) + 40.0f));
		fSlow6 = (1.0f / fSlow5);
		fSlow7 = (1.0f / (((fSlow6 + 1.41421354f) / fSlow5) + 1.0f));
		fSlow8 = (fSlow5 * fSlow5);
		fSlow9 = (1.0f / fSlow8);
		fSlow10 = (((fSlow6 + -1.41421354f) / fSlow5) + 1.0f);
		fSlow11 = (2.0f * (1.0f - fSlow9));
		fSlow12 = (0.0f - (2.0f / fSlow8));
		fSlow13 = (1.0f / strike3);
		fSlow14 = (((strike2 + -1.41421354f) / strike1) + 1.0f);
		fSlow15 = (2.0f * (1.0f - (1.0f / (strike1 * strike1))));
		adEnvRate =
			(1.0f / MathTools::max<float>(
						1.0f, (fConst2 * MathTools::min<float>((float(sharpCV) + float(sharpnessKnob)), 1.0f))));
		slowTrig = trigIn > 0.f ? 1.f : 0.f;
		slowFreq = (float(freqCV) * float(freqKnob));

		// Coef: a1
		fSlow19 = (fConst4 * MathTools::cos_close((fConst5 * slowFreq)));
		fSlow20 = (fConst8 * MathTools::cos_close((fConst5 * (slowFreq + 200.0f))));
		fSlow21 = (fConst11 * MathTools::cos_close((fConst5 * (slowFreq + 400.0f))));
		fSlow22 = (fConst14 * MathTools::cos_close((fConst5 * (slowFreq + 600.0f))));
		fSlow23 = (fConst17 * MathTools::cos_close((fConst5 * (slowFreq + 800.0f))));
		fSlow24 = (fConst20 * MathTools::cos_close((fConst5 * (slowFreq + 1000.0f))));
		fSlow25 = (fConst23 * MathTools::cos_close((fConst5 * (slowFreq + 1200.0f))));
		fSlow26 = (fConst26 * MathTools::cos_close((fConst5 * (slowFreq + 1400.0f))));
		fSlow27 = (fConst29 * MathTools::cos_close((fConst5 * (slowFreq + 1600.0f))));
		fSlow28 = (fConst32 * MathTools::cos_close((fConst5 * (slowFreq + 1800.0f))));
		fSlow29 = (fConst35 * MathTools::cos_close((fConst5 * (slowFreq + 2000.0f))));
		fSlow30 = (fConst38 * MathTools::cos_close((fConst5 * (slowFreq + 2200.0f))));
		fSlow31 = (fConst41 * MathTools::cos_close((fConst5 * (slowFreq + 2400.0f))));
		fSlow32 = (fConst44 * MathTools::cos_close((fConst5 * (slowFreq + 2600.0f))));
		fSlow33 = (fConst47 * MathTools::cos_close((fConst5 * (slowFreq + 2800.0f))));
		fSlow34 = (fConst50 * MathTools::cos_close((fConst5 * (slowFreq + 3000.0f))));
		fSlow35 = (fConst53 * MathTools::cos_close((fConst5 * (slowFreq + 3200.0f))));
		fSlow36 = (fConst56 * MathTools::cos_close((fConst5 * (slowFreq + 3400.0f))));
		fSlow37 = (fConst59 * MathTools::cos_close((fConst5 * (slowFreq + 3600.0f))));
		fSlow38 = (fConst62 * MathTools::cos_close((fConst5 * (slowFreq + 3800.0f))));
	}

	void set_param(int const param_id, const float val)
	{
		switch (param_id) {
			case Freq:
				freqKnob = MathTools::map_value(val, 0.f, 1.f, 20.f, 500.f);
				paramsNeedUpdating = true;
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
				paramsNeedUpdating = true;
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
				trigIn = val;
				paramsNeedUpdating = true;
				break;
		}
	}

	float get_output(const int output_id) const
	{
		switch (output_id) {
			case 0:
				// return noise_hp[0];
				return signalOut;
				break;
		}
		return 0;
	}

private:
	bool paramsNeedUpdating = false;
	float signalOut = 0;

	float IOTA;

	float gainCV;
	float gainKnob;
	// float fConst1;
	float strikeCV;
	float strikeKnob;
	int noise[2];
	float noise_hp[3];
	float noise_hp_lp[3];
	// float fConst2;
	float sharpCV;
	float sharpnessKnob;
	float trigIn;
	float fVecTrig[2];
	int iRec4[2];
	// float fConst4;
	// float fConst5;
	float freqCV;
	float freqKnob;
	// float fConst6;
	float fRec0[3];
	// float fConst8;
	// float fConst9;
	float fRec5[3];
	// float fConst11;
	// float fConst12;
	float fRec6[3];
	// float fConst14;
	// float fConst15;
	float fRec7[3];
	// float fConst17;
	// float fConst18;
	float fRec8[3];
	// float fConst20;
	// float fConst21;
	float fRec9[3];
	// float fConst23;
	// float fConst24;
	float fRec10[3];
	// float fConst26;
	// float fConst27;
	float fRec11[3];
	// float fConst29;
	// float fConst30;
	float fRec12[3];
	// float fConst32;
	// float fConst33;
	float fRec13[3];
	// float fConst35;
	// float fConst36;
	float fRec14[3];
	// float fConst38;
	// float fConst39;
	float fRec15[3];
	// float fConst41;
	// float fConst42;
	float fRec16[3];
	// float fConst44;
	// float fConst45;
	float fRec17[3];
	// float fConst47;
	// float fConst48;
	float fRec18[3];
	// float fConst50;
	// float fConst51;
	float fRec19[3];
	// float fConst53;
	// float fConst54;
	float fRec20[3];
	// float fConst56;
	// float fConst57;
	float fRec21[3];
	// float fConst59;
	// float fConst60;
	float fRec22[3];
	// float fConst62;
	// float fConst63;
	float fRec23[3];
	float strike0;
	float strike1;
	float strike2;
	float strike3;
	float fSlow4;
	float fSlow5;
	float fSlow6;
	float fSlow7;
	float fSlow8;
	float fSlow9;
	float fSlow10;
	float fSlow11;
	float fSlow12;
	float fSlow13;
	float fSlow14;
	float fSlow15;
	float adEnvRate;
	float slowTrig;
	float slowFreq;
	float fSlow19;
	float fSlow20;
	float fSlow21;
	float fSlow22;
	float fSlow23;
	float fSlow24;
	float fSlow25;
	float fSlow26;
	float fSlow27;
	float fSlow28;
	float fSlow29;
	float fSlow30;
	float fSlow31;
	float fSlow32;
	float fSlow33;
	float fSlow34;
	float fSlow35;
	float fSlow36;
	float fSlow37;
	float fSlow38;
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
