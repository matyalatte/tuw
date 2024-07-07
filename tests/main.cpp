#include <gtest/gtest.h>

// test json files
#include "json_paths.h"

// test files
#include "exe_container_test.h"
#include "json_check_test.h"
#include "main_frame_test.h"

void GetTestJson(rapidjson::Document& json) {
    json_utils::JsonResult result = json_utils::LoadJson(JSON_ALL_KEYS, json);
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(json.ObjectEmpty());
}

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
    ::testing::InitGoogleTest(&argc, argv);

    char *exe_dir = envuGetExecutableDir();
    envuSetCwd(exe_dir);
    envuFree(exe_dir);

    MainFrameDisableDialog();

    return RUN_ALL_TESTS();
}
