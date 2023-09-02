#pragma once
#include <string>

namespace env_utils {
#ifdef _WIN32
    void InitEnv(wchar_t* envp[]);
#else
    void InitEnv(char* envp[]);
#endif
    std::string GetExecutablePath();
    bool FileExists(const std::string& path);
    std::string GetFullPath(const std::string& path);
    std::string GetDirectory(const std::string& path);
    std::string GetCwd();
    void SetCwd(const std::string& path);
    std::string GetHome();
}  // namespace stdpath

