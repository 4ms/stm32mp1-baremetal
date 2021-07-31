#pragma once
#include "audio_codec_conf.hh"
#include "audio_stream_conf.hh"
#include "drivers/codec_CS42L51.hh"
#include "drivers/i2c.hh"

using mdrivlib::CodecCS42L51;
using mdrivlib::I2CPeriph;
using AudioInBuffer = AudioStreamConf::AudioInBuffer;
using AudioOutBuffer = AudioStreamConf::AudioOutBuffer;
using AudioInBlock = AudioStreamConf::AudioInBlock;
using AudioOutBlock = AudioStreamConf::AudioOutBlock;

class AudioStream {
	I2CPeriph i2c;
	CodecCS42L51 codec;

	// These must be in a region of RAM that's not cached -- OR -- we have to clean/invalidate the cache before and
	// after processing DMA data
	static inline __attribute__((section(".sysram"))) AudioInBlock audio_in_dma_block;
	static inline __attribute__((section(".sysram"))) AudioOutBlock audio_out_dma_block;

public:
	using AudioProcessFunction = std::function<void(AudioInBuffer &, AudioOutBuffer &)>;

	AudioStream()
		: i2c{i2c_conf}
		, codec{i2c, sai_conf}
	{
		codec.init();
		codec.set_rx_buffers(audio_in_dma_block);
		codec.set_tx_buffers(audio_out_dma_block);
	}

	void set_process_function(AudioProcessFunction &&process)
	{
		_process = std::move(process);
		codec.set_callbacks([this] { _process(audio_in_dma_block[0], audio_out_dma_block[0]); },
							[this] { _process(audio_in_dma_block[1], audio_out_dma_block[1]); });
	}

	void start()
	{
		codec.start();
	}

private:
	AudioProcessFunction _process;
};
