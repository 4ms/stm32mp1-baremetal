#pragma once

struct Hex {
	unsigned x;
};

void printone(const unsigned char *str);
void printone(const char *str);
void printone(int value);
void printone(Hex value);

template<typename... Types>
void print(Types... args)
{
	(printone(args), ...);
}
