#pragma once
#include "drivers/colors.hh"
#include "drivers/frame_buffer_led.hh"
#include "drivers/i2c.hh"
#include "drivers/led_driver.hh"
#include "drivers/pca9685_led_driver.hh"
#include "drivers/pin.hh"
#include "drivers/rgbled.hh"
#include "drivers/stm32xx.h"

using DriverRgbLed = RgbLed<FrameBufferLED>;

class LedCtl {
public:
	LedCtl(ILedDmaDriver &led_driver)
		: led_driver_{led_driver}
	{}

	void start_dma_mode()
	{
		led_driver_.start();
		led_driver_.start_dma_mode();
	}

	// Todo: only update if glowing or fading
	void update()
	{
		freq[0].refresh();
		res[0].refresh();
		but[0].refresh();

		freq[1].refresh();
		res[1].refresh();
		but[1].refresh();

		mode[0].refresh();
		mode[1].refresh();
		mode[2].refresh();
		mode[3].refresh();
		mode[4].refresh();
	}

private:
	ILedDmaDriver &led_driver_;

	enum { Chip0 = 0, Chip1 = 1 };

public:
	DriverRgbLed freq[2] = {
		{
			{led_driver_.get_buf_addr(Chip1, 3)},
			{led_driver_.get_buf_addr(Chip1, 2)},
			{led_driver_.get_buf_addr(Chip1, 4)},
		},
		{
			{led_driver_.get_buf_addr(Chip1, 11)},
			{led_driver_.get_buf_addr(Chip1, 12)},
			{led_driver_.get_buf_addr(Chip1, 13)},
		},
	};

	DriverRgbLed res[2] = {
		{
			{led_driver_.get_buf_addr(Chip1, 5)},
			{led_driver_.get_buf_addr(Chip1, 6)},
			{led_driver_.get_buf_addr(Chip1, 7)},
		},
		{
			{led_driver_.get_buf_addr(Chip1, 8)},
			NoLED,
			{led_driver_.get_buf_addr(Chip1, 10)},
		},
	};

	DriverRgbLed but[2] = {
		{
			{led_driver_.get_buf_addr(Chip1, 0)},
			{led_driver_.get_buf_addr(Chip1, 9)},
			{led_driver_.get_buf_addr(Chip0, 15)},
		},
		{
			{led_driver_.get_buf_addr(Chip1, 1)},
			{led_driver_.get_buf_addr(Chip1, 14)},
			{led_driver_.get_buf_addr(Chip1, 15)},
		},
	};

	DriverRgbLed mode[5]{
		{
			{led_driver_.get_buf_addr(Chip0, 8)},
			{led_driver_.get_buf_addr(Chip0, 9)},
			{led_driver_.get_buf_addr(Chip0, 10)},
		},
		{
			{led_driver_.get_buf_addr(Chip0, 11)},
			{led_driver_.get_buf_addr(Chip0, 12)},
			{led_driver_.get_buf_addr(Chip0, 13)},
		},
		{
			{led_driver_.get_buf_addr(Chip0, 14)},
			{led_driver_.get_buf_addr(Chip0, 7)},
			{led_driver_.get_buf_addr(Chip0, 6)},
		},
		{
			{led_driver_.get_buf_addr(Chip0, 5)},
			{led_driver_.get_buf_addr(Chip0, 4)},
			{led_driver_.get_buf_addr(Chip0, 3)},
		},
		{
			{led_driver_.get_buf_addr(Chip0, 2)},
			{led_driver_.get_buf_addr(Chip0, 1)},
			{led_driver_.get_buf_addr(Chip0, 0)},
		},
	};

private:
	NoFrameBufferLED NoLED;
};

