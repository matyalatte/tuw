#pragma once
// Tests for json embedding
// Todo: Write more tests

#include "test_utils.h"

TEST(JsonEmbeddingTest, Embed) {
    {
        rapidjson::Document test_json;
        test_json.SetObject();
        GetTestJson(test_json);
        ExeContainer exe;
        json_utils::JsonResult result = exe.Read(JSON_ALL_KEYS);
        EXPECT_TRUE(result.ok);
        exe.SetJson(test_json);
        result = exe.Write("embedded.json");
        EXPECT_TRUE(result.ok);
    }
    {
        rapidjson::Document test_json;
        test_json.SetObject();
        GetTestJson(test_json);
        rapidjson::Document embedded_json;
        embedded_json.SetObject();
        ExeContainer exe;
        json_utils::JsonResult result = exe.Read("embedded.json");
        EXPECT_TRUE(result.ok);
        exe.GetJson(embedded_json);
        EXPECT_EQ(embedded_json, test_json);
    }
}
