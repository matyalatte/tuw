#include "std_path.h"

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>

#else
#include <sys/stat.h>
#endif

#ifndef MAX_PATH
    #define MAX_PATH  260
#endif

namespace stdpath {

    std::string g_exe_path;

#ifdef _WIN32
    void InitStdPath(const char* argv0) {
        char filename[MAX_PATH];
        GetModuleFileName(NULL, filename, MAX_PATH);
        g_exe_path = filename;
    }

    bool FileExists(const std::string& path) {
        return GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES;
    }

    std::string GetFullPath(const std::string& path) {
        char fullpath[MAX_PATH];
        GetFullPathName(path.c_str(), MAX_PATH, fullpath, nullptr);
        return fullpath;
    }

    std::string GetCwd() {
        char cwd[MAX_PATH];
        _getcwd(cwd, MAX_PATH);
        return cwd;
    }

    void SetCwd(const std::string& path) {
        _chdir(path.c_str());
    }

    int OpenURL(const std::string& path) {
        std::string cmd = "start " + path;
        return system(cmd.c_str());
    }
#else
    void InitStdPath(const char* argv0) {
        g_exe_path = "";
    }

    bool FileExists(const std::string& path) {
        struct stat buffer;
        return (stat (path.c_str(), &buffer) == 0);
    }

    std::string GetFullPath(const std::string& path) {
        return path;
    }

    std::string GetCwd() {
        char cwd[MAX_PATH];
        getcwd(cwd, MAX_PATH);
        return cwd;
    }
    void SetCwd(const std::string& path) {
        chdir(path.c_str());
    }
    int OpenURL(const std::string& path) {
    #ifdef __linux__
        std::string cmd = "xdg-open " + path;
    #else
        std::string cmd = "open " + path;
    #endif
        return system(cmd.c_str());
    }
#endif

    std::string GetDirectory(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        return (std::string::npos == pos)
         ? ""
         : path.substr(0, pos);
    }

    std::string GetExecutablePath() {
        return g_exe_path;
    }
}  // namespace stdpath
