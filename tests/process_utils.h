#pragma once

#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
typedef DWORD PidType;
#else  // _WIN32
#include <unistd.h>
typedef pid_t PidType;
#endif  // _WIN32

std::vector<PidType> GetProcessIDs();
std::string GetProcessName(PidType pid);
void KillProcess(PidType pid, const std::string& name);
std::string GetDefaultBrowser();
void CloseNewBrowser(const std::vector<PidType>& old_ids);

#if defined(_WIN32) || defined(__linux__)
#ifndef USE_BROWSER
#define USE_BROWSER 1
#endif
#else
#ifndef USE_BROWSER
#define USE_BROWSER 0
#endif
#endif
