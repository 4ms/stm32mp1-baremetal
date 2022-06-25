#pragma once

#include "printf/printf.h"

// #define debug(...) printf_(__VA_ARGS__)
#define debug(...)
#define pr_err(...) printf_(__VA_ARGS__)
#define panic(...)                                                                                                     \
	do {                                                                                                               \
		printf_(__VA_ARGS__);                                                                                          \
		while (1)                                                                                                      \
			;                                                                                                          \
	} while (0);
