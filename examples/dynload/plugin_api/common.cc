#include "common.hh"
#include "stm32mp1xx_hal.h"

unsigned make_number(Channel chan) { return chan.num < 4 ? chan.num : 0; }
float perform_calculationA(float val) { return (val * 100.f) + (HAL_GetTick() % 70); }
unsigned perform_calculationB(float val) { return (val * 50.f) + (HAL_GetTick() % 30); }
