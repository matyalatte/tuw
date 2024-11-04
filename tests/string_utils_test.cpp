// Tests for string utils
// Todo: Write more tests

#include "test_utils.h"

void expect_nullstr(const tuwString& str) {
    EXPECT_TRUE(str.empty());
    EXPECT_EQ(str.size(), 0);
    EXPECT_EQ(str.data(), nullptr);
    EXPECT_STREQ(str.c_str(), "");
    EXPECT_EQ(str, "");
}

void expect_tuwstr(const char* expected, const tuwString& actual) {
    size_t size = strlen(expected);
    EXPECT_EQ(actual.size(), size);
    EXPECT_STREQ(actual.data(), expected);
    EXPECT_STREQ(actual.c_str(), expected);
    EXPECT_EQ(actual, expected);
}

// Test tuwString()
TEST(tuwStringTest, Construct) {
    tuwString str;
    expect_nullstr(str);
}

TEST(tuwStringTest, ConstructWithNull) {
    tuwString str(nullptr);
    expect_nullstr(str);
}

TEST(tuwStringTest, ConstructWithCstr) {
    tuwString str("test");
    EXPECT_FALSE(str.empty());
    expect_tuwstr("test", str);
}

TEST(tuwStringTest, ConstructWithCstrAndSize) {
    tuwString str("testtest", 4);
    expect_tuwstr("test", str);
}

TEST(tuwStringTest, ConstructWithNullAndSize) {
    tuwString str(nullptr, 4);
    expect_nullstr(str);
}

TEST(tuwStringTest, ConstructWithTuwstr) {
    tuwString str2("test");
    tuwString str(str2);
    expect_tuwstr("test", str);
    expect_tuwstr("test", str2);
}

TEST(tuwStringTest, ConstructWithMovedTuwstr) {
    tuwString str2("test");
    tuwString str(std::move(str2));
    expect_tuwstr("test", str);
    expect_nullstr(str2);
}

TEST(tuwStringTest, ConstructWithSize) {
    tuwString str(4);
    EXPECT_TRUE(str.empty());
    EXPECT_EQ(str.size(), 4);
    EXPECT_EQ(str.data()[3], '\0');
    EXPECT_STREQ(str.c_str(), "");
    EXPECT_EQ(str, "");
}

// Test =
TEST(tuwStringTest, AssignNull) {
    tuwString str = nullptr;
    expect_nullstr(str);
}

TEST(tuwStringTest, AssignCstr) {
    tuwString str = "test";
    expect_tuwstr("test", str);
}

TEST(tuwStringTest, AssignTuwstr) {
    tuwString str2 ="test";
    tuwString str = str2;
    expect_tuwstr("test", str);
    expect_tuwstr("test", str2);
}

TEST(tuwStringTest, AssingMovedTuwstr) {
    tuwString str2 = "test";
    tuwString str = std::move(str2);
    expect_tuwstr("test", str);
    expect_nullstr(str2);
}

TEST(tuwStringTest, AssingSelf) {
    tuwString str = "test";
    str = str;
    expect_tuwstr("test", str);
}

TEST(tuwStringTest, AssingMovedSelf) {
    tuwString str = "test";
    str = std::move(str);
    expect_tuwstr("test", str);
}

// Test +=
TEST(tuwStringTest, AppendCstr) {
    tuwString str = "test";
    str += "foo";
    expect_tuwstr("testfoo", str);
}

TEST(tuwStringTest, AppendNull) {
    tuwString str = "test";
    str += nullptr;
    expect_tuwstr("test", str);
}

TEST(tuwStringTest, AppendToNull) {
    tuwString str = nullptr;
    str += "test";
    expect_tuwstr("test", str);
}

TEST(tuwStringTest, AppendTuwstr) {
    tuwString str = "test";
    tuwString str2 = "foo";
    str += str2;
    expect_tuwstr("testfoo", str);
    expect_tuwstr("foo", str2);
}

// Test +
TEST(tuwStringTest, PlusCstr) {
    tuwString str = tuwString("test") + "foo";
    expect_tuwstr("testfoo", str);
}

TEST(tuwStringTest, PlusNull) {
    tuwString str = tuwString("test") + nullptr;
    expect_tuwstr("test", str);
}

TEST(tuwStringTest, PlusToNull) {
    tuwString str = tuwString(nullptr) + "test";
    expect_tuwstr("test", str);
}

TEST(tuwStringTest, PlusTuwstr) {
    tuwString str = "test";
    tuwString str2 = "foo";
    tuwString str3 = str + str2;
    expect_tuwstr("test", str);
    expect_tuwstr("foo", str2);
    expect_tuwstr("testfoo", str3);
}

TEST(tuwStringTest, PlusInt) {
    int a = -1;
    tuwString result = tuwString("test") + a + "bar";
    EXPECT_STREQ("test-1bar", result.c_str());
}

TEST(tuwStringTest, PlusSizet) {
    size_t a = 100;
    tuwString result = tuwString("test") + a + "bar";
    EXPECT_STREQ("test100bar", result.c_str());
}

TEST(tuwStringTest, PlusUint32) {
    uint32_t a = 100;
    tuwString result = tuwString("test") + a + "bar";
    EXPECT_STREQ("test100bar", result.c_str());
}

TEST(tuwStringTest, CstrPlusTuwstr) {
    tuwString str = "test" + tuwString("foo");
    expect_tuwstr("testfoo", str);
}

