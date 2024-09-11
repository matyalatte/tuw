#include "string_utils.h"

#ifdef _WIN32
#include "windows/uipriv_windows.hpp"
#else
#include <cstring>
#endif

static const uint32_t FNV_OFFSET_BASIS_32 = 2166136261U;
static const uint32_t FNV_PRIME_32 = 16777619U;

uint32_t Fnv1Hash32(const std::string& str) {
    uint32_t hash = FNV_OFFSET_BASIS_32;
    for (const char& c : str) hash = (FNV_PRIME_32 * hash) ^ c;
    return hash;
}

#ifdef _WIN32
std::string UTF16toUTF8(const wchar_t* str) {
    char* uchar = toUTF8(str);
    std::string ustr = uchar;
    uiprivFree(uchar);
    return ustr;
}

std::wstring UTF8toUTF16(const char* str) {
    wchar_t* widechar = toUTF16(str);
    std::wstring wstr = widechar;
    uiprivFree(widechar);
    return wstr;
}

void PrintFmt(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    va_list va2;
    va_copy(va2, va);
    size_t n = _vscprintf(fmt, va2);
    va_end(va2);
    n++;

    char* buf = reinterpret_cast<char *>(uiprivAlloc(n * sizeof (char), "char[]"));
    vsprintf_s(buf, n, fmt, va);
    va_end(va);

    WCHAR* wfmt = toUTF16(buf);
    wprintf(L"%ls", wfmt);

    uiprivFree(buf);
    uiprivFree(wfmt);
}

