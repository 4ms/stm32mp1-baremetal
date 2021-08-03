#pragma once
#include <string>

struct ReplaceString {
	std::string str;

	ReplaceString(std::string initval)
		: str{initval}
	{}

	ReplaceString &replace_all(const std::string str_find, const std::string str_replace)
	{
		size_t index = 0;
		while (true) {
			index = str.find(str_find, index);
			if (index == std::string::npos) {
				break;
			}
			str.replace(index, str_find.length(), str_replace);
			index += str_find.length();
		}
		return *this;
	}
};
