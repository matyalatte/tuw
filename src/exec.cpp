#include "exec.h"
#include <vector>
#include "subprocess.h"
#include "string_utils.h"
#ifdef _WIN32
#include "windows.h"
#endif

static std::string GetLastLine(const std::string& input) {
    if (input.length() == 0) return "";
    size_t end = input.length() - 1;
    if (input[end] == '\n') {
        if (end == 0) return "";
        end--;
    }
#ifdef _WIN32
    if (input[end] == '\r') {
        if (end == 0) return "";
        end--;
    }
#endif
    if (end == 0) return "";
    size_t position = end;
    while ((input[position] != '\n') && position > 0) position--;
    if (input[position] == '\n') position++;
    if (end - position <= 0) return "";
    return input.substr(position, end - position + 1);
}

ExecuteResult Execute(const std::string& cmd) {
#ifdef _WIN32
    std::wstring wcmd = UTF8toUTF16(cmd.c_str());

    int argc;
    wchar_t** parsed = CommandLineToArgvW(wcmd.c_str(), &argc);
    wchar_t** argv = new wchar_t*[argc + 3];
    wchar_t a[] = L"cmd.exe";
    wchar_t b[] = L"/c";
    argv[0] = &a[0];
    argv[1] = &b[0];
    for (int i = 0; i < argc; i++) {
        argv[i + 2] = parsed[i];
    }
    argv[argc + 2] = 0;
#else
    const char* argv[] = {"/bin/sh", "-c", cmd.c_str(), NULL};
#endif

    struct subprocess_s process;
    int options = subprocess_option_inherit_environment
                  | subprocess_option_search_user_path
                  | subprocess_option_enable_async;
    int result = subprocess_create(&argv[0], options, &process);

#ifdef _WIN32
    LocalFree((LPWSTR)parsed);
    delete[] argv;
#endif

    if (0 != result)
        return { -1, "Failed to create a subprocess.\n"};

    const unsigned BUF_SIZE = 1024;
    char out_buf[BUF_SIZE + 1];
    char err_buf[BUF_SIZE + 1];
    std::string last_line;
    std::string err_msg;
    unsigned out_read_size = 0;
    unsigned err_read_size = 0;

    while (subprocess_alive(&process) || out_read_size || err_read_size) {
        out_read_size = subprocess_read_stdout(&process, out_buf, BUF_SIZE);
        out_buf[out_read_size] = 0;
        err_read_size = subprocess_read_stderr(&process, err_buf, BUF_SIZE);
        err_buf[err_read_size] = 0;
#ifdef _WIN32
        printf("%s", out_buf);
#else
        PrintFmt("%s", out_buf);
#endif
        last_line += out_buf;
        err_msg += err_buf;
        if (last_line.length() > 2048) {
            last_line = last_line.substr(last_line.length() - 1024, 1024);
        }
        if (err_msg.length() > 4096) {
            err_msg = err_msg.substr(err_msg.length() - 2048, 2048);
        }
    }

    int return_code;
    result = subprocess_join(&process, &return_code);
    if (0 != result)
        return { -1, "Failed to join a subprocess.\n"};

    result = subprocess_destroy(&process);
    if (0 != result)
        return { -1, "Failed to destroy a subprocess.\n"};

    last_line = GetLastLine(last_line);

    return { return_code, err_msg, last_line };
}

ExecuteResult LaunchDefaultApp(const std::string& url) {
#ifdef _WIN32
    const wchar_t* argv[] = {L"cmd.exe", L"/c", L"start", UTF8toUTF16(url.c_str()).c_str(), NULL};
#elif defined(__linux__)
    const char* argv[] = {"xdg-open", url.c_str(), NULL};
#else
    const char* argv[] = {"open", url.c_str(), NULL};
#endif
    struct subprocess_s process;
    int options = subprocess_option_inherit_environment
                  | subprocess_option_search_user_path;
    int result = subprocess_create(argv, options, &process);
    if (0 != result)
        return { -1, "Failed to create a subprocess.\n"};

    int return_code;
    result = subprocess_join(&process, &return_code);
    if (0 != result)
        return { -1, "Failed to join a subprocess.\n"};

    result = subprocess_destroy(&process);
    if (0 != result)
        return { -1, "Failed to destroy a subprocess.\n"};

    return { 0 };
}
