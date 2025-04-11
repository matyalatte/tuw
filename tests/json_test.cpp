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

TEST_F(JsonTest, ParseStringEscape) {
    tuwjson::Error err = parser.ParseJson("  \"\\\"\\\\\\/\\b\\f\\n\\r\\t\"  ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsString());
    EXPECT_STREQ(root.GetString(), "\"\\/\b\f\n\r\t");
}

TEST_F(JsonTest, ParseStringUTF) {
    tuwjson::Error err = parser.ParseJson("  \"\xc2\x80\xe0\x80\xbf\xf0\x80\xbf\xa0\"  ", &root);
    EXPECT_EQ(err, tuwjson::JSON_OK);
    EXPECT_TRUE(root.IsString());
    EXPECT_STREQ(root.GetString(), "\xc2\x80\xe0\x80\xbf\xf0\x80\xbf\xa0");
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
        "{\r\n\"a\": NaN\r\n}",
        tuwjson::JSON_ERR_UNKNOWN_LITERAL,
        "unknown literal detected (line: 2, column: 6)"
    },
    {
        "1.a",
        tuwjson::JSON_ERR_UNKNOWN_LITERAL,
        "unknown literal detected (line: 1, column: 1)"
    },
    {
        "{\"\xc2\xbe" "abc\xbf" "\": true}",
        tuwjson::JSON_ERR_INVALID_UTF,
        "invalid UTF8 character detected (line: 1, column: 8)"
    },
    {
        "\xc2\x32}",
        tuwjson::JSON_ERR_INVALID_UTF,
        "invalid UTF8 character detected (line: 1, column: 2)"
    },
    {
        "\xf5\xbe\xbe\xbe",
        tuwjson::JSON_ERR_INVALID_UTF,
        "invalid UTF8 character detected (line: 1, column: 1)"
    },
    {
        "1111111111111111111111111",
        tuwjson::JSON_ERR_INVALID_INT,
        "failed to parse an integer (line: 1, column: 1)"
    },
    {
        "1.9123749837249827349e-10938903810",
        tuwjson::JSON_ERR_INVALID_DOUBLE,
        "failed to parse a double number (line: 1, column: 1)"
    },
    {
        "[\ntrue,\n,false]",
        tuwjson::JSON_ERR_INVALID_COMMA,
        "there is a comma in the wrong position (line: 3, column: 1)"
    },
    {
        "{\"\ba\": true}",
        tuwjson::JSON_ERR_CONTROL_CHAR,
        "there is a control character in a string (line: 1, column: 3)"
    },
    {
        "\"\\\ta\"",
        tuwjson::JSON_ERR_CONTROL_CHAR,
        "there is a control character in a string (line: 1, column: 3)"
    },
    {
        "\"\\a\"",
        tuwjson::JSON_ERR_INVALID_ESCAPE,
        "invalid escaped character: \\a (line: 1, column: 3)"
    },
    {
        "\"\\u0034\"",
        tuwjson::JSON_ERR_UNICODE_ESCAPE,
        "unicode escape (\\uXXXX) is not supported. "
        "use UTF-8 characters instead (line: 1, column: 3)"
    },
    {
        "\"abcd",
        tuwjson::JSON_ERR_UNCLOSED_STR,
        "string is not closed with '\"' (line: 1, column: 5)"
    },
    {
        "\"abc\nd\"",
        tuwjson::JSON_ERR_UNCLOSED_STR,
        "string is not closed with '\"' (line: 1, column: 4)"
    },
    {
        "[\ntrue, false /* \n]",
        tuwjson::JSON_ERR_UNCLOSED_COMMENT,
        "multiline comment is not closed (line: 3, column: 2)"
    },
    {
        "[\ntrue, /* false\n]",
        tuwjson::JSON_ERR_UNCLOSED_COMMENT,
        "multiline comment is not closed (line: 3, column: 2)"
    },
    {
        "[\ntrue /* , false\n]",
        tuwjson::JSON_ERR_UNCLOSED_COMMENT,
        "multiline comment is not closed (line: 3, column: 2)"
    },
    {
        "{\n\"key\": \"val\" /* \n}",
        tuwjson::JSON_ERR_UNCLOSED_COMMENT,
        "multiline comment is not closed (line: 3, column: 2)"
    },
    {
        "{\n\"key\": /* \"val\" \n}",
        tuwjson::JSON_ERR_UNCLOSED_COMMENT,
        "multiline comment is not closed (line: 3, column: 2)"
    },
    {
        "{\n\"key\"/* : \"val\" \n}",
        tuwjson::JSON_ERR_UNCLOSED_COMMENT,
        "multiline comment is not closed (line: 3, column: 2)"
    },
    {
        "{\n/* \"key\": \"val\" \n}",
        tuwjson::JSON_ERR_UNCLOSED_COMMENT,
        "multiline comment is not closed (line: 3, column: 2)"
    },
    {
        "[true, false",
        tuwjson::JSON_ERR_UNCLOSED_ARRAY,
        "comma ',' or closing bracket ']' is missing (line: 1, column: 13)"
    },
    {
        "[true, false false]",
        tuwjson::JSON_ERR_UNCLOSED_ARRAY,
        "comma ',' or closing bracket ']' is missing (line: 1, column: 14)"
    },
    {
        "{\"a\":true",
        tuwjson::JSON_ERR_UNCLOSED_OBJECT,
        "comma ',' or closing brace '}' is missing (line: 1, column: 10)"
    },
    {
        "{\"a\":true \"b\":true}",
        tuwjson::JSON_ERR_UNCLOSED_OBJECT,
        "comma ',' or closing brace '}' is missing (line: 1, column: 11)"
    },
    {
        "{\"a\", true}",
        tuwjson::JSON_ERR_EXPECTED_COLON,
        "colon ':' is missing (line: 1, column: 5)"
    },
    {
        "{1: true}",
        tuwjson::JSON_ERR_INVALID_KEY,
        "string key is missing (line: 1, column: 2)"
    },
    {
        "{\n\"a\": true,\n\"a\": false}",
        tuwjson::JSON_ERR_DUPLICATED_KEY,
        "there is a duplicated key (line: 3, column: 1)"
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
