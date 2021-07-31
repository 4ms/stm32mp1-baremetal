#pragma once
#include <cstdint>

namespace mdrivlib
{
// Todo: template param for bit depth
class FrameBufferLED {
	const uint8_t LEDBitDepth = 8;
	const uint8_t DriverBitDepth = 12;

public:
	FrameBufferLED(uint32_t *frame_buffer_element)
		: buffer(frame_buffer_element) {
	}

	void set(uint32_t val) const {
		*buffer = val << (16 + (DriverBitDepth - LEDBitDepth));
	}

private:
	uint32_t *buffer{nullptr};

protected:
	FrameBufferLED() = default;
};

class NoFrameBufferLED : public FrameBufferLED {
public:
	NoFrameBufferLED() = default;
	void set(uint32_t val) const {
	}
};
} // namespace mdrivlib
