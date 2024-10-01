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
        ErrorState result;
        exe.Read(JSON_ALL_KEYS, &result);
        EXPECT_TRUE(result.Ok());
        exe.SetJson(test_json);
        exe.Write("embedded.json", &result);
        EXPECT_TRUE(result.Ok());
    }
    {
        rapidjson::Document test_json;
        test_json.SetObject();
        GetTestJson(test_json);
        rapidjson::Document embedded_json;
        embedded_json.SetObject();
        ExeContainer exe;
        ErrorState result;
        exe.Read("embedded.json", &result);
        EXPECT_TRUE(result.Ok());
        exe.GetJson(embedded_json);
        EXPECT_EQ(embedded_json, test_json);
    }
}
