#pragma once
#include <cstring>

template<size_t CAPACITY>
struct StaticString {
	char data[CAPACITY + 1];

	static constexpr size_t capacity = CAPACITY;
	StaticString()
	{
		data[0] = '\0';
	}

	StaticString(const char *s)
	{
		size_t i = 0;
		while (*s && i < CAPACITY) {
			data[i] = *s;
			i++;
			s++;
		}
		data[i] = '\0';
	}

	const char *cstr() const
	{
		return data;
	}

	template<size_t CAP2>
	bool operator==(const StaticString<CAP2> &rhs) const
	{
		return (strcmp(data, rhs.data) == 0);
	}

	operator const char *() const
	{
		return data;
	}
};
