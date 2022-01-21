#include <cstdarg>
#include <string>
#include <vector>
//
// #define RYML_SINGLE_HDR_DEFINE_NOW
// #include "ryml_all.hpp"

// namespace std {

// struct mutex {
//   void lock() {}
//   void unlock() {}
// };
// } // namespace std
// #define DOCTEST_CONFIG_NO_POSIX_SIGNALS
// #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
// #include "doctest.h"

// TEST_CASE("FF") { CHECK(false); }

volatile unsigned int *const UART0DR = (unsigned int *)0x101f1000;

void print_uart0(const char *s)
{
	while (*s != '\0') {
		*UART0DR = (unsigned int)(*s);
		s++;
	}
}

int main()
{
	print_uart0("Hello world\n");
	char yml_buf[] = "{foo: 1, bar: [2, 3], john: doe}";
	// ryml::Tree tree = ryml::parse_in_place(ryml::substr(yml_buf));

	// char buf0_[1];
	// char buf1_[2];
	// char buf2_[3];
	// char buf3_[4];
	// char buf4_[5];
	// ryml::substr buf0 = buf0_;
	// ryml::substr buf1 = buf1_;
	// ryml::substr buf2 = buf2_;
	// ryml::substr buf3 = buf3_;
	// ryml::substr buf4 = buf4_;
	// if (buf0.len == 0)
	//   print_uart0("ok\n");
	// if (buf1.len == 1)
	//   print_uart0("ok\n");
	// if (buf2.len == 2)
	//   print_uart0("ok\n");
	// if (buf3.len == 3)
	//   print_uart0("ok\n");
	// if (buf4.len == 4)
	//   print_uart0("ok\n");

	print_uart0(yml_buf);

	int *v = new int[600];
	for (int i = 0; i < 600; i++)
		v[i] = i;

	std::vector<float> vf(60);
	int i = 0;
	for (auto &f : vf) {
		f = v[i++] * 1.7f;
	}

	delete[] v;
	for (auto &f : vf) {
		if (f > 1.0f)
			print_uart0("X");
	}

	std::string s{"std::string\n"};
	print_uart0(s.c_str());

	print_uart0("Ryml did something\n");

	while (true)
		;

	return 0;
}
