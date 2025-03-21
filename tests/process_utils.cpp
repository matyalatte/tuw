#include "process_utils.h"

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <shlwapi.h>
#include "string_utils.h"
#pragma comment(lib, "Shlwapi.lib")
#else  // _WIN32
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <fstream>
#include <algorithm>
#endif  // _WIN32

bool ichar_equals(char a, char b) {
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
}

// Case insensitive comparison
bool iequals(const std::string& a, const std::string& b) {
    return a.size() == b.size() &&
           std::equal(a.begin(), a.end(), b.begin(), ichar_equals);
}

void CloseNewBrowser(const std::vector<PidType>& old_ids) {
    std::vector<PidType> new_ids = GetProcessIDs();
    std::vector<std::string> browsers = {
        "chrome", "firefox", "msedge", "brave", "opera", "safari",
        GetDefaultBrowser(),
    };

    for (PidType pid : new_ids) {
        if (std::find(old_ids.begin(), old_ids.end(), pid) != old_ids.end()) continue;
        std::string name = GetProcessName(pid);
        if (std::find_if(
                browsers.begin(), browsers.end(), [&name](const std::string& browser) {
                    return iequals(browser, name);
                }) != browsers.end()) {
            KillProcess(pid, name);
        }
    }
}

#ifdef _WIN32

std::vector<PidType> GetProcessIDs() {
    std::vector<PidType> ids = {};
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE) return ids;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            ids.push_back(pe.th32ProcessID);
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return ids;
}

std::string GetAfterBackslash(const std::string& path) {
    size_t pos = path.find_last_of("\\/");
    return (pos != std::string::npos) ? path.substr(pos + 1) : path;
}

std::string GetProcessName(PidType pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess)
        return "";

    // Get module name
    std::string name = "";
    DWORD size = MAX_PATH;
    while (true) {
        std::vector<wchar_t> buffer(size + 1);
        DWORD size_tmp = size;
        if (QueryFullProcessImageNameW(hProcess, 0, buffer.data(), &size_tmp)) {
            noex::string utf8 = UTF16toUTF8(&buffer[0]);
            if (utf8.size() > 4) {
                name = std::string(utf8.data(), utf8.data() + utf8.size() - 4);
                name = GetAfterBackslash(name);
            }
            break;
        }
        if (ERROR_INSUFFICIENT_BUFFER != GetLastError() || size > 16640)
            break;
        size *= 2;
    }
    CloseHandle(hProcess);
    return name;
}

std::string GetDefaultBrowser() {
    std::string browser = "unknown";
    DWORD size = MAX_PATH;
    while (true) {
        std::vector<wchar_t> buffer(size + 1);
        DWORD size_tmp = size;
        if (AssocQueryStringW(
                ASSOCF_NONE, ASSOCSTR_EXECUTABLE,
                L"https", nullptr, buffer.data(), &size_tmp) == S_OK) {
            noex::string utf8 = UTF16toUTF8(&buffer[0]);
            if (utf8.size() > 4) {
                browser = std::string(utf8.data(), utf8.data() + utf8.size() - 4);
                browser = GetAfterBackslash(browser);
            }
            break;
        }
        if (ERROR_INSUFFICIENT_BUFFER != GetLastError() || size > 16640)
            break;
        size *= 2;
    }
    return browser;
}

void KillProcess(PidType pid, const std::string& name) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == nullptr)
        return;

    if (TerminateProcess(hProcess, 0))
        printf("Closed a browser: %s\n", name.c_str());
    CloseHandle(hProcess);
}

#else  // _WIN32

void KillProcess(PidType pid, const std::string& name) {
    if (kill(pid, SIGTERM) == 0)
        printf("Closed a browser: %s\n", name.c_str());
}

std::string GetDefaultBrowser() { return "unknown"; }

#ifdef __linux__

std::vector<PidType> GetProcessIDs() {
    std::vector<PidType> ids;
    DIR* dir = opendir("/proc");
    if (!dir) return ids;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            std::string dirName(entry->d_name);
            if (std::all_of(dirName.begin(), dirName.end(), ::isdigit)) {
                ids.push_back(static_cast<PidType>(std::stoi(dirName)));
            }
        }
    }
    closedir(dir);
    return ids;
}

std::string GetProcessName(PidType pid) {
    std::ifstream file("/proc/" + std::to_string(pid) + "/comm");
    if (!file) return "";
    std::string name;
    std::getline(file, name);
    return name;
}

#else  // __linux__

std::vector<PidType> GetProcessIDs() { return {}; }
std::string GetProcessName(PidType pid) {}

#endif  // __linux__
#endif  // _WIN32
