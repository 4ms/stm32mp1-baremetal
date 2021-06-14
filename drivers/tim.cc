#include "tim.hh"
#include "arch.hh"
#include "clocks.hh"
#include "periph.hh"
#include "system.hh"

namespace mdrivlib
{

void TIMPeriph::init_periph(TIM_TypeDef *TIM, uint32_t period, uint16_t prescaler, uint32_t clock_division) {
	Clocks::TIM::enable(TIM);
	LL_TIM_InitTypeDef timinit = {.Prescaler = prescaler,
								  .CounterMode = LL_TIM_COUNTERMODE_UP,
								  .Autoreload = period,
								  .ClockDivision = clock_division,
								  .RepetitionCounter = 0};
	LL_TIM_Init(TIM, &timinit);
	LL_TIM_DisableARRPreload(TIM);
}

void TIMPeriph::init_periph_once(TIM_TypeDef *TIM, uint32_t period, uint16_t prescaler, uint32_t clock_division) {
	static bool did_init[target::peripherals::TIM::NumPeriph]{false};

	uint8_t tim_i = target::peripherals::TIM::to_num(TIM);

	if (tim_i == 0 || did_init[tim_i - 1])
		return;

	init_periph(TIM, period, prescaler, clock_division);

	did_init[tim_i - 1] = true;
}
} // namespace mdrivlib
