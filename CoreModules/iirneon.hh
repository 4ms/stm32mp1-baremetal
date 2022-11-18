#if defined(CORE_CA7) && defined(__arm__) && !defined(CLANGD)
	#include <arm_neon.h>
#else
	#include "util/soft_neon.hh"
#endif

class ParallelBPIIR {
	// Todo: re-arranging might improve cache performance
public:
	float32x4_t __attribute__((aligned(16))) fSlow19202122;
	// float32x4_t __attribute__((aligned(16))) fRec0567_0;
	float32x4_t __attribute__((aligned(16))) fRec0567_1;
	float32x4_t __attribute__((aligned(16))) fRec0567_2;
	float32x4_t __attribute__((aligned(16))) fConst691215;
	float32x4_t __attribute__((aligned(16))) outmixWeights;

public:
	ParallelBPIIR()
	{
		fRec0567_1 = vdupq_n_f32(0.f);
		fRec0567_2 = vdupq_n_f32(0.f);
	}

	// float[4]: par_weights: weighting (0..1) for each parallel IIR filter output in the mix
	// float[4]: slows = control-dependant values (a1?)
	// float[4]: consts = samplerate-dependant values (a2?)
	ParallelBPIIR(const float slows[4], const float consts[4], const float par_weights[4])
	{
		fRec0567_1 = vdupq_n_f32(0.f);
		fRec0567_2 = vdupq_n_f32(0.f);
		set_slows(slows);
		set_outmix(par_weights);
		set_consts(consts);
	}

	~ParallelBPIIR() = default;

	void set_outmix(const float par_weights[4])
	{
		outmixWeights = vld1q_f32(par_weights);
	}
	void set_consts(const float consts[4])
	{
		fConst691215 = vld1q_f32(consts);
	}

	void set_slows(const float slows[4])
	{
		fSlow19202122 = vld1q_f32(slows);
	}

	float calc_4iir(float in)
	{
		float32x4_t __attribute__((aligned(16))) fRec0567_0;
		fRec0567_0 = vmulq_f32(fSlow19202122, fRec0567_1);
		fRec0567_0 = vmlaq_f32(fRec0567_0, fConst691215, fRec0567_2);

		float32x4_t __attribute__((aligned(16))) in_v = vdupq_n_f32(in);
		fRec0567_0 = vsubq_f32(in_v, fRec0567_0);

		float32x4_t __attribute__((aligned(16))) diff0567_02 = vsubq_f32(fRec0567_0, fRec0567_2);
		float32x4_t __attribute__((aligned(16))) outvec = vmulq_f32(diff0567_02, outmixWeights);
		float32x2_t __attribute__((aligned(16))) sum_tmp1 = vpadd_f32(vget_low_f32(outvec), vget_high_f32(outvec));

		// Todo:
		// We need to do this:
		// fRec0567_2 = fRec0567_1;
		// fRec0567_1 = fRec0567_0;
		// But it compiles to a no-op (same assembly if we remove those lines)
		// There is no vmovq_f32(), so we need another solution.
		// What we want is a vst1q_f32(fRec0567_1, {address where fRec0567_2 was loaded from});
		// e.g: VSTR Q0, [R0, #16]

		// This works, at the expense of a pipeline delay, (but only for one of the max/movs)
		// std::cout << "fSlow19 = " << fSlow19202122.v[0] << "\t";
		// std::cout << "fConst6 = " << fConst691215.v[0] << "\t";
		// std::cout << "fRec0[0] = " << fRec0567_0.v[0] << "\t";
		// std::cout << "fRec0[1] = " << fRec0567_1.v[0] << "\t";
		// std::cout << "fRec0[2] = " << fRec0567_2.v[0] << "\t";

		// fRec0567_2[0] = fRec0567_1[0];
		// fRec0567_2[1] = fRec0567_1[1];
		// fRec0567_2[2] = fRec0567_1[2];
		// fRec0567_2[3] = fRec0567_1[3];

		// fRec0567_1[0] = fRec0567_0[0];
		// fRec0567_1[1] = fRec0567_0[1];
		// fRec0567_1[2] = fRec0567_0[2];
		// fRec0567_1[3] = fRec0567_0[3];

		fRec0567_2 = vmaxq_f32(fRec0567_1, fRec0567_1);
		fRec0567_1 = vmaxq_f32(fRec0567_0, fRec0567_0);

		// Doesn't generate a store:
		// float rec_1[4] = {vgetq_lane_f32(fRec0567_1, 0),
		// 				  vgetq_lane_f32(fRec0567_1, 1),
		// 				  vgetq_lane_f32(fRec0567_1, 2),
		// 				  vgetq_lane_f32(fRec0567_1, 3)};
		// vst1q_f32(rec_1, fRec0567_2);
		// float rec_0[4] = {vgetq_lane_f32(fRec0567_0, 0),
		// 				  vgetq_lane_f32(fRec0567_0, 1),
		// 				  vgetq_lane_f32(fRec0567_0, 2),
		// 				  vgetq_lane_f32(fRec0567_0, 3)};
		// vst1q_f32(rec_0, fRec0567_1);

		// Won't compile:
		// asm("vmov %0, %1\n\t" : "=r"(fRec0567_2) : "r"(fRec0567_1));
		// asm("vmov %0, %1\n\t" : "=r"(fRec0567_1) : "r"(fRec0567_0));

		return vget_lane_f32(sum_tmp1, 0) + vget_lane_f32(sum_tmp1, 1);
	}
};
