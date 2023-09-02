// Tests for json embedding
// Todo: Write more tests

#include <gtest/gtest.h>
#include "exe_container.h"
#include "string_utils.h"
#include "env_utils.h"

std::string json_file;

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[], wchar_t* envp[]) {
#else
int main(int argc, char* argv[], char* envp[]) {
#endif
    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 2);

#ifdef _WIN32
    json_file = UTF16toUTF8(argv[1]);
#else
    json_file = argv[1];
#endif

    env_utils::InitEnv(envp);
    std::string exe_path = env_utils::GetExecutablePath();
    env_utils::SetCwd(env_utils::GetDirectory(exe_path));

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
