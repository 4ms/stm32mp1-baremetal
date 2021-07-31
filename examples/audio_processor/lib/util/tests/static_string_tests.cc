#include "../static_string.hh"
#include "doctest.h"

TEST_CASE("Static String construction")
{
	SUBCASE("Default (empty) constructor")
	{
		StaticString<3> emptystr;
		CHECK(emptystr.data[0] == '\0');
	}

	SUBCASE("const char* construction")
	{
		StaticString<3> ccstr{"ABC"};
		CHECK(ccstr.data[0] == 'A');
		CHECK(ccstr.data[1] == 'B');
		CHECK(ccstr.data[2] == 'C');
		CHECK(ccstr.data[3] == '\0');

		const char DEF[] = "DEF";
		StaticString<3> str{DEF};
		CHECK(str.data[0] == 'D');
		CHECK(str.data[1] == 'E');
		CHECK(str.data[2] == 'F');
		CHECK(str.data[3] == '\0');

		SUBCASE("Extra capacity doesn't change string")
		{
			StaticString<40> str40{DEF};
			CHECK(str40.data[0] == 'D');
			CHECK(str40.data[1] == 'E');
			CHECK(str40.data[2] == 'F');
			CHECK(str40.data[3] == '\0');
		}
	}
}

TEST_CASE("Static string reading")
{
	SUBCASE(".cstr() returns actual data")
	{
		StaticString<3> str{"123"};
		CHECK(str.cstr()[0] == '1');
		CHECK(str.cstr()[1] == '2');
		CHECK(str.cstr()[2] == '3');
		CHECK(str.cstr()[3] == '\0');
	}

	SUBCASE("explicit const char * cast returns actual data")
	{
		StaticString<5> str{"123"};
		auto str2 = static_cast<const char *>(str);
		CHECK(str2[0] == '1');
		CHECK(str2[1] == '2');
		CHECK(str2[2] == '3');
		CHECK(str2[3] == '\0');
	}

	SUBCASE("implicit const char * cast returns actual data")
	{
		StaticString<5> str{"123"};
		CHECK_EQ(strcmp(str, "123"), 0);
	}
}

TEST_CASE("Static String comparison")
{
	SUBCASE("Equality operator")
	{
		StaticString<3> str1{"123"};
		StaticString<3> str2{"123"};
		StaticString<3> str3{"abc"};
		CHECK_EQ(str1 == str2, true);
		CHECK_EQ(str1 == str3, false);
	}

	SUBCASE("Equality with different capacity StaticStrings")
	{
		StaticString<3> str1{"123"};
		StaticString<2> str2{"12"};
		CHECK_EQ(str1 == str2, false);
		CHECK_EQ(str2 == str1, false);

		StaticString<3> str3{"abc"};
		StaticString<5> str4{"abc"};
		CHECK_EQ(str3 == str4, true);
		CHECK_EQ(str4 == str3, true);
	}
}
