#pragma once

// Tests for json checking
// Todo: Write more tests

#include "test_utils.h"

TEST(StringUtilsTest, ConcatCStringsChar) {
    std::string result = ConcatCStrings("test", "foo", "bar");
    EXPECT_STREQ("testfoobar", result.c_str());
    result = ConcatCStrings("test", "foo", nullptr);
    EXPECT_STREQ("testfoo", result.c_str());
}

TEST(StringUtilsTest, ConcatCStringsInt) {
    int a = -1;
    std::string result = ConcatCStrings("test", a, "bar");
    EXPECT_STREQ("test-1bar", result.c_str());
    result = ConcatCStrings("test", a, nullptr);
    EXPECT_STREQ("test-1", result.c_str());
}

TEST(StringUtilsTest, ConcatCStringsSizet) {
    size_t a = 100;
    std::string result = ConcatCStrings("test", a, "bar");
    EXPECT_STREQ("test100bar", result.c_str());
    result = ConcatCStrings("test", a, nullptr);
    EXPECT_STREQ("test100", result.c_str());
}

TEST(StringUtilsTest, ConcatCStringsUint32) {
    uint32_t a = 100;
    std::string result = ConcatCStrings("test", a, "bar");
    EXPECT_STREQ("test100bar", result.c_str());
    result = ConcatCStrings("test", a, nullptr);
    EXPECT_STREQ("test100", result.c_str());
}
