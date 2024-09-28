#include <gtest/gtest.h>

// test json files
#include "test_utils.h"

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
