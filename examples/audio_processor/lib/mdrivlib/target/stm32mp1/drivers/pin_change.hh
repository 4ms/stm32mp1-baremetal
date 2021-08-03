#include "arch.hh"
#include "drivers/exti.hh"
#include "interrupt.hh"
#include "pin.hh"
#include <functional>

namespace mdrivlib
{
struct DefaultPinChangeConf {
	static constexpr uint32_t pin = 0;
	static constexpr GPIO port = GPIO::A;
	static constexpr bool on_rising_edge = false;
	static constexpr bool on_falling_edge = false;
	static constexpr uint32_t priority1 = 3;
	static constexpr uint32_t priority2 = 3;
	static constexpr uint32_t core = 1;
};

template<typename ConfT>
requires std::derived_from<ConfT, DefaultPinChangeConf> class PinChangeInt {
public:
	PinChangeInt() = default;

	PinChangeInt(std::function<void(void)> &&func)
		: task_func{std::move(func)} {
		_init();
	}

	void init(std::function<void(void)> &&func) {
		task_func = std::move(func);
		_init();
	}

	void start() {
		EXTI_::Core1PinInterruptMask<ConfT::pin>::set();
	}

	void stop() {
		EXTI_::Core1PinInterruptMask<ConfT::pin>::clear();
	}

private:
	void _init() {
		auto port = ConfT::port == GPIO::A ? EXTI_::PortA
				  : ConfT::port == GPIO::B ? EXTI_::PortB
				  : ConfT::port == GPIO::C ? EXTI_::PortC
				  : ConfT::port == GPIO::D ? EXTI_::PortD
				  : ConfT::port == GPIO::E ? EXTI_::PortE
				  : ConfT::port == GPIO::F ? EXTI_::PortF
				  : ConfT::port == GPIO::G ? EXTI_::PortG
				  : ConfT::port == GPIO::H ? EXTI_::PortH
				  : ConfT::port == GPIO::I ? EXTI_::PortI
				  : ConfT::port == GPIO::J ? EXTI_::PortJ
										   : EXTI_::PortK;
		if constexpr (ConfT::pin == 1)
			EXTI_::Pin1::write(port);
		if constexpr (ConfT::pin == 2)
			EXTI_::Pin2::write(port);
		if constexpr (ConfT::pin == 3)
			EXTI_::Pin3::write(port);
		if constexpr (ConfT::pin == 4)
			EXTI_::Pin4::write(port);
		if constexpr (ConfT::pin == 5)
			EXTI_::Pin5::write(port);
		if constexpr (ConfT::pin == 6)
			EXTI_::Pin6::write(port);
		if constexpr (ConfT::pin == 7)
			EXTI_::Pin7::write(port);
		if constexpr (ConfT::pin == 8)
			EXTI_::Pin8::write(port);
		if constexpr (ConfT::pin == 9)
			EXTI_::Pin9::write(port);
		if constexpr (ConfT::pin == 10)
			EXTI_::Pin10::write(port);
		if constexpr (ConfT::pin == 11)
			EXTI_::Pin11::write(port);
		if constexpr (ConfT::pin == 12)
			EXTI_::Pin12::write(port);
		if constexpr (ConfT::pin == 13)
			EXTI_::Pin13::write(port);
		if constexpr (ConfT::pin == 14)
			EXTI_::Pin14::write(port);
		if constexpr (ConfT::pin == 15)
			EXTI_::Pin15::write(port);

		if constexpr (ConfT::on_rising_edge)
			EXTI_::PinRisingTrig<ConfT::pin>::set();
		else
			EXTI_::PinRisingTrig<ConfT::pin>::clear();

		if constexpr (ConfT::on_falling_edge)
			EXTI_::PinFallingTrig<ConfT::pin>::set();
		else
			EXTI_::PinFallingTrig<ConfT::pin>::clear();

		auto irqn = ConfT::pin == 0	 ? EXTI0_IRQn
				  : ConfT::pin == 1	 ? EXTI1_IRQn
				  : ConfT::pin == 2	 ? EXTI2_IRQn
				  : ConfT::pin == 3	 ? EXTI3_IRQn
				  : ConfT::pin == 4	 ? EXTI4_IRQn
				  : ConfT::pin == 5	 ? EXTI5_IRQn
				  : ConfT::pin == 6	 ? EXTI6_IRQn
				  : ConfT::pin == 7	 ? EXTI7_IRQn
				  : ConfT::pin == 8	 ? EXTI8_IRQn
				  : ConfT::pin == 9	 ? EXTI9_IRQn
				  : ConfT::pin == 10 ? EXTI10_IRQn
				  : ConfT::pin == 11 ? EXTI11_IRQn
				  : ConfT::pin == 12 ? EXTI12_IRQn
				  : ConfT::pin == 13 ? EXTI13_IRQn
				  : ConfT::pin == 14 ? EXTI14_IRQn
									 : EXTI15_IRQn;

		InterruptManager::registerISR(irqn, ConfT::priority1, ConfT::priority2, [&]() {
			if constexpr (ConfT::on_rising_edge) {
				if (EXTI_::PinRisingTrigPending<ConfT::pin>::read()) {
					EXTI_::PinRisingTrigPending<ConfT::pin>::set(); // clear on write
					task_func();
				}
			}
			if constexpr (ConfT::on_falling_edge) {
				if (EXTI_::PinFallingTrigPending<ConfT::pin>::read()) {
					EXTI_::PinFallingTrigPending<ConfT::pin>::set(); // clear on write
					task_func();
				}
			}
		});
	}

private:
	std::function<void(void)> task_func;
};
} // namespace mdrivlib
