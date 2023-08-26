#include "std_path.h"

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#include "string_utils.h"
#else
#include <sys/stat.h>
#endif

#ifndef MAX_PATH
    #define MAX_PATH  260
#endif

namespace stdpath {

    std::string g_exe_path;

#ifdef _WIN32
    void InitStdPath() {
        wchar_t filename[MAX_PATH + 1];
        filename[MAX_PATH] = 0;
        GetModuleFileNameW(NULL, filename, MAX_PATH);
        g_exe_path = UTF16toUTF8(filename);
    }

    bool FileExists(const std::string& path) {
        std::wstring wpath = UTF8toUTF16(path.c_str());
        return GetFileAttributesW(wpath.c_str()) != INVALID_FILE_ATTRIBUTES;
    }

    std::string GetFullPath(const std::string& path) {
        std::wstring wpath = UTF8toUTF16(path.c_str());
        wchar_t fullpath[MAX_PATH + 1];
        fullpath[MAX_PATH] = 0;
        GetFullPathNameW(wpath.c_str(), MAX_PATH, fullpath, nullptr);
        return UTF16toUTF8(fullpath);
    }

    std::string GetCwd() {
        wchar_t cwd[MAX_PATH + 1];
        cwd[MAX_PATH] = 0;
        _wgetcwd(cwd, MAX_PATH);
        return UTF16toUTF8(cwd);
    }

    void SetCwd(const std::string& path) {
        std::wstring wpath = UTF8toUTF16(path.c_str());
        _wchdir(wpath.c_str());
    }

    static std::string GetEnv(const wchar_t* name) {
        size_t size;
        if (_wgetenv_s(&size, NULL, 0, name))
            return "";
        wchar_t* buf = new wchar_t[size + 1];
        buf[size] = 0;
        if (_wgetenv_s(&size, buf, size, name))
            return "";
        std::string ustr = UTF16toUTF8(buf);
        delete[] buf;
        return ustr;
    }

    std::string GetHome() {
        std::string userprof = GetEnv(L"USERPROFILE");
        if (userprof != "")
            return userprof;
        std::string drive = GetEnv(L"HOMEDRIVE");
        std::string path = GetEnv(L"HOMEPATH");
        if (drive == "")
            return "C:\\";
        if (path == "")
            return drive + "\\";
        return drive + path;
    }

    // Todo: use subprocess
    int OpenURL(const std::string& path) {
        std::string cmd = "start " + path;
        return system(cmd.c_str());
    }
#else
    // Todo: support InitStdPath()
    void InitStdPath(const char* argv0) {
        g_exe_path = "";
    }

    bool FileExists(const std::string& path) {
        struct stat buffer;
        return (stat (path.c_str(), &buffer) == 0);
    }

    // Todo: support GetFullPath()
    std::string GetFullPath(const std::string& path) {
        return path;
    }

    std::string GetCwd() {
        char cwd[MAX_PATH + 1];
        cwd[MAX_PATH] = 0;
        getcwd(cwd, MAX_PATH);
        return cwd;
    }

    void SetCwd(const std::string& path) {
        chdir(path.c_str());
    }

    // Todo: support GetHomw()
    std::string GetHome() {
        return "";
    }

    // Todo: use subprocess
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
