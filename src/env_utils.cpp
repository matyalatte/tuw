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
#ifdef __FreeBSD__
#include <sys/param.h>
#include <sys/sysctl.h>  // for GetExecutablePathFreeBSD()
#endif
#ifdef __TUW_HAIKU__
#include <kernel/image.h>  // for GetExecutablePathHaiku()
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

#ifdef __TUW_UNIX__
#ifdef __FreeBSD__
    // FreeBSD requires sysctl to get the executable path.
    void GetExecutablePathFreeBSD(char *path) {
        size_t path_size = PATH_MAX;
        int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
        int error = sysctl(mib, 4, path, &path_size, NULL, 0);
        if (error < 0 || path_size == 0)
            path_size = 0;
        path[path_size] = 0;
    }

    std::string GetExecutablePath() {
        char path[PATH_MAX + 1];
        path[PATH_MAX] = 0;
        GetExecutablePathFreeBSD(path);
        if (path[0] == 0)
            return "/";
        return path;
    }
#elif defined(__TUW_HAIKU__)
    // Haiku OS requires its own API to get the executable path.
    std::string GetExecutablePath() {
        int32_t cookie = 0;
        image_info info;
        while (get_next_image_info(B_CURRENT_TEAM, &cookie, &info) == B_OK) {
            if (info.type == B_APP_IMAGE)
                return info.name;
        }
        return "/";
    }
#else
    // Linux distributons support readlink to get the executable path.
    int TryReadlink(const char *link, char *path, int path_size) {
        int new_path_size;
        if (path_size != 0)
            return path_size;
        new_path_size = readlink(link, path, PATH_MAX);
        if (new_path_size == -1)
            new_path_size = 0;
        return new_path_size;
    }

    void GetExecutablePathUnix(char *path) {
        int path_size = 0;
        path_size = TryReadlink("/proc/self/exe", path, path_size);  // Linux
        path_size = TryReadlink("/proc/curproc/exe", path, path_size);  // NetBSD
        path_size = TryReadlink("/proc/curproc/file", path, path_size);  // OpenBSD
        path[path_size] = 0;
    }

    std::string GetExecutablePath() {
        char path[PATH_MAX + 1];
        path[PATH_MAX] = 0;
        GetExecutablePathUnix(path);
        if (path[0] == 0)
            return "/";
        return path;
    }
#endif
#endif  // __TUW_UNIX__

#ifdef __APPLE__
    std::string GetExecutablePath() {
        char path[PATH_MAX + 1];
        path[PATH_MAX] = 0;
        uint32_t bufsize = PATH_MAX;
        _NSGetExecutablePath(path, &bufsize);
        if (path[0] == 0)
            return "/";
        return path;
    }
#endif

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
