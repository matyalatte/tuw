// Tests for string utils
// Todo: Write more tests

#include "test_utils.h"

// Test noex::vector()
TEST(VectorTest, Construct) {
    noex::vector<int> vec;
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(0, vec.size());
    EXPECT_EQ(0, vec.capacity());
    EXPECT_EQ(noex::OK, noex::GetErrorNo());
}

TEST(VectorTest, Push) {
    noex::vector<int> vec;
    vec.push_back(2);
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(1, vec.size());
    EXPECT_EQ(1, vec.capacity());
    EXPECT_EQ(2, vec[0]);
    EXPECT_EQ(noex::OK, noex::GetErrorNo());
}

TEST(VectorTest, Reserve) {
    noex::vector<int> vec;
    vec.reserve(10);
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(0, vec.size());
    EXPECT_EQ(10, vec.capacity());
    EXPECT_EQ(noex::OK, noex::GetErrorNo());
}

TEST(VectorTest, ReservePush) {
    noex::vector<int> vec;
    vec.reserve(10);
    vec.push_back(2);
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(1, vec.size());
    EXPECT_EQ(10, vec.capacity());
    EXPECT_EQ(2, vec[0]);
    EXPECT_EQ(noex::OK, noex::GetErrorNo());
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
    EXPECT_EQ(noex::OK, noex::GetErrorNo());
    vec.shrink_to_fit();
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(3, vec.size());
    EXPECT_EQ(3, vec.capacity());
    EXPECT_EQ(noex::OK, noex::GetErrorNo());
}
