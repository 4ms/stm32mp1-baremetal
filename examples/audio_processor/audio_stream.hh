#pragma once
#include "audio_codec_conf.hh"
#include "audio_stream_conf.hh"
#include "drivers/cache.hh"
#include "drivers/codec_CS42L51.hh"
#include "drivers/cycle_counter.hh"
#include "drivers/i2c.hh"

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
		codec.init();
		codec.set_rx_buffers(audio_in_dma_block[0]);
		codec.set_tx_buffers(audio_out_dma_block[0]);
		load_measurer.init();
	}

	void set_process_function(AudioProcessFunction &&process)
	{
		_process_func = std::move(process);
		codec.set_callbacks([this] { _process<1>(); }, [this] { _process<0>(); });
	}

	void start()
	{
		codec.start();
	}

	uint32_t get_load_measurement()
	{
		return audio_load;
	}

	template<uint32_t buffer_half>
	void _process()
	{
		audio_load = load_measurer.get_last_measurement_load_percent();
		load_measurer.start_measurement();
		_process_func(audio_in_dma_block[buffer_half], audio_out_dma_block[buffer_half]);
		load_measurer.end_measurement();
	}

private:
	AudioProcessFunction _process_func;
	mdrivlib::CycleCounter load_measurer;
	uint32_t audio_load;
};
