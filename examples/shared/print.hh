#pragma once

void printone(const char *str);
void printone(int value);

template<typename... Types>
void print(Types... args)
{
	(printone(args), ...);
}
