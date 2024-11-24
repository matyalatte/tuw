#pragma once
#include "string_utils.h"

struct ExecuteResult {
    int exit_code;
    noex::string err_msg;
    noex::string last_line;
};

// When use_utf8_on_windows is true,
// Tuw converts output strings from UTF-8 to UTF-16 on Windows.
ExecuteResult Execute(const noex::string& cmd,
                      bool use_utf8_on_windows = false) noexcept;
ExecuteResult LaunchDefaultApp(const noex::string& url) noexcept;
