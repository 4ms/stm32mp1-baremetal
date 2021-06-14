#include <cstdint>
struct SMPControl {
	static void notify(uint32_t) {
	}
	static void write(uint32_t) {
	}

	static uint32_t read() {
		return 0;
	}
};
