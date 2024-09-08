#pragma once
#include <string>

struct ExecuteResult {
    int exit_code;
    std::string err_msg;
    std::string last_line;
};

// When use_utf8_on_windows is true,
// Tuw converts output strings from UTF-8 to UTF-16 on Windows.
ExecuteResult Execute(const std::string& cmd, bool use_utf8_on_windows = false);
ExecuteResult LaunchDefaultApp(const std::string& url);
