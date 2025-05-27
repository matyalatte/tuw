#include "string_utils.h"

#include <cstring>

#ifdef _WIN32
#include "windows/uipriv_windows.hpp"
#endif

static inline bool IsNewline(char ch) noexcept {
    return ch == '\n' || ch == '\r';
}

noex::string GetLastLine(const noex::string& str) noexcept {
    if (str.empty()) return "";

    const char* begin = str.begin();
    const char* end = str.end() - 1;
    // Trim trailing line feeds.
    while (end >= begin && IsNewline(*end)) end--;
    if (end < begin) return "";

    // Search the next line feed.
    const char* sub_begin = end;
    while (sub_begin >= begin && !IsNewline(*sub_begin)) sub_begin--;
    sub_begin++;
    return str.substr(static_cast<size_t>(sub_begin - begin), end - sub_begin + 1);
}

noex::string envuStr(char *cstr) noexcept {
    noex::string str = cstr;
    envuFree(cstr);
    return str;
}

static const uint32_t FNV_OFFSET_BASIS_32 = 2166136261U;
static const uint32_t FNV_PRIME_32 = 16777619U;

uint32_t Fnv1Hash32(const char* str) noexcept {
    uint32_t hash = FNV_OFFSET_BASIS_32;
    while (*str) {
        hash = (FNV_PRIME_32 * hash) ^ *str;
        str++;
    }
    return hash;
}

#ifdef _WIN32
noex::string UTF16toUTF8(const wchar_t* str) noexcept {
    char* uchar = toUTF8(str);
    noex::string ustr = uchar;
    uiprivFree(uchar);
    return ustr;
}

noex::wstring UTF8toUTF16(const char* str) noexcept {
    wchar_t* widechar = toUTF16(str);
    noex::wstring wstr = widechar;
    uiprivFree(widechar);
    return wstr;
}

// Note: Some localized versions of Windows (e.g. Japanese, Chinese, Korean, etc.)
//       still use non-UTF encodings for char strings.
//       So, we have to convert redirected buffers to UTF8 with this function.
noex::string ANSItoUTF8(const noex::string& str) noexcept {
    int wstr_len = MultiByteToWideChar(CP_ACP, 0,
        str.c_str(), static_cast<int>(str.size() + 1), NULL, 0);

    noex::wstring wstr(wstr_len);

    int ret = MultiByteToWideChar(CP_ACP, 0,
        str.c_str(), static_cast<int>(str.size() + 1),
        wstr.data(), static_cast<int>(wstr.size()));

    if (wstr.empty() || ret != wstr_len)
        return "";  // Failed to convert
    return UTF16toUTF8(wstr.data());
}

void FprintFmt(FILE* out, const char* fmt, ...) noexcept {
    va_list va;
    va_start(va, fmt);
    size_t n = _vscprintf(fmt, va);
    noex::string buf = noex::string(n);
    vsprintf_s(buf.data(), buf.size() + 1, fmt, va);
    va_end(va);

    noex::wstring wbuf = UTF8toUTF16(buf.c_str());
    fwprintf(out, L"%ls", wbuf.c_str());
}

// Enable ANSI escape sequences on the console window.
void EnableCSI() noexcept {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

#elif defined(__TUW_UNIX__)
#include <stdarg.h>
#include "ui.h"
#include "gtk_ansi.h"

static void SetMonospaceFont(GtkWidget* text_widget) {
    /*
    If your monospace font doesn't work,
    you should make a config file to change the default font.
    ```
    <!-- ~/.config/fontconfig/fonts.conf -->
    <match target="pattern">
        <test name="family" qual="any">
            <string>monospace</string>
        </test>
        <edit binding="strong" mode="prepend" name="family">
            <string>Source Code Pro</string>
        </edit>
    </match>
    ```
    */
    GtkStyleContext* style = gtk_widget_get_style_context(text_widget);
    gtk_style_context_add_class(style, "monospace");
}

class Logger {
 private:
    uiMultilineEntry* m_log_entry;
    noex::string m_log_buffer;
    GtkAnsiParser* m_ansi_parser;

 public:
    Logger() noexcept : m_log_entry(nullptr), m_log_buffer(""),
                        m_ansi_parser() {}
    ~Logger() noexcept {
        gtk_ansi_free(m_ansi_parser);
    }

    void SetLogEntry(uiMultilineEntry* log_entry) noexcept {
        gtk_ansi_free(m_ansi_parser);
        m_ansi_parser = nullptr;
        m_log_entry = log_entry;
        if (!m_log_entry)
            return;
        GtkWidget *scrolled_window =
            reinterpret_cast<GtkWidget*>(uiControlHandle(uiControl(m_log_entry)));

        GtkWidget *text_widget = gtk_bin_get_child(GTK_BIN(scrolled_window));
        SetMonospaceFont(text_widget);

        // Make parser
        GtkTextView *text_view = GTK_TEXT_VIEW(text_widget);
        GtkTextBuffer *buf = gtk_text_view_get_buffer(text_view);
        m_ansi_parser = gtk_ansi_new(buf);
        gtk_ansi_set_default_color_with_textview(m_ansi_parser, text_view);
        if (!m_log_buffer.empty())
            Log("");
    }

    void Log(const char* str) noexcept {
        if (!m_log_entry) {
            m_log_buffer += str;
        } else {
            // Note: gtk_ansi_append returns the last few bytes
            //       when they are incomplete sequences
            const char* rest;
            m_log_buffer += str;
            rest = gtk_ansi_append(m_ansi_parser, m_log_buffer.c_str());
            m_log_buffer = rest;
            uiUnixMuntilineEntryScrollToEnd(m_log_entry);
        }
    }
};

Logger g_logger = Logger();

void SetLogEntry(void* log_entry) noexcept {
    g_logger.SetLogEntry(static_cast<uiMultilineEntry*>(log_entry));
}

void Log(const char* str) noexcept {
    g_logger.Log(str);
}

void FprintFmt(FILE* out, const char* fmt, ...) noexcept {
    va_list va;
    va_start(va, fmt);
    va_list va2;
    va_copy(va2, va);
    size_t n = vsnprintf(NULL, 0, fmt, va2);
    va_end(va2);
    noex::string buf = noex::string(n);
    vsnprintf(buf.data(), buf.size() + 1, fmt, va);
    g_logger.Log(buf.c_str());
    fprintf(out, "%s", buf.c_str());
    va_end(va);
}
#endif

