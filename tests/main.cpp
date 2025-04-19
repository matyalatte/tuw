#include <gtest/gtest.h>

// test json files
#include "test_utils.h"
#include "process_utils.h"

#ifdef USE_BROWSER
// Some tests launch browsers. We close them after testing.
class BrowserCloser : public ::testing::Environment {
 private:
    std::vector<PidType> ids;

 public:
    virtual void SetUp() {
        ids = GetProcessIDs();
    }
    virtual void TearDown() {
        CloseNewBrowser(ids);
    }
};
#endif

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

#ifdef USE_BROWSER
    ::testing::Environment* const foo_env =
        ::testing::AddGlobalTestEnvironment(new BrowserCloser);
    (void) foo_env;
#endif
    return RUN_ALL_TESTS();
}
