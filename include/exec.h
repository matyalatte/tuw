#pragma once
#include "string_utils.h"

struct ExecuteResult {
    int exit_code;
    tuwString err_msg;
    tuwString last_line;
};

// When use_utf8_on_windows is true,
// Tuw converts output strings from UTF-8 to UTF-16 on Windows.
ExecuteResult Execute(const tuwString& cmd, bool use_utf8_on_windows = false);
ExecuteResult LaunchDefaultApp(const tuwString& url);
