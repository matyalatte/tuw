// Tests for string utils
// Todo: Write more tests

#include "test_utils.h"

void expect_nullstr(const noex::string& str) {
    EXPECT_TRUE(str.empty());
    EXPECT_EQ(str.size(), 0);
    EXPECT_EQ(str.data(), nullptr);
    EXPECT_STREQ(str.c_str(), "");
    EXPECT_EQ(str, "");
}

void expect_tuwstr(const char* expected, const noex::string& actual) {
    size_t size = strlen(expected);
    EXPECT_EQ(actual.size(), size);
    EXPECT_STREQ(actual.data(), expected);
    EXPECT_STREQ(actual.c_str(), expected);
    EXPECT_EQ(actual, expected);
}

// Test noex::string()
TEST(StringTest, Construct) {
    noex::string str;
    expect_nullstr(str);
}

TEST(StringTest, ConstructWithNull) {
    noex::string str(nullptr);
    expect_nullstr(str);
}

TEST(StringTest, ConstructWithCstr) {
    noex::string str("test");
    EXPECT_FALSE(str.empty());
    expect_tuwstr("test", str);
}

TEST(StringTest, ConstructWithCstrAndSize) {
    noex::string str("testtest", 4);
    expect_tuwstr("test", str);
}

TEST(StringTest, ConstructWithNullAndSize) {
    noex::string str(nullptr, 4);
    expect_nullstr(str);
}

TEST(StringTest, ConstructWithTuwstr) {
    noex::string str2("test");
    noex::string str(str2);
    expect_tuwstr("test", str);
    expect_tuwstr("test", str2);
}

TEST(StringTest, ConstructWithMovedTuwstr) {
    noex::string str2("test");
    noex::string str(std::move(str2));
    expect_tuwstr("test", str);
    expect_nullstr(str2);
}

TEST(StringTest, ConstructWithSize) {
    noex::string str(4);
    EXPECT_FALSE(str.empty());
    EXPECT_EQ(str.size(), 4);
    EXPECT_EQ(str.data()[3], '\0');
    EXPECT_STREQ(str.c_str(), "");
    EXPECT_EQ(str, "");
}

// Test =
TEST(StringTest, AssignNull) {
    noex::string str = nullptr;
    expect_nullstr(str);
}

TEST(StringTest, AssignCstr) {
    noex::string str = "test";
    expect_tuwstr("test", str);
}

TEST(StringTest, AssignTuwstr) {
    noex::string str2 ="test";
    noex::string str = str2;
    expect_tuwstr("test", str);
    expect_tuwstr("test", str2);
}

TEST(StringTest, AssingMovedTuwstr) {
    noex::string str2 = "test";
    noex::string str = std::move(str2);
    expect_tuwstr("test", str);
    expect_nullstr(str2);
}

TEST(StringTest, AssingSelf) {
    noex::string str = "test";
    str = str;
    expect_tuwstr("test", str);
}

TEST(StringTest, AssingMovedSelf) {
    noex::string str = "test";
    str = std::move(str);
    expect_tuwstr("test", str);
}

// Test +=
TEST(StringTest, AppendCstr) {
    noex::string str = "test";
    str += "foo";
    expect_tuwstr("testfoo", str);
}

TEST(StringTest, AppendNull) {
    noex::string str = "test";
    str += nullptr;
    expect_tuwstr("test", str);
}

TEST(StringTest, AppendToNull) {
    noex::string str = nullptr;
    str += "test";
    expect_tuwstr("test", str);
}

TEST(StringTest, AppendTuwstr) {
    noex::string str = "test";
    noex::string str2 = "foo";
    str += str2;
    expect_tuwstr("testfoo", str);
    expect_tuwstr("foo", str2);
}

// Test +
TEST(StringTest, PlusCstr) {
    noex::string str = noex::string("test") + "foo";
    expect_tuwstr("testfoo", str);
}

TEST(StringTest, PlusNull) {
    noex::string str = noex::string("test") + nullptr;
    expect_tuwstr("test", str);
}

TEST(StringTest, PlusToNull) {
    noex::string str = noex::string(nullptr) + "test";
    expect_tuwstr("test", str);
}

TEST(StringTest, PlusTuwstr) {
    noex::string str = "test";
    noex::string str2 = "foo";
    noex::string str3 = str + str2;
    expect_tuwstr("test", str);
    expect_tuwstr("foo", str2);
    expect_tuwstr("testfoo", str3);
}

TEST(StringTest, PlusInt) {
    int a = -1;
    noex::string result = noex::string("test") + a + "bar";
    EXPECT_STREQ("test-1bar", result.c_str());
}

TEST(StringTest, PlusSizet) {
    size_t a = 100;
    noex::string result = noex::string("test") + a + "bar";
    EXPECT_STREQ("test100bar", result.c_str());
}

TEST(StringTest, PlusUint32) {
    uint32_t a = 100;
    noex::string result = noex::string("test") + a + "bar";
    EXPECT_STREQ("test100bar", result.c_str());
}

TEST(StringTest, CstrPlusTuwstr) {
    noex::string str = "test" + noex::string("foo");
    expect_tuwstr("testfoo", str);
}

TEST(StringTest, NullPlusTuwstr) {
    char* null = nullptr;
    noex::string str = null + noex::string("test");
    expect_tuwstr("test", str);
}

