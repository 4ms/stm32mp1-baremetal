#pragma once
#include "system.hh"
#include <functional>

namespace mdrivlib
{
struct TimekeeperConfig {
	TIM_TypeDef *TIMx;
	uint32_t period_ns;
	uint32_t priority1;
	uint32_t priority2;
};

class Timekeeper {
public:
	Timekeeper();
	Timekeeper(const TimekeeperConfig &config, std::function<void(void)> &&func);
	void init(const TimekeeperConfig &config, std::function<void(void)> &&func);

	void start();
	void stop();

private:
	TIM_TypeDef *timx;
	IRQn_Type irqn;
	bool is_running;
	std::function<void(void)> task_func;

	void _set_periph(TIM_TypeDef *timx);
	void _set_timing(uint32_t period_ns, uint32_t priority1 = 3, uint32_t priority2 = 3);
	void _register_task();
	void _init(const TimekeeperConfig &config);

	bool tim_update_IT_is_set() const;
	bool tim_update_IT_is_source() const;
	void tim_update_IT_clear() const;
};
} // namespace mdrivlib
