#include "test_utils.h"

class JsonTest : public ::testing::Test {
 protected:
    tuwjson::Value root;
    tuwjson::Parser parser;
};

static double eps = 0.00001;

// Test tuwjson::string()
TEST_F(JsonTest, ParseEmpty) {
    tuwjson::Error err = parser.ParseJson("  ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsNull());
}

TEST_F(JsonTest, ParseNull) {
    tuwjson::Error err = parser.ParseJson("  null  ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsNull());
}

TEST_F(JsonTest, ParseTrue) {
    tuwjson::Error err = parser.ParseJson("  true  ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsBool());
    EXPECT_TRUE(root.GetBool());
}

TEST_F(JsonTest, ParseFalse) {
    tuwjson::Error err = parser.ParseJson("  false  ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsBool());
    EXPECT_FALSE(root.GetBool());
}

TEST_F(JsonTest, ParseInt) {
    tuwjson::Error err = parser.ParseJson("  123  ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsInt());
    EXPECT_EQ(root.GetInt(), 123);
    EXPECT_TRUE(root.IsDouble());
    EXPECT_NEAR(root.GetDouble(), 123.0, eps);
}

TEST_F(JsonTest, ParseFloat) {
    tuwjson::Error err = parser.ParseJson("  -12.3e-1  ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsDouble());
    EXPECT_NEAR(root.GetDouble(), -1.23, eps);
}

TEST_F(JsonTest, ParseString) {
    tuwjson::Error err = parser.ParseJson("  \"abcdefg\"  ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsString());
    EXPECT_STREQ(root.GetString(), "abcdefg");
}

TEST_F(JsonTest, ParseArray) {
    tuwjson::Error err = parser.ParseJson("  [\"abcdefg\", 1,true,]  ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsArray());
    EXPECT_FALSE(root.IsEmpty());
    EXPECT_EQ(root.Size(), 3);
    EXPECT_TRUE(root[0].IsString());
    EXPECT_STREQ(root[0].GetString(), "abcdefg");
    EXPECT_TRUE(root[1].IsInt());
    EXPECT_EQ(root[1].GetInt(), 1);
    EXPECT_TRUE(root[2].IsBool());
    EXPECT_TRUE(root[2].GetBool());
}

TEST_F(JsonTest, ParseObject) {
    tuwjson::Error err = parser.ParseJson(" { \"key\" : \"val\",\"key2\":2.3} ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsObject());
    EXPECT_FALSE(root.IsEmpty());
    EXPECT_EQ(root.Size(), 2);
    EXPECT_TRUE(root["key"].IsString());
    EXPECT_STREQ(root["key"].GetString(), "val");
    EXPECT_TRUE(root["key2"].IsDouble());
    EXPECT_NEAR(root["key2"].GetDouble(), 2.3, eps);
}

struct ParseFailCase {
    const char* json;
    tuwjson::Error err;
    const char* err_msg;
};

class ParseFailTest : public ::testing::TestWithParam<ParseFailCase> {
 protected:
    tuwjson::Value root;
    tuwjson::Parser parser;
};

const ParseFailCase parse_fail_cases[] = {
    {
        "{\"a\": [{\"key\": true]}",
        tuwjson::JSON_ERR_UNCLOSED_OBJECT,
        "comma ',' or closing brace '}' is missing (line: 1, offset: 19)"
    },
};

INSTANTIATE_TEST_SUITE_P(ParseFailTestInstantiation,
    ParseFailTest,
    ::testing::ValuesIn(parse_fail_cases));

TEST_P(ParseFailTest, ParseFail) {
    const ParseFailCase test_case = GetParam();
    tuwjson::Error err = parser.ParseJson(test_case.json, &root);
    noex::string json_str = test_case.json;
    if (json_str.size() > 20)
        json_str = json_str.substr(0, 20) + "...";
    EXPECT_EQ(err, test_case.err)  <<
        "  json: " << json_str.c_str();
    EXPECT_STREQ(parser.GetErrMsg(), test_case.err_msg) <<
        "  json: " << json_str.c_str();
}
