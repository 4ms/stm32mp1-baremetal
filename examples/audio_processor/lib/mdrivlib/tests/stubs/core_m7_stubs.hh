#pragma once

#include <cstdint>

#ifndef __IM
#define __IM
#endif
#ifndef __IOM
#define __IOM
#endif
#ifndef __OM
#define __OM
#endif

static inline void __DMB() {
}
static inline void __DSB() {
}
static inline void __ISB() {
}

static inline void NVIC_DisableIRQ(uint32_t) {
}
static inline void NVIC_EnableIRQ(uint32_t) {
}
static inline void NVIC_SetPriority(uint32_t, uint32_t) {
}
