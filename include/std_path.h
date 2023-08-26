#pragma once
#include <string>

namespace stdpath {
#ifdef _WIN32
    void InitStdPath();
#else
    void InitStdPath(const char* argv0);
#endif
    std::string GetExecutablePath();
    bool FileExists(const std::string& path);
    std::string GetFullPath(const std::string& path);
    std::string GetDirectory(const std::string& path);
    std::string GetCwd();
    void SetCwd(const std::string& path);
    std::string GetHome();
    int OpenURL(const std::string& url);
}  // namespace stdpath

