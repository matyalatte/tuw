#pragma once
#include <string>

struct ExecuteResult {
    int exit_code;
    std::string err_msg;
    std::string last_line;
};

ExecuteResult Execute(const std::string& cmd);
ExecuteResult LaunchDefaultApp(const std::string& url);
