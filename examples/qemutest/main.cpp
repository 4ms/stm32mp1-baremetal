#include <cstdarg>
//
#define RYML_SINGLE_HDR_DEFINE_NOW
#include "ryml_all.hpp"

int main() {
	char yml_buf[] = "{foo: 1, bar: [2, 3], john: doe}";
	ryml::Tree tree = ryml::parse_in_place(ryml::substr(yml_buf));

	while (1) {
	}

	return 1;
}
