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
    if (end <= position) return "";
    return input.substr(position, end - position + 1);
}

enum READ_IO_TYPE : int {
    READ_STDOUT = 0,
    READ_STDERR,
};

unsigned ReadIO(subprocess_s &process,
                int read_io_type,
                char *buf, const unsigned buf_size,
                std::string& str, const unsigned str_size) {
    unsigned read_size = 0;
    if (read_io_type == READ_STDOUT) {
        read_size = subprocess_read_stdout(&process, buf, buf_size);
    } else {
        read_size = subprocess_read_stderr(&process, buf, buf_size);
    }

    buf[read_size] = 0;
    str += buf;
    if (str.length() > str_size * 2) {
        str = str.substr(str.length() - str_size, str_size);
    }

    return read_size;
}

void DestroyProcess(subprocess_s &process, int *return_code, std::string &err_msg) {
    int result = subprocess_join(&process, return_code);
    if (0 != result) {
        *return_code = -1;
        err_msg = "Failed to join a subprocess.\n";
        return;
    }

    result = subprocess_destroy(&process);
    if (0 != result) {
        *return_code = -1;
        err_msg = "Failed to destroy a subprocess.\n";
        return;
    }
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
        out_read_size = ReadIO(process, READ_STDOUT, out_buf, BUF_SIZE, last_line, BUF_SIZE);
        err_read_size = ReadIO(process, READ_STDERR, err_buf, BUF_SIZE, err_msg, BUF_SIZE * 2);
#ifdef _WIN32
        printf("%s", out_buf);
#else
        PrintFmt("%s", out_buf);
#endif
    }

    // Sometimes stderr still have unread characters
    ReadIO(process, READ_STDERR, err_buf, BUF_SIZE, err_msg, BUF_SIZE * 2);

    int return_code;
    DestroyProcess(process, &return_code, err_msg);

    last_line = GetLastLine(last_line);

    return { return_code, err_msg, last_line };
}

ExecuteResult LaunchDefaultApp(const std::string& url) {
#ifdef _WIN32
    std::wstring utf16_url = UTF8toUTF16(url.c_str());
    const wchar_t* argv[] = {L"cmd.exe", L"/c", L"start", utf16_url.c_str(), NULL};
#elif defined(__TUW_UNIX__) && !defined(__HAIKU__)
    // Linux and BSD
    const char* argv[] = {"xdg-open", url.c_str(), NULL};
#else
    // macOS and Haiku OS
    const char* argv[] = {"open", url.c_str(), NULL};
#endif
    struct subprocess_s process;
    int options = subprocess_option_inherit_environment
                  | subprocess_option_search_user_path;
    int result = subprocess_create(argv, options, &process);
    if (0 != result)
        return { -1, "Failed to create a subprocess.\n"};

    int return_code;
    std::string err_msg = "";
    DestroyProcess(process, &return_code, err_msg);

    return { return_code, err_msg };
}
