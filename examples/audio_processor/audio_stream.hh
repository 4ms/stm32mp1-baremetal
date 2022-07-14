#pragma once
#include "audio_codec_conf.hh"
#include "audio_stream_conf.hh"
#include "drv/cache.hh"
#include "drv/codec_CS42L51.hh"
#include "drv/cycle_counter.hh"
#include "drv/i2c.hh"
#include "rcc_conf.hh"

using AudioInBuffer = AudioStreamConf::AudioInBuffer;
using AudioOutBuffer = AudioStreamConf::AudioOutBuffer;
using AudioInBlock = AudioStreamConf::AudioInBlock;
using AudioOutBlock = AudioStreamConf::AudioOutBlock;

class AudioStream {
	mdrivlib::I2CPeriph i2c;
	mdrivlib::CodecCS42L51 codec;

	// DMA buffers must be in a region of RAM that's not cached
	// -- OR -- we have to use cache maintenance routines
	// (That is, invalidate the cache before reading the incoming DMA buffer
	// and clean the cache after writing to the outgoing DMA buffer)
	static inline __attribute__((section(".noncachable"))) AudioInBlock audio_in_dma_block;
	static inline __attribute__((section(".noncachable"))) AudioOutBlock audio_out_dma_block;

public:
	using AudioProcessFunction = std::function<void(AudioInBuffer &, AudioOutBuffer &)>;

	AudioStream()
		: i2c{i2c_conf}
		, codec{i2c, sai_conf}
	{
		// Setup clocks needed for codec
		HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_conf);

		codec.init();
		codec.set_rx_buffers(audio_in_dma_block[0]);
		codec.set_tx_buffers(audio_out_dma_block[0]);
		load_measurer.init();
	}

	void set_process_function(AudioProcessFunction &process)
	{
		_process_func = process;
	}

	void start(AudioProcessFunction &process)
	{
		_process_func = process;
		codec.set_callbacks([this] { _process<1>(); }, [this] { _process<0>(); });
		codec.start();
	}

	uint32_t get_load_measurement()
	{
		return (uint32_t)(audio_load_smoothed * 100.f);
	}

	template<uint32_t buffer_half>
	void _process()
	{
		float last_audio_load = load_measurer.get_last_measurement_load_float();
		audio_load_smoothed = last_audio_load * kSmoothCoef + audio_load_smoothed * kinvSmoothCoef;
		load_measurer.start_measurement();
		_process_func(audio_in_dma_block[buffer_half], audio_out_dma_block[buffer_half]);
		load_measurer.end_measurement();
	}

private:
	AudioProcessFunction _process_func;
	mdrivlib::CycleCounter load_measurer;
	float audio_load_smoothed = 0.f;
	static constexpr float kSmoothCoef = 0.01; // 100 process blocks = 6400 samples: tau=0.133s
	static constexpr float kinvSmoothCoef = 1.f - kSmoothCoef;
};
