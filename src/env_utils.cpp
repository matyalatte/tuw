#include "env_utils.h"

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#include "string_utils.h"
#ifndef MAX_PATH
    #define MAX_PATH  260
#endif

#else  // _WIN32
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <limits.h>
#include <vector>
#include <cstring>
#ifndef PATH_MAX
    #define PATH_MAX  260
#endif
#endif  // _WIN32

namespace env_utils {

#ifdef _WIN32
    void InitEnv(wchar_t* envp[]) {
        while (*envp) {
            _wputenv(*envp);
            envp++;
        }
    }

    std::string GetExecutablePath() {
        wchar_t filename[MAX_PATH + 1];
        filename[MAX_PATH] = 0;
        GetModuleFileNameW(NULL, filename, MAX_PATH);
        return UTF16toUTF8(filename);
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

#else  // _WIN32
    void InitEnv(char* envp[]) {
        while (*envp) {
            putenv(*envp);
            envp++;
        }
    }

    std::string GetExecutablePath() {
        char path[PATH_MAX + 1];
        path[PATH_MAX] = 0;
    #ifdef __linux__
        const size_t LINKSIZE = 100;
        char link[LINKSIZE];
        snprintf(link, LINKSIZE, "/proc/%d/exe", getpid() );
        readlink(link, path, PATH_MAX);
    #else
        uint32_t bufsize = PATH_MAX;
        _NSGetExecutablePath(path, &bufsize);
    #endif
        if (path[0] == 0)
            return "/";
        return path;
    }

    bool FileExists(const std::string& path) {
        struct stat buffer;
        return (stat (path.c_str(), &buffer) == 0);
    }

    static void ResolvePath(const std::string& path, std::vector<std::string>& segments) {
        size_t size = path.length();
        char* buf = new char[size + 1];
        buf[size] = 0;
        memcpy(buf, path.c_str(), size);
        size_t pos = 0;
        for (size_t i = 0; i < size; i++) {
            if (buf[i] == "/"[0]) {
                buf[i] = 0;
                std::string seg = &buf[pos];
                if (seg == "..") {
                    if (!segments.empty())
                        segments.pop_back();
                } else if (seg != ".") {
                    segments.push_back(seg);
                }
                pos = i + 1;
            }
        }
        segments.push_back(&buf[pos]);
        delete[] buf;
    }

    static std::string GetFullPathOriginal(const std::string& path) {
        size_t size = path.length();
        if (size == 0)
            return "/";
        if (path[0] == "/"[0])
            // Todo: might not be a fullpath (e.g. /a/../b/./c)
            return path;

        std::vector<std::string> segments;
        ResolvePath(GetCwd(), segments);
        ResolvePath(path, segments);

        std::string fullpath;
        for (const std::string& seg : segments) {
            fullpath += seg + "/";
        }
        return fullpath.substr(0, fullpath.length() - 1);
    }

    std::string GetFullPath(const std::string& path) {
        // try the native API first.
        char* fullpath = realpath(path.c_str(), NULL);

        if (fullpath == NULL)
            return GetFullPathOriginal(path);
        std::string ret = fullpath;
        free(fullpath);
        return ret;
    }

    std::string GetCwd() {
        char cwd[PATH_MAX + 1];
        cwd[PATH_MAX] = 0;
        getcwd(cwd, PATH_MAX);
        return cwd;
    }

    void SetCwd(const std::string& path) {
        chdir(path.c_str());
    }

    std::string GetHome() {
        char buf[PATH_MAX + 1];
        buf[0] = 0;
        buf[PATH_MAX] = 0;
        struct passwd pbuf;
        struct passwd *p = NULL;
        getpwuid_r(getuid(), &pbuf, buf, PATH_MAX, &p);

        char* homedir = NULL;
        if (p != NULL)  // try to get from user info
            homedir = p->pw_dir;
        if (homedir == NULL)  // try to get from env
            homedir = getenv("HOME");
        if (homedir == NULL)  // failed to get homedir
            return "/";
        return homedir;
    }
#endif  // _WIN32

    std::string GetDirectory(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        return (std::string::npos == pos)
         ? ""
         : path.substr(0, pos);
    }
}  // namespace env_utils
