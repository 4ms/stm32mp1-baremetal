#pragma once
#include "audio_codec_conf.hh"
#include "audio_stream_conf.hh"
#include "drivers/codec_CS42L51.hh"
#include "drivers/cycle_counter.hh"
#include "drivers/i2c.hh"
#include "drivers/cache.hh"

using AudioInBuffer = AudioStreamConf::AudioInBuffer;
using AudioOutBuffer = AudioStreamConf::AudioOutBuffer;
using AudioInBlock = AudioStreamConf::AudioInBlock;
using AudioOutBlock = AudioStreamConf::AudioOutBlock;

class AudioStream {
	mdrivlib::I2CPeriph i2c;
	mdrivlib::CodecCS42L51 codec;

	// These must be in a region of RAM that's not cached
	// -- OR else we have to invalidate the cache before we use incoming DMA data
	// and clean it after processing and writing to the outgoing DMA buffer
	// static inline __attribute__((section(".noncachable"))) AudioInBlock audio_in_dma_block;
	// static inline __attribute__((section(".noncachable"))) AudioOutBlock audio_out_dma_block;
	 static inline AudioInBlock audio_in_dma_block;
	 static inline AudioOutBlock audio_out_dma_block;

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
		for (auto &out_buffer : audio_out_dma_block) {
			for (auto &out : out_buffer) {
				out.chan[0] = 0xAAAAAA;
				out.chan[1] = 0x111111;
			}
		}

		mdrivlib::SystemCache::clean_dcache();	
	}

	void set_process_function(AudioProcessFunction &&process)
	{
		_process = std::move(process);
		codec.set_callbacks(
			[this] {
				audio_load = load_measurer.get_last_measurement_load_percent();
				load_measurer.start_measurement();
				mdrivlib::SystemCache::invalidate_dcache_by_range(audio_in_dma_block[1].data(), sizeof(AudioInBuffer));	
				_process(audio_in_dma_block[1], audio_out_dma_block[1]);
				mdrivlib::SystemCache::clean_dcache_by_range(audio_out_dma_block[1].data(), sizeof(AudioOutBuffer));	
				load_measurer.end_measurement();
			},
			[this] {
				audio_load = load_measurer.get_last_measurement_load_percent();
				load_measurer.start_measurement();
				mdrivlib::SystemCache::invalidate_dcache_by_range(audio_in_dma_block[0].data(), sizeof(AudioInBuffer));	
				_process(audio_in_dma_block[0], audio_out_dma_block[0]);
				mdrivlib::SystemCache::clean_dcache_by_range(audio_out_dma_block[0].data(), sizeof(AudioOutBuffer));	
				load_measurer.end_measurement();
			});
	}

	void start()
	{
		codec.start();
	}

	uint32_t get_load_measurement()
	{
		return audio_load;
	}

private:
	AudioProcessFunction _process;
	mdrivlib::CycleCounter load_measurer;
	uint32_t audio_load;
};
