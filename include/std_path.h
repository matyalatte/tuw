#pragma once
#include <string>

namespace stdpath {
    void InitStdPath(const char* argv0);
    std::string GetExecutablePath();
    bool FileExists(const std::string& path);
    std::string GetFullPath(const std::string& path);
    std::string GetDirectory(const std::string& path);
    std::string GetCwd();
    void SetCwd(const std::string& path);
    int OpenURL(const std::string& url);
}  // namespace stdpath

