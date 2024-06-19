// Tests for json embedding
// Todo: Write more tests

#include <gtest/gtest.h>
#include "exe_container.h"
#include "string_utils.h"
#include "env_utils.h"

const char* json_file;

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 2);

#ifdef _WIN32
    std::string argv1 = UTF16toUTF8(argv[1]);
    json_file = &argv1[0];
#else
    json_file = argv[1];
#endif

    char *exe_dir = envuGetExecutableDir();
    envuSetCwd(exe_dir);
    envuFree(exe_dir);

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
        json_utils::JsonResult result = exe.Read(json_file);
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
