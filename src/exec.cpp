#include "exec.h"
#include "subprocess.h"
#include "string_utils.h"
#ifdef __TUW_UNIX__
#include <gtk/gtk.h>
#endif
#ifdef _WIN32
#include "windows.h"
#else
#include <time.h>
#endif

enum READ_IO_TYPE : int {
    READ_STDOUT = 0,
    READ_STDERR,
};

#define BUF_SIZE 65536
#define LAST_CHARS_MAX_LEN 2048

class RedirectContext {
 private:
#ifdef _WIN32
    HANDLE m_file;
    bool m_use_utf8_on_windows;
#else
    FILE* m_file;
#endif
    int m_io_type;
    char m_buf[BUF_SIZE + 1];
    noex::string m_last_chars;  // Stores the last characters

    noex::string TruncateStr(noex::string* str) noexcept {
        if (str->size() < LAST_CHARS_MAX_LEN)
            return *str;
        char* buf = str->data();
        char* new_buf = buf + str->size() - LAST_CHARS_MAX_LEN;
        *new_buf = '.';
        *(new_buf + 1) = '.';
        *(new_buf + 2) = '.';
        return new_buf;
    }

 public:
    explicit RedirectContext(int read_io_type, int use_utf8_on_windows) noexcept :
    #ifdef _WIN32
            m_use_utf8_on_windows(use_utf8_on_windows),
    #endif
            m_io_type(read_io_type), m_last_chars() {
    #ifdef _WIN32
        if (read_io_type == READ_STDOUT)
            m_file = GetStdHandle(STD_OUTPUT_HANDLE);
        else
            m_file = GetStdHandle(STD_ERROR_HANDLE);
    #else
        if (read_io_type == READ_STDOUT)
            m_file = stdout;
        else
            m_file = stderr;
    #endif
    }

    void RedirectOutput(subprocess_s &process) noexcept {
        unsigned read_size = 0;
        while (1) {
            // Read outputs
            if (m_io_type == READ_STDOUT)
                read_size = subprocess_read_stdout(&process, m_buf, BUF_SIZE);
            else
                read_size = subprocess_read_stderr(&process, m_buf, BUF_SIZE);
            m_buf[read_size] = 0;

            if (!read_size)
                break;

            // Store last characters
            // TODO: Use a ring buffer to remove string.erase()
            // Note: We use LAST_CHARS_MAX_LEN * 2 to avoid shifting memory
            //       with string.erase()
            if (m_last_chars.length() + read_size <= LAST_CHARS_MAX_LEN * 2) {
                m_last_chars += m_buf;
            } else if (read_size >= LAST_CHARS_MAX_LEN) {
                m_last_chars = m_buf + read_size - LAST_CHARS_MAX_LEN;
            } else {
                m_last_chars.erase(0, m_last_chars.length() + read_size - LAST_CHARS_MAX_LEN);
                m_last_chars += m_buf;
            }

            // Redirect to console
        #ifdef _WIN32
            DWORD written;
            if (m_use_utf8_on_windows) {
                noex::wstring wout = UTF8toUTF16(m_buf);
                WriteConsoleW(m_file,
                    wout.c_str(), static_cast<DWORD>(wout.size()),
                    &written, NULL);
            } else {
                WriteFile(m_file, m_buf, static_cast<DWORD>(read_size), &written, NULL);
            }
        #else  // _WIN32
        #ifdef __TUW_UNIX__
            Log(m_buf);
        #endif
            fwrite(m_buf, sizeof(char), read_size, m_file);
        #endif  // _WIN32
        }
    }

    noex::string GetLastChars() noexcept {
        return TruncateStr(&m_last_chars);
    }

    noex::string GetLine() noexcept {
        noex::string last_line = GetLastLine(m_last_chars);
        last_line = TruncateStr(&last_line);
        return last_line;
    }
};

void DestroyProcess(subprocess_s &process,
                    int *return_code, noex::string &err_msg) noexcept {
    if (subprocess_join(&process, return_code) || subprocess_destroy(&process)) {
        *return_code = -1;
        err_msg = "Failed to manage subprocess.\n";
    }
}