// Test []
TEST(StringTest, Index) {
    noex::string str = "test";
    EXPECT_EQ('t', str[0]);
    EXPECT_EQ('e', str[1]);
    EXPECT_EQ('s', str[2]);
    EXPECT_EQ('t', str[3]);
    EXPECT_EQ('\0', str[4]);
    EXPECT_EQ(noex::OK, noex::GetErrorNo());
    EXPECT_EQ('\0', str[5]);
    EXPECT_EQ(noex::STR_BOUNDARY_ERROR, noex::GetErrorNo());
    noex::ClearErrorNo();
}

TEST(StringTest, IndexNull) {
    noex::string str;
    EXPECT_EQ('\0', str[0]);
}

// Test == and !=
TEST(StringTest, EqualToStr) {
    noex::string str = "test";
    char* null = nullptr;
    EXPECT_TRUE(str != null);
    EXPECT_EQ(str, "test");
    EXPECT_EQ(str, noex::string("test"));
    EXPECT_NE(str, "task");
    EXPECT_NE(str, noex::string("task"));
}

TEST(StringTest, EqualToStrReverse) {
    noex::string str = "test";
    char* null = nullptr;
    EXPECT_NE(null, str);
    EXPECT_EQ("test", str);
    EXPECT_NE("task", str);
}

TEST(StringTest, EqualToNull) {
    noex::string str;
    char* null = nullptr;
    EXPECT_EQ(str, null);
    EXPECT_EQ(str, "");
    EXPECT_EQ(str, noex::string());
    EXPECT_NE(str, "test");
    EXPECT_NE(str, noex::string("test"));
}

TEST(StringTest, EqualToNullReverse) {
    noex::string str;
    char* null = nullptr;
    EXPECT_EQ(null, str);
    EXPECT_EQ("", str);
    EXPECT_NE("test", str);
}

// Test find()
TEST(StringTest, FindChar) {
    noex::string str = "footestfoo";
    EXPECT_EQ(0, str.find('f'));
    EXPECT_EQ(3, str.find('t'));
    EXPECT_EQ(5, str.find('s'));
    EXPECT_EQ(noex::string::npos, str.find('a'));
    EXPECT_EQ(noex::string::npos, noex::string().find('a'));
}

TEST(StringTest, FindCstr) {
    noex::string str = "footestfoo";
    EXPECT_EQ(0, str.find("foo"));
    EXPECT_EQ(3, str.find("test"));
    EXPECT_EQ(noex::string::npos, str.find(nullptr));
    EXPECT_EQ(noex::string::npos, str.find("task"));
    EXPECT_EQ(noex::string::npos, str.find("fooo"));
    EXPECT_EQ(noex::string::npos, noex::string().find("test"));
}

TEST(StringTest, FindTuwstr) {
    noex::string str = "footestfoo";
    EXPECT_EQ(0, str.find(noex::string("foo")));
    EXPECT_EQ(3, str.find(noex::string("test")));
    EXPECT_EQ(0, str.find(noex::string(nullptr)));
    EXPECT_EQ(noex::string::npos, str.find(noex::string("task")));
    EXPECT_EQ(noex::string::npos, str.find(noex::string("fooo")));
    EXPECT_EQ(noex::string::npos, noex::string().find(noex::string("test")));
}

// Test push_back()
TEST(StringTest, Pushback) {
    noex::string str;
    expect_nullstr(str);
    str.push_back('t');
    expect_tuwstr("t", str);
    str.push_back('e');
    str.push_back('s');
    str.push_back('t');
    expect_tuwstr("test", str);
}

// Test substr()
TEST(StringTest, Substr) {
    noex::string str = "footestfoo";
    expect_tuwstr("foo", str.substr(0, 3));
    expect_tuwstr("test", str.substr(3, 4));
    EXPECT_EQ(noex::OK, noex::GetErrorNo());
    expect_nullstr(str.substr(7, 10));
    expect_nullstr(str.substr(20, 4));
    expect_nullstr(noex::string().substr(0, 3));
    EXPECT_EQ(noex::STR_BOUNDARY_ERROR, noex::GetErrorNo());
    noex::ClearErrorNo();
}

// Test begin() and end()
TEST(StringTest, Iter) {
    noex::string str = "footestfoo";
    EXPECT_EQ(&str.data()[0], str.begin());
    EXPECT_EQ(&str.data()[str.size()], str.end());
}

TEST(StringTest, IterForNull) {
    noex::string str;
    EXPECT_EQ(&str.c_str()[0], str.begin());
    EXPECT_EQ(&str.c_str()[0], str.end());
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

// Test noex::wstring
void expect_nullwstr(const noex::wstring& str) {
    EXPECT_TRUE(str.empty());
    EXPECT_EQ(str.size(), 0);
    EXPECT_STREQ(str.c_str(), L"");
}

void expect_tuwwstr(const wchar_t* expected, const noex::wstring& actual) {
    size_t size = wcslen(expected);
    EXPECT_EQ(actual.size(), size);
    EXPECT_STREQ(actual.c_str(), expected);
}

TEST(WstringTest, ConstructWithNull) {
    noex::wstring str(nullptr);
    expect_nullwstr(str);
    noex::wstring str2(L"");
    expect_nullwstr(str2);
}

TEST(WstringTest, ConstructWithCstr) {
    noex::wstring str(L"test");
    EXPECT_FALSE(str.empty());
    expect_tuwwstr(L"test", str);
}
