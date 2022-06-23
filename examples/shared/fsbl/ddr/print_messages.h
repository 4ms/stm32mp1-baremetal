#include "printf/printf.h"

#define debug(...) printf_(__VA_ARGS__)
#define pr_err(...) printf_(__VA_ARGS__)
#define panic(...)                                                                                                     \
	printf_(__VA_ARGS__);                                                                                              \
	while (1)                                                                                                          \
		;