TEST(tuwStringTest, NullPlusTuwstr) {
    tuwString str = nullptr + tuwString("test");
    expect_tuwstr("test", str);
}

// Test []
TEST(tuwStringTest, Index) {
    tuwString str = "test";
    EXPECT_EQ('t', str[0]);
    EXPECT_EQ('e', str[1]);
    EXPECT_EQ('s', str[2]);
    EXPECT_EQ('t', str[3]);
    EXPECT_EQ('\0', str[4]);
    EXPECT_EQ(STR_OK, GetStringError());
    EXPECT_EQ('\0', str[5]);
    EXPECT_EQ(STR_BOUNDARY_ERROR, GetStringError());
    ClearStringError();
}

TEST(tuwStringTest, IndexNull) {
    tuwString str;
    EXPECT_EQ('\0', str[0]);
}

// Test == and !=
TEST(tuwStringTest, EqualToStr) {
    tuwString str = "test";
    EXPECT_TRUE(str != nullptr);
    EXPECT_EQ(str, "test");
    EXPECT_EQ(str, tuwString("test"));
    EXPECT_NE(str, "task");
    EXPECT_NE(str, tuwString("task"));
}

TEST(tuwStringTest, EqualToStrReverse) {
    tuwString str = "test";
    EXPECT_NE(nullptr, str);
    EXPECT_EQ("test", str);
    EXPECT_NE("task", str);
}

TEST(tuwStringTest, EqualToNull) {
    tuwString str;
    EXPECT_EQ(str, nullptr);
    EXPECT_EQ(str, "");
    EXPECT_EQ(str, tuwString());
    EXPECT_NE(str, "test");
    EXPECT_NE(str, tuwString("test"));
}

TEST(tuwStringTest, EqualToNullReverse) {
    tuwString str;
    EXPECT_EQ(nullptr, str);
    EXPECT_EQ("", str);
    EXPECT_NE("test", str);
}

// Test find()
TEST(tuwStringTest, FindChar) {
    tuwString str = "footestfoo";
    EXPECT_EQ(0, str.find('f'));
    EXPECT_EQ(3, str.find('t'));
    EXPECT_EQ(5, str.find('s'));
    EXPECT_EQ(tuwString::npos, str.find('a'));
    EXPECT_EQ(tuwString::npos, tuwString().find('a'));
}

TEST(tuwStringTest, FindCstr) {
    tuwString str = "footestfoo";
    EXPECT_EQ(0, str.find("foo"));
    EXPECT_EQ(3, str.find("test"));
    EXPECT_EQ(tuwString::npos, str.find(nullptr));
    EXPECT_EQ(tuwString::npos, str.find("task"));
    EXPECT_EQ(tuwString::npos, str.find("fooo"));
    EXPECT_EQ(tuwString::npos, tuwString().find("test"));
}

TEST(tuwStringTest, FindTuwstr) {
    tuwString str = "footestfoo";
    EXPECT_EQ(0, str.find(tuwString("foo")));
    EXPECT_EQ(3, str.find(tuwString("test")));
    EXPECT_EQ(0, str.find(tuwString(nullptr)));
    EXPECT_EQ(tuwString::npos, str.find(tuwString("task")));
    EXPECT_EQ(tuwString::npos, str.find(tuwString("fooo")));
    EXPECT_EQ(tuwString::npos, tuwString().find(tuwString("test")));
}

// Test push_back()
TEST(tuwStringTest, Pushback) {
    tuwString str;
    expect_nullstr(str);
    str.push_back('t');
    expect_tuwstr("t", str);
    str.push_back('e');
    str.push_back('s');
    str.push_back('t');
    expect_tuwstr("test", str);
}

// Test substr()
TEST(tuwStringTest, Substr) {
    tuwString str = "footestfoo";
    expect_tuwstr("foo", str.substr(0, 3));
    expect_tuwstr("test", str.substr(3, 4));
    EXPECT_EQ(STR_OK, GetStringError());
    expect_nullstr(str.substr(7, 10));
    expect_nullstr(str.substr(20, 4));
    expect_nullstr(tuwString().substr(0, 3));
    EXPECT_EQ(STR_BOUNDARY_ERROR, GetStringError());
    ClearStringError();
}

// Test begin() and end()
TEST(tuwStringTest, Iter) {
    tuwString str = "footestfoo";
    EXPECT_EQ(&str.data()[0], str.begin());
    EXPECT_EQ(&str.data()[str.size()], str.end());
}

TEST(tuwStringTest, IterForNull) {
    tuwString str;
    EXPECT_EQ(&str.c_str()[0], str.begin());
    EXPECT_EQ(&str.c_str()[0], str.end());
}

// Test tuwWstring
void expect_nullwstr(const tuwWstring& str) {
    EXPECT_TRUE(str.empty());
    EXPECT_EQ(str.size(), 0);
    EXPECT_STREQ(str.c_str(), L"");
}

void expect_tuwwstr(const wchar_t* expected, const tuwWstring& actual) {
    size_t size = wcslen(expected);
    EXPECT_EQ(actual.size(), size);
    EXPECT_STREQ(actual.c_str(), expected);
}

TEST(tuwWstringTest, ConstructWithNull) {
    tuwWstring str(nullptr);
    expect_nullwstr(str);
}

TEST(tuwWstringTest, ConstructWithCstr) {
    tuwWstring str(L"test");
    EXPECT_FALSE(str.empty());
    expect_tuwwstr(L"test", str);
}
