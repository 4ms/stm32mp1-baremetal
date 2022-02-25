// C-style debug pin outputs
// Assumes the pins have been init already

#include "stm32mp1xx_ll_gpio.h"

static inline void red1_on(void) { LL_GPIO_ResetOutputPin(GPIOZ, (1 << 6)); }
static inline void red1_off(void) { LL_GPIO_SetOutputPin(GPIOZ, (1 << 6)); }

static inline void red2_on(void) { LL_GPIO_ResetOutputPin(GPIOI, (1 << 8)); }
static inline void red2_off(void) { LL_GPIO_SetOutputPin(GPIOI, (1 << 8)); }
