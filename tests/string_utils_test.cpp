// Tests for string utils
// Todo: Write more tests

#include "test_utils.h"

static void expect_nullstr(const noex::string& str) {
    EXPECT_TRUE(str.empty());
    EXPECT_EQ(str.size(), 0);
    EXPECT_EQ(str.data(), nullptr);
    EXPECT_STREQ(str.c_str(), "");
    EXPECT_EQ(str, "");
}

static void expect_tuwstr(const char* expected, const noex::string& actual) {
    size_t size = strlen(expected);
    EXPECT_EQ(actual.size(), size);
    EXPECT_STREQ(actual.data(), expected);
    EXPECT_STREQ(actual.c_str(), expected);
    EXPECT_EQ(actual, expected);
}

// TestGetLastLine
TEST(StringTest, GetLastLine) {
    expect_tuwstr("last", GetLastLine("last"));
    expect_tuwstr("last", GetLastLine("last\n"));
    expect_tuwstr("last", GetLastLine("\nlast"));
    expect_tuwstr("last", GetLastLine("firs\nsecond\nlast\r\n\r\n"));
}

TEST(StringTest, GetLastLineOneChar) {
    expect_tuwstr("l", GetLastLine("l"));
}

TEST(StringTest, GetLastLineNull) {
    expect_nullstr(GetLastLine(nullptr));
    expect_nullstr(GetLastLine(""));
    expect_nullstr(GetLastLine("\n\n\n\n"));
}