ExecuteResult Execute(const noex::string& cmd,
                      bool use_utf8_on_windows) noexcept {
#ifdef _WIN32
    noex::wstring wcmd = UTF8toUTF16(cmd.c_str());

    if (noex::get_error_no() != noex::OK) {
        // Reject the command as it might have unexpected value.
        return { -1,
                 "Fatal error has occored while editing strings or vectors.\n",
                 "" };
    }

    int argc;
    wchar_t** parsed = CommandLineToArgvW(wcmd.c_str(), &argc);
    wchar_t** argv = static_cast<wchar_t**>(malloc((argc + 3) * sizeof(wchar_t*)));
    if (argv == nullptr)
        return { -1, "Failed to allocate wchar_t array.\n", ""};
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
    struct timespec  ten_ms = { 0, 10 * 1000000 };  // 10ms;
#endif

    struct subprocess_s process;
    int options = subprocess_option_inherit_environment
                  | subprocess_option_search_user_path
                  | subprocess_option_enable_async;
    int result = subprocess_create(&argv[0], options, &process);

#ifdef _WIN32
    LocalFree((LPWSTR)parsed);
    free(argv);
#endif

    if (0 != result)
        return { -1, "Failed to create a subprocess.\n", ""};

    RedirectContext stdout_context(READ_STDOUT, use_utf8_on_windows);
    RedirectContext stderr_context(READ_STDERR, use_utf8_on_windows);

    do {
        stdout_context.RedirectOutput(process);
        stderr_context.RedirectOutput(process);
#ifdef __TUW_UNIX__
        // Update the console window
        while (gtk_events_pending())
            gtk_main_iteration_do(FALSE);
#endif
#ifdef _WIN32
        Sleep(10);  // wait 10ms
#else
        nanosleep(&ten_ms, nullptr);  // wait 10ms
#endif
    } while (subprocess_alive(&process));

    // Sometimes stdout and stderr still have unread characters
    stdout_context.RedirectOutput(process);
    stderr_context.RedirectOutput(process);

    // Get buffered characters from stdout and stderr
    noex::string last_line = stdout_context.GetLine();
    noex::string err_msg = stderr_context.GetLastChars();

    int return_code;
    DestroyProcess(process, &return_code, err_msg);

#ifdef _WIN32
    if (!use_utf8_on_windows)
        return { return_code, ANSItoUTF8(err_msg), ANSItoUTF8(last_line) };
#endif
    return { return_code, err_msg, last_line };
}

#ifdef _WIN32
static ExecuteResult LaunchDefaultAppBase(const wchar_t** argv) noexcept {
#else
static ExecuteResult LaunchDefaultAppBase(const char** argv) noexcept {
#endif
    struct subprocess_s process;
    int options = subprocess_option_inherit_environment
                  | subprocess_option_search_user_path;
    int result = subprocess_create(argv, options, &process);
    if (0 != result)
        return { -1, "Failed to create a subprocess.\n", ""};

    int return_code;
    noex::string err_msg;
    DestroyProcess(process, &return_code, err_msg);

    return { return_code, ANSItoUTF8(err_msg), "" };
}

ExecuteResult LaunchDefaultApp(const noex::string& url) noexcept {
#ifdef _WIN32
    noex::wstring utf16_url = UTF8toUTF16(url.c_str());
    const wchar_t* argv[] = {L"cmd.exe", L"/c", L"start", utf16_url.c_str(), NULL};
#elif defined(__TUW_UNIX__) && !defined(__HAIKU__)
    // Linux and BSD
    const char* argv[] = {"xdg-open", url.c_str(), NULL};
#else
    // macOS and Haiku OS
    const char* argv[] = {"open", url.c_str(), NULL};
#endif

    ExecuteResult res = LaunchDefaultAppBase(argv);
#if defined(__TUW_UNIX__) && !defined(__HAIKU__)
    if (res.exit_code != 0) {
        // Try "gio open" if xdg-open failed
        const char* argv2[] = {"gio", "open", url.c_str(), NULL};
        res = LaunchDefaultAppBase(argv2);
    }
#endif
    return res;
}
