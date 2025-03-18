// Tests for string utils
// Todo: Write more tests

#include "test_utils.h"

// Test noex::vector()
TEST(VectorTest, Construct) {
    noex::vector<int> vec;
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(0, vec.size());
    EXPECT_EQ(0, vec.capacity());
    EXPECT_EQ(noex::OK, noex::get_error_no());
}

TEST(VectorTest, Push) {
    noex::vector<int> vec;
    vec.push_back(2);
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(1, vec.size());
    EXPECT_EQ(1, vec.capacity());
    EXPECT_EQ(2, vec[0]);
    EXPECT_EQ(noex::OK, noex::get_error_no());
}

TEST(VectorTest, Reserve) {
    noex::vector<int> vec;
    vec.reserve(10);
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(0, vec.size());
    EXPECT_EQ(10, vec.capacity());
    EXPECT_EQ(noex::OK, noex::get_error_no());
}

TEST(VectorTest, ReservePush) {
    noex::vector<int> vec;
    vec.reserve(10);
    vec.push_back(2);
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(1, vec.size());
    EXPECT_EQ(10, vec.capacity());
    EXPECT_EQ(2, vec[0]);
    EXPECT_EQ(noex::OK, noex::get_error_no());
}

TEST(VectorTest, Shrink) {
    noex::vector<int> vec;
    vec.reserve(10);
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(3, vec.size());
    EXPECT_EQ(10, vec.capacity());
    EXPECT_EQ(noex::OK, noex::get_error_no());
    vec.shrink_to_fit();
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(3, vec.size());
    EXPECT_EQ(3, vec.capacity());
    EXPECT_EQ(noex::OK, noex::get_error_no());
}

TEST(VectorTest, Clear) {
    noex::vector<int> vec;
    vec.reserve(10);
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(3, vec.size());
    EXPECT_EQ(10, vec.capacity());
    EXPECT_EQ(noex::OK, noex::get_error_no());
    vec.clear();
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(0, vec.size());
    EXPECT_EQ(0, vec.capacity());
    EXPECT_EQ(noex::OK, noex::get_error_no());
}

TEST(VectorTest, EmplaceBack) {
    noex::vector<noex::string> vec;
    vec.emplace_back("aaa");
    noex::string str = "bbb";
    vec.emplace_back(str);
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(2, vec.size());
    EXPECT_EQ(2, vec.capacity());
    EXPECT_STREQ("aaa", vec[0].c_str());
    EXPECT_STREQ("bbb", vec[1].c_str());
    EXPECT_EQ(noex::OK, noex::get_error_no());
}

struct TestObj {
    int* a;
    int b;
};

void AddTestObj(noex::vector<TestObj>* vec, int* a, int b) {
    vec->push_back({ a, b });
}

TEST(VectorTest, StructVec) {
    noex::vector<TestObj> vec;
    int a1 = 1;
    int a2 = 3;
    int a3 = 5;
    AddTestObj(&vec, &a1, 2);
    AddTestObj(&vec, &a2, 4);
    AddTestObj(&vec, &a3, 6);
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(3, vec.size());
    EXPECT_EQ(3, vec.capacity());
    EXPECT_EQ(1, *vec[0].a);
    EXPECT_EQ(2, vec[0].b);
    EXPECT_EQ(3, *vec[1].a);
    EXPECT_EQ(4, vec[1].b);
    EXPECT_EQ(5, *vec[2].a);
    EXPECT_EQ(6, vec[2].b);
    EXPECT_EQ(noex::OK, noex::get_error_no());
}

TEST(VectorTest, Assign) {
    noex::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    EXPECT_EQ(4, vec.size());
    EXPECT_EQ(4, vec[3]);
    noex::vector<int> vec2(vec);
    EXPECT_EQ(4, vec2.size());
    EXPECT_EQ(4, vec2[3]);
    EXPECT_EQ(noex::OK, noex::get_error_no());
}

TEST(VectorTest, Move) {
    noex::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    EXPECT_EQ(4, vec.size());
    EXPECT_EQ(4, vec[3]);
    noex::vector<int> vec2(std::move(vec));
    EXPECT_EQ(4, vec2.size());
    EXPECT_EQ(4, vec2[3]);
    EXPECT_EQ(noex::OK, noex::get_error_no());
}
