#include "arch.hh"
#include "drivers/exti.hh"
#include "interrupt.hh"
#include "pin.hh"
#include <functional>

struct DefaultPinChangeConf {
	static constexpr uint32_t pin = 0;
	static constexpr GPIO port = GPIO::A;
	static constexpr bool on_rising_edge = false;
	static constexpr bool on_falling_edge = false;
	static constexpr uint32_t priority1 = 3;
	static constexpr uint32_t priority2 = 3;
};

// requires ConfT derives from DefaultPinChangeConf
template<typename ConfT>
class PinChangeInt {
public:
	PinChangeInt() {
	}

	PinChangeInt(std::function<void(void)> &&func)
		: task_func{std::move(func)} {
		_init();
	}

	void init(std::function<void(void)> &&func) {
		task_func = std::move(func);
		_init();
	}

	void start() {
		target::EXTI_::Core1PinInterruptMask<ConfT::pin>::set();
	}

	void stop() {
		target::EXTI_::Core1PinInterruptMask<ConfT::pin>::clear();
	}

private:
	void _init() {
		auto port = ConfT::port == GPIO::A ? target::EXTI_::PortA
				  : ConfT::port == GPIO::B ? target::EXTI_::PortB
				  : ConfT::port == GPIO::C ? target::EXTI_::PortC
				  : ConfT::port == GPIO::D ? target::EXTI_::PortD
				  : ConfT::port == GPIO::E ? target::EXTI_::PortE
				  : ConfT::port == GPIO::F ? target::EXTI_::PortF
				  : ConfT::port == GPIO::G ? target::EXTI_::PortG
				  : ConfT::port == GPIO::H ? target::EXTI_::PortH
				  : ConfT::port == GPIO::I ? target::EXTI_::PortI
				  : ConfT::port == GPIO::J ? target::EXTI_::PortJ
										   : target::EXTI_::PortK;
		if constexpr (ConfT::pin == 1)
			target::EXTI_::Pin1::write(port);
		if constexpr (ConfT::pin == 2)
			target::EXTI_::Pin2::write(port);
		if constexpr (ConfT::pin == 3)
			target::EXTI_::Pin3::write(port);
		if constexpr (ConfT::pin == 4)
			target::EXTI_::Pin4::write(port);
		if constexpr (ConfT::pin == 5)
			target::EXTI_::Pin5::write(port);
		if constexpr (ConfT::pin == 6)
			target::EXTI_::Pin6::write(port);
		if constexpr (ConfT::pin == 7)
			target::EXTI_::Pin7::write(port);
		if constexpr (ConfT::pin == 8)
			target::EXTI_::Pin8::write(port);
		if constexpr (ConfT::pin == 9)
			target::EXTI_::Pin9::write(port);
		if constexpr (ConfT::pin == 10)
			target::EXTI_::Pin10::write(port);
		if constexpr (ConfT::pin == 11)
			target::EXTI_::Pin11::write(port);
		if constexpr (ConfT::pin == 12)
			target::EXTI_::Pin12::write(port);
		if constexpr (ConfT::pin == 13)
			target::EXTI_::Pin13::write(port);
		if constexpr (ConfT::pin == 14)
			target::EXTI_::Pin14::write(port);
		if constexpr (ConfT::pin == 15)
			target::EXTI_::Pin15::write(port);

		if constexpr (ConfT::on_rising_edge)
			target::EXTI_::PinRisingTrig<ConfT::pin>::set();
		else
			target::EXTI_::PinRisingTrig<ConfT::pin>::clear();

		if constexpr (ConfT::on_falling_edge)
			target::EXTI_::PinFallingTrig<ConfT::pin>::set();
		else
			target::EXTI_::PinFallingTrig<ConfT::pin>::clear();

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
				if (target::EXTI_::PinRisingTrigPending<ConfT::pin>::read()) {
					target::EXTI_::PinRisingTrigPending<ConfT::pin>::set(); // clear on write
					task_func();
				}
			}
			if constexpr (ConfT::on_falling_edge) {
				if (target::EXTI_::PinFallingTrigPending<ConfT::pin>::read()) {
					target::EXTI_::PinFallingTrigPending<ConfT::pin>::set(); // clear on write
					task_func();
				}
			}
		});
	}

private:
	std::function<void(void)> task_func;
};