// Enable ANSI escape sequences on the console window.
void EnableCSI() {
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

// \e[XXm
typedef int ansi_escape_code;
enum ANSI_ESCAPE: ansi_escape_code {
    ANSI_RESET         = 0,
    ANSI_BOLD          = 1,
    ANSI_ITALIC        = 3,
    ANSI_UNDERLINE     = 4,
    ANSI_STRIKETHROUGH = 9,
    ANSI_FONT_BLACK    = 30,
    ANSI_FONT_RED      = 31,
    ANSI_FONT_GREEN    = 32,
    ANSI_FONT_YELLOW   = 33,
    ANSI_FONT_BLUE     = 34,
    ANSI_FONT_MAGENTA  = 35,
    ANSI_FONT_CYAN     = 36,
    ANSI_FONT_WHITE    = 37,
    ANSI_BG_BLACK      = 40,
    ANSI_BG_RED        = 41,
    ANSI_BG_GREEN      = 42,
    ANSI_BG_YELLOW     = 43,
    ANSI_BG_BLUE       = 44,
    ANSI_BG_MAGENTA    = 45,
    ANSI_BG_CYAN       = 46,
    ANSI_BG_WHITE      = 47,
};

struct pango_tag {
    const char* str;
    int len;
};

constexpr int strlen_constexpr(const char* str, int count = 0) {
    return (*str == '\0') ? count : strlen_constexpr(str + 1, count + 1);
}

constexpr pango_tag PangoTag(const char* str) {
    return { str, strlen_constexpr(str) };
}

constexpr pango_tag FONT_TAGS[] = {
    PangoTag("<span foreground='black'>"),
    PangoTag("<span foreground='red'>"),
    PangoTag("<span foreground='green'>"),
    PangoTag("<span foreground='yellow'>"),
    PangoTag("<span foreground='blue'>"),
    PangoTag("<span foreground='magenta'>"),
    PangoTag("<span foreground='cyan'>"),
    PangoTag("<span foreground='white'>")
};

constexpr pango_tag BG_TAGS[] = {
    PangoTag("<span background='black'>"),
    PangoTag("<span background='red'>"),
    PangoTag("<span background='green'>"),
    PangoTag("<span background='yellow'>"),
    PangoTag("<span background='blue'>"),
    PangoTag("<span background='magenta'>"),
    PangoTag("<span background='cyan'>"),
    PangoTag("<span background='white'>")
};

// Get opening markup tag from ANSI escape sequence
static const pango_tag GetOpeningTag(ansi_escape_code code) {
    if (code == ANSI_BOLD)
        return PangoTag("<b>");
    else if (code == ANSI_ITALIC)
        return PangoTag("<i>");
    else if (code == ANSI_UNDERLINE)
        return PangoTag("<u>");
    else if (code == ANSI_STRIKETHROUGH)
        return PangoTag("<s>");
    else if (ANSI_FONT_BLACK <= code && code <= ANSI_FONT_WHITE)
        return FONT_TAGS[code - ANSI_FONT_BLACK];
    else if (ANSI_BG_BLACK <= code && code <= ANSI_BG_WHITE)
        return BG_TAGS[code - ANSI_BG_BLACK];
    return PangoTag("");
}

// Get closing markup tag from ANSI escape sequence
static const pango_tag GetClosingTag(ansi_escape_code code) {
    if (code == ANSI_BOLD)
        return PangoTag("</b>");
    else if (code == ANSI_ITALIC)
        return PangoTag("</i>");
    else if (code == ANSI_UNDERLINE)
        return PangoTag("</u>");
    else if (code == ANSI_STRIKETHROUGH)
        return PangoTag("</s>");
    else if ((ANSI_FONT_BLACK <= code && code <= ANSI_FONT_WHITE) ||
            (ANSI_BG_BLACK <= code && code <= ANSI_BG_WHITE))
        return PangoTag("</span>");
    return PangoTag("");
}

#define MAX_STACK_SIZE 64

// Stack for markup tags
class TagStack {
 private:
    ansi_escape_code m_tags[MAX_STACK_SIZE];
    int m_top;

 public:
    TagStack() : m_tags(), m_top(-1) {}

    void Push(ansi_escape_code code) {
        if (m_top < MAX_STACK_SIZE - 1)
            m_tags[++m_top] = code;
    }

    int Size() const { return m_top + 1; }

    void Clear() { m_top = -1; }

    using GetTagFunc = const pango_tag (*)(ansi_escape_code);

    // Get length of stacked tags
    int GetTagLength(GetTagFunc GetTag) const {
        int len = 0;
        const ansi_escape_code* max_tag = m_tags + m_top;
        for (const ansi_escape_code* code = m_tags; code <= max_tag; code++) {
            len += GetTag(*code).len;
        }
        return len;
    }

    inline int OpeningTagLength() const {
        return GetTagLength(GetOpeningTag);
    }

    inline int ClosingTagLength() const {
        return GetTagLength(GetClosingTag);
    }

    // Copy stacked tags to char*
    int CopyTag(char* output, GetTagFunc GetTag, bool reverse) const {
        char* start = output;
        for (int i = 0; i <= m_top; i++) {
            int idx = reverse ? m_top - i : i;
            const pango_tag opening_tag = GetTag(m_tags[idx]);
            memcpy(output, opening_tag.str, opening_tag.len);
            output += opening_tag.len;
        }
        return static_cast<int>(output - start);
    }

    inline int CopyOpeningTag(char* output) const {
        return CopyTag(output, GetOpeningTag, false);
    }

    inline int CopyClosingTag(char* output) const {
        return CopyTag(output, GetClosingTag, true);
    }
};

// Get string length for ConvertAnsiToPango()
int ConvertAnsiToPangoLength(TagStack* stack, const char *input) {
    const char *p = input;
    int closing_tag_len = stack->ClosingTagLength();
    int len = stack->OpeningTagLength();

    while (*p) {
        if (*p == '\033' && *(p + 1) == '[') {  // Found an ANSI escape sequence
            p += 2;  // Skip "\033["
            while (*p) {
                int code = 0;
                while (*p >= '0' && *p <= '9') {
                    code = code * 10 + (*p - '0');
                    p++;
                }

                char c = *p;
                if (c == 'm' || c == ';') {
                    p++;  // Skip 'm' and ';'
                    if (code == ANSI_RESET) {
                        len += closing_tag_len;
                        closing_tag_len = 0;
                    } else {
                        len += GetOpeningTag(code).len;
                        closing_tag_len += GetClosingTag(code).len;
                    }
                    if (c == 'm')
                        break;
                }
            }
            continue;
        } else if (*p == '&') {
            len += 5;  // &amp;
        } else if (*p == '<') {
            len += 4;  // &lt;
        } else if (*p == '>') {
            len += 4;  // &gt;
        } else if (*p == '\'') {
            len += 6;  // &apos;
        } else if (*p == '"') {
            len += 6;  // &quot;
        } else {
            // Copy regular characters
            len++;
        }
        p++;
    }

    len += closing_tag_len;

    return len;
}

// Function to replace ANSI escape sequences with Pango markup
void ConvertAnsiToPango(TagStack* stack, const char *input, char *output) {
    const char *p = input;
    char *q = output;

    // Add opening tags
    q += stack->CopyOpeningTag(q);

    while (*p) {
        if (*p == '\033' && *(p + 1) == '[') {  // Found an ANSI escape sequence
            p += 2;  // Skip "\033["

            while (*p) {
                int code = 0;
                while (*p >= '0' && *p <= '9') {
                    code = code * 10 + (*p - '0');
                    p++;
                }

                char c = *p;
                if (c == 'm' || c == ';') {
                    p++;  // Skip 'm' and ';'
                    if (code == ANSI_RESET) {
                        q += stack->CopyClosingTag(q);
                        stack->Clear();
                    } else {
                        const pango_tag opening_tag = GetOpeningTag(code);
                        memcpy(q, opening_tag.str, opening_tag.len);
                        q += opening_tag.len;
                        stack->Push(code);
                    }
                    if (c == 'm')
                        break;
                }
            }
            continue;
        } else if (*p == '&') {
            memcpy(q, "&amp;", 5);
            q += 5;
        } else if (*p == '<') {
            memcpy(q, "&lt;", 4);
            q += 4;
        } else if (*p == '>') {
            memcpy(q, "&gt;", 4);
            q += 4;
        } else if (*p == '\'') {
            memcpy(q, "&apos;", 6);
            q += 6;
        } else if (*p == '"') {
            memcpy(q, "&quot;", 6);
            q += 6;
        } else {
            // Copy regular characters
            *q++ = *p;
        }
        p++;
    }

    // Add closing tags
    q += stack->CopyClosingTag(q);

    // Null-terminate the output
    *q = '\0';
}

#define MAX_LOG_BUFFER_SIZE 1024 * 1024

class Logger {
 private:
    uiMultilineEntry* m_log_entry;
    std::string m_log_buffer;
    TagStack m_tag_stack;  // stack for markup tags
    int m_buffer_length;

 public:
    Logger() : m_log_entry(NULL), m_log_buffer(""),
               m_tag_stack(), m_buffer_length(0) {}
    ~Logger() {}

    void SetLogEntry(void* log_entry) {
        m_log_entry = static_cast<uiMultilineEntry*>(log_entry);
        if (m_log_buffer != "") {
            Log(m_log_buffer.c_str());
            m_log_buffer = "";
        }
    }

    void Log(const char* str) {
        int markup_length = ConvertAnsiToPangoLength(&m_tag_stack, str);
        char *markup_str = new char[markup_length + 1];
        ConvertAnsiToPango(&m_tag_stack, str, markup_str);
        if (m_log_entry == NULL) {
            m_log_buffer += markup_str;
        } else {
            if (m_buffer_length + markup_length > MAX_LOG_BUFFER_SIZE) {
                m_buffer_length = 0;
                char* text = uiMultilineEntryText(m_log_entry);
                if (text) {
                    int text_len = strlen(text);
                    if (text_len > MAX_LOG_BUFFER_SIZE / 2) {
                        m_buffer_length = MAX_LOG_BUFFER_SIZE / 2;
                        uiMultilineEntrySetText(m_log_entry,
                                                text + text_len - MAX_LOG_BUFFER_SIZE / 2);
                    } else {
                        uiMultilineEntrySetText(m_log_entry, "");
                    }
                    uiFreeText(text);
                } else {
                    uiMultilineEntrySetText(m_log_entry, "");
                }
            }
            m_buffer_length += markup_length;
            uiUnixMultilineEntryMarkupAppend(m_log_entry, markup_str);
            uiUnixMuntilineEntryScrollToEnd(m_log_entry);
        }
    }
};

Logger g_logger = Logger();

void SetLogEntry(void* log_entry) {
    g_logger.SetLogEntry(log_entry);
}

void PrintFmt(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    va_list va2;
    va_copy(va2, va);
    size_t size = vsnprintf(NULL, 0, fmt, va2);
    va_end(va2);
    char* buf = new char[size + 1];
    buf[size] = 0;
    vsnprintf(buf, size + 1, fmt, va);
    g_logger.Log(buf);
    printf("%s", buf);
    delete[] buf;
    va_end(va);
}
#endif

