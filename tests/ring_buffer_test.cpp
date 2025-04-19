// Tests for string utils
// Todo: Write more tests

#include "test_utils.h"

// Test RingStrBuffer
TEST(RingStrBufferTest, Construct) {
    RingStrBuffer<10> buf;
    EXPECT_TRUE(buf.IsEmpty());
    EXPECT_FALSE(buf.IsFull());
    EXPECT_STREQ("", buf.ToString().c_str());
}

TEST(RingStrBufferTest, PushBackChar) {
    RingStrBuffer<10> buf;
    buf.PushBack('c');
    EXPECT_FALSE(buf.IsEmpty());
    EXPECT_FALSE(buf.IsFull());
    EXPECT_STREQ("c", buf.ToString().c_str());
}

TEST(RingStrBufferTest, PushBackStr) {
    RingStrBuffer<10> buf;
    buf.PushBack("test", 4);
    EXPECT_FALSE(buf.IsEmpty());
    EXPECT_FALSE(buf.IsFull());
    EXPECT_STREQ("test", buf.ToString().c_str());
}

TEST(RingStrBufferTest, PushBackLongStr) {
    RingStrBuffer<10> buf;
    buf.PushBack("0123456789test", 14);
    EXPECT_FALSE(buf.IsEmpty());
    EXPECT_TRUE(buf.IsFull());
    EXPECT_STREQ("456789test", buf.ToString().c_str());
}

TEST(RingStrBufferTest, PushBackTwoStr) {
    RingStrBuffer<10> buf;
    buf.PushBack("0123456", 7);
    buf.PushBack("789test", 7);
    EXPECT_FALSE(buf.IsEmpty());
    EXPECT_TRUE(buf.IsFull());
    EXPECT_STREQ("456789test", buf.ToString().c_str());
}

TEST(RingStrBufferTest, PushBackStrAndChar) {
    RingStrBuffer<10> buf;
    buf.PushBack("0123456789", 10);
    EXPECT_FALSE(buf.IsEmpty());
    EXPECT_TRUE(buf.IsFull());
    buf.PushBack('c');
    EXPECT_STREQ("123456789c", buf.ToString().c_str());
}

TEST(RingStrBufferTest, PushBackStrAndLongStr) {
    RingStrBuffer<10> buf;
    buf.PushBack("0123456", 7);
    const char* str = "abcdefghijklmnopqrstuvwxyz0123456789";
    buf.PushBack(str, strlen(str));
    EXPECT_FALSE(buf.IsEmpty());
    EXPECT_TRUE(buf.IsFull());
    EXPECT_STREQ("0123456789", buf.ToString().c_str());
}

TEST(RingStrBufferTest, PushBackNull) {
    RingStrBuffer<10> buf;
    const char* null = nullptr;
    buf.PushBack(null, 4);
    EXPECT_TRUE(buf.IsEmpty());
    EXPECT_FALSE(buf.IsFull());
    EXPECT_STREQ("", buf.ToString().c_str());
}
