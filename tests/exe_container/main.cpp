// Tests for json embedding
// Todo: Write more tests

#include <gtest/gtest.h>
#include "exe_container.h"

char const * json_file;

int main(int argc, char * argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 2);

    json_file = argv[1];

    return RUN_ALL_TESTS();
}

void GetTestJson(rapidjson::Document& json) {
    json_utils::LoadJson(json_file, json);
    EXPECT_FALSE(json.ObjectEmpty());
}

TEST(JsonEmbeddingTest, Embed) {
    {
        rapidjson::Document test_json;
        test_json.SetObject();
        GetTestJson(test_json);
        ExeContainer exe;
        wxResult result = exe.Read(json_file);
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
        wxResult result = exe.Read("embedded.json");
        EXPECT_TRUE(result.ok);
        exe.GetJson(embedded_json);
        EXPECT_EQ(embedded_json, test_json);
    }
}
