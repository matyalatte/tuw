// Tests for json embedding
// Todo: Write more tests

#include "test_utils.h"

TEST(JsonEmbeddingTest, Embed) {
    {
        tuwjson::Value test_json;
        test_json.SetObject();
        GetTestJson(test_json);
        ExeContainer exe;
        noex::string result = exe.Read(JSON_ALL_KEYS);
        EXPECT_STREQ("", result.c_str());
        exe.SetJson(test_json);
        result = exe.Write("embedded.json");
        EXPECT_STREQ("", result.c_str());
    }
    {
        tuwjson::Value test_json;
        test_json.SetObject();
        GetTestJson(test_json);
        tuwjson::Value embedded_json;
        embedded_json.SetObject();
        ExeContainer exe;
        noex::string result = exe.Read("embedded.json");
        EXPECT_STREQ("", result.c_str());
        exe.GetJson(embedded_json);
        EXPECT_EQ(embedded_json, test_json);
    }
}
