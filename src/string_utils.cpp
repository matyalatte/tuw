#include "string_utils.h"

#include <inttypes.h>
#include <wchar.h>
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include "windows/uipriv_windows.hpp"
#endif

static StringError g_error_status = STR_OK;

StringError GetStringError() noexcept {
    return g_error_status;
}

void ClearStringError() noexcept {
    g_error_status = STR_OK;
}

inline static void SetStringError(StringError err) noexcept {
    g_error_status = err;
}

void tuwString::alloc_cstr(const char *str, size_t size) noexcept {
    clear();
    if (!str || size == 0)
        return;

    m_size = size;
    m_str = reinterpret_cast<char*>(malloc(size + 1));
    if (!m_str) {
        m_size = 0;
        SetStringError(STR_ALLOCATION_ERROR);
        return;
    }
    memcpy(m_str, str, size);
    m_str[size] = '\0';
}

#define get_strlen(str) ((str) ? strlen(str) : 0)

tuwString::tuwString(const char* str) noexcept :
        m_str(nullptr), m_size(0) {
    alloc_cstr(str, get_strlen(str));
}

tuwString::tuwString(const char* str, size_t size) noexcept :
        m_str(nullptr), m_size(0) {
    alloc_cstr(str, size);
}

tuwString::tuwString(const tuwString& str) noexcept :
        m_str(nullptr), m_size(0) {
    alloc_cstr(str.c_str(), str.size());
}

tuwString::tuwString(tuwString&& str) noexcept :
        m_str(str.m_str), m_size(str.m_size) {
    str.m_str = nullptr;
    str.m_size = 0;
}

tuwString::tuwString(size_t size) noexcept : m_size(size) {
    m_str = reinterpret_cast<char*>(calloc(size + 1, sizeof(char)));
    if (!m_str) {
        m_size = 0;
        SetStringError(STR_ALLOCATION_ERROR);
    }
}

tuwString& tuwString::operator=(const char* str) noexcept {
    alloc_cstr(str, get_strlen(str));
    return *this;
}

tuwString& tuwString::operator=(const tuwString& str) noexcept {
    if (this == &str) return *this;
    alloc_cstr(str.c_str(), str.size());
    return *this;
}

tuwString& tuwString::operator=(tuwString&& str) noexcept {
    if (this == &str) return *this;
    clear();
    m_str = str.m_str;
    m_size = str.m_size;
    str.m_str = nullptr;
    str.m_size = 0;
    return *this;
}

void tuwString::append(const char* str, size_t size) noexcept {
    if (!str || size == 0)
        return;

    size_t new_size = m_size + size;
    char* new_cstr = reinterpret_cast<char*>(malloc(new_size + 1));
    if (!new_cstr) {
        SetStringError(STR_ALLOCATION_ERROR);
        return;
    }

    if (!empty())
        memcpy(new_cstr, m_str, m_size);
    memcpy(new_cstr + m_size, str, size);
    new_cstr[new_size] = '\0';

    clear();
    m_str = new_cstr;
    m_size = new_size;
}

tuwString& tuwString::operator+=(const char* str) noexcept {
    append(str, get_strlen(str));
    return *this;
}

tuwString& tuwString::operator+=(const tuwString& str) noexcept {
    append(str.c_str(), str.size());
    return *this;
}

tuwString tuwString::operator+(const char* str) const noexcept {
    tuwString new_str(*this);
    new_str += str;
    return new_str;
}

tuwString tuwString::operator+(const tuwString& str) const noexcept {
    tuwString new_str(*this);
    new_str += str;
    return new_str;
}

// Convert number to c string, and append it to string.
# define DEFINE_PLUS_FOR_NUM(num_type, fmt) \
tuwString tuwString::operator+(num_type num) const noexcept { \
    tuwString new_str(*this); \
    \
    int num_size = snprintf(nullptr, 0, "%" fmt, num); \
    if (num_size <= 0) { \
        SetStringError(STR_FORMAT_ERROR); \
        return new_str; \
    } \
    \
    char* num_str = reinterpret_cast<char*>(malloc(num_size + 1)); \
    if (!num_str) { \
        SetStringError(STR_ALLOCATION_ERROR); \
        return new_str; \
    } \
    \
    int ret = snprintf(num_str, num_size + 1, "%" fmt, num); \
    if (ret == num_size) { \
        new_str.append(num_str, num_size); \
    } else { \
        SetStringError(STR_FORMAT_ERROR); \
    } \
    free(num_str); \
    return new_str; \
}

DEFINE_PLUS_FOR_NUM(int, "d")
DEFINE_PLUS_FOR_NUM(size_t, "zu")
DEFINE_PLUS_FOR_NUM(uint32_t, PRIu32)

#define null_to_empty(str) ((str) ? str : "")

bool tuwString::operator==(const char* str) const noexcept {
    return strcmp(c_str(), null_to_empty(str)) == 0;
}

bool tuwString::operator==(const tuwString& str) const noexcept {
    return strcmp(c_str(), str.c_str()) == 0;
}

const size_t tuwString::npos = static_cast<size_t>(-1);

size_t tuwString::find(const char c) const noexcept {
    if (empty()) return npos;
    const char* p = begin();
    for (; p < end(); p++) {
        if (*p == c)
            return static_cast<size_t>(p - m_str);
    }
    return npos;
}

size_t tuwString::find(const char* str) const noexcept {
    if (empty() || !str) return npos;

    // Note: This function uses a slow algorithm.
    const char* p = begin();
    for (; p < end(); p++) {
        const char* tmp_p = p;
        const char* str_p = str;
        while (tmp_p < end() && *str_p != '\0' && *tmp_p == *str_p) {
            tmp_p++;
            str_p++;
        }
        if (*str_p == '\0')
            return static_cast<size_t>(p - m_str);
    }
    return npos;
}

tuwString tuwString::substr(size_t start, size_t size) const noexcept {
    if (start + size > m_size) {
        SetStringError(STR_BOUNDARY_ERROR);
        return tuwString();
    }
    tuwString new_str(m_str + start, size);
    return new_str;
}

const char& tuwString::operator[](size_t id) const noexcept {
    if (id > m_size) {
        SetStringError(STR_BOUNDARY_ERROR);
        return ""[0];
    }
    return c_str()[id];
}

tuwString operator+(const char* str1, const tuwString& str2) noexcept {
    tuwString new_str(str1);
    new_str += str2;
    return new_str;
}

static inline bool IsNewline(char ch) noexcept {
    return ch == '\n' || ch == '\r';
}

tuwString GetLastLine(const tuwString& str) noexcept {
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

tuwWstring::tuwWstring(const wchar_t* str) noexcept :
        m_str(nullptr), m_size(0) {
    if (!str) return;

    size_t size = wcslen(str);
    if (size == 0)
        return;

    m_str = reinterpret_cast<wchar_t*>(malloc((size + 1) * sizeof(wchar_t)));
    if (!m_str) {
        SetStringError(STR_ALLOCATION_ERROR);
        return;
    }

    m_size = size;
    memcpy(m_str, str, size * sizeof(wchar_t));
    m_str[size] = L'\0';
}

static const uint32_t FNV_OFFSET_BASIS_32 = 2166136261U;
static const uint32_t FNV_PRIME_32 = 16777619U;

uint32_t Fnv1Hash32(const tuwString& str) noexcept {
    uint32_t hash = FNV_OFFSET_BASIS_32;
    for (const char c : str) hash = (FNV_PRIME_32 * hash) ^ c;
    return hash;
}

#ifdef _WIN32
tuwString UTF16toUTF8(const wchar_t* str) noexcept {
    char* uchar = toUTF8(str);
    tuwString ustr = uchar;
    uiprivFree(uchar);
    return ustr;
}

tuwWstring UTF8toUTF16(const char* str) noexcept {
    wchar_t* widechar = toUTF16(str);
    tuwWstring wstr = widechar;
    uiprivFree(widechar);
    return wstr;
}

void FprintFmt(FILE* out, const char* fmt, ...) noexcept {
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
    fwprintf(out, L"%ls", wfmt);

    uiprivFree(buf);
    uiprivFree(wfmt);
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
static const pango_tag GetOpeningTag(ansi_escape_code code) noexcept {
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
static const pango_tag GetClosingTag(ansi_escape_code code) noexcept {
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
    TagStack() noexcept : m_tags(), m_top(-1) {}

    void Push(ansi_escape_code code) noexcept {
        if (m_top < MAX_STACK_SIZE - 1)
            m_tags[++m_top] = code;
    }

    int Size() const noexcept { return m_top + 1; }

    void Clear() noexcept { m_top = -1; }

    using GetTagFunc = const pango_tag (*)(ansi_escape_code);

    // Get length of stacked tags
    int GetTagLength(GetTagFunc GetTag) const noexcept {
        int len = 0;
        const ansi_escape_code* max_tag = m_tags + m_top;
        for (const ansi_escape_code* code = m_tags; code <= max_tag; code++) {
            len += GetTag(*code).len;
        }
        return len;
    }

    inline int OpeningTagLength() const noexcept {
        return GetTagLength(GetOpeningTag);
    }

    inline int ClosingTagLength() const noexcept {
        return GetTagLength(GetClosingTag);
    }

    // Copy stacked tags to char*
    int CopyTag(char* output, GetTagFunc GetTag, bool reverse) const noexcept {
        char* start = output;
        for (int i = 0; i <= m_top; i++) {
            int idx = reverse ? m_top - i : i;
            const pango_tag opening_tag = GetTag(m_tags[idx]);
            memcpy(output, opening_tag.str, opening_tag.len);
            output += opening_tag.len;
        }
        return static_cast<int>(output - start);
    }

    inline int CopyOpeningTag(char* output) const noexcept {
        return CopyTag(output, GetOpeningTag, false);
    }

    inline int CopyClosingTag(char* output) const noexcept {
        return CopyTag(output, GetClosingTag, true);
    }
};

// Get string length for ConvertAnsiToPango()
int ConvertAnsiToPangoLength(TagStack* stack, const char *input) noexcept {
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
void ConvertAnsiToPango(TagStack* stack, const char *input, char *output) noexcept {
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
    tuwString m_log_buffer;
    TagStack m_tag_stack;  // stack for markup tags
    int m_buffer_length;

 public:
    Logger() noexcept : m_log_entry(NULL), m_log_buffer(""),
                        m_tag_stack(), m_buffer_length(0) {}
    ~Logger() noexcept {}

    void SetLogEntry(void* log_entry) noexcept {
        m_log_entry = static_cast<uiMultilineEntry*>(log_entry);
        if (!m_log_buffer.empty()) {
            Log(m_log_buffer.c_str());
            m_log_buffer = "";
        }
    }

     void Log(const char* str) noexcept {
        int markup_length = ConvertAnsiToPangoLength(&m_tag_stack, str);
        tuwString markup_str = tuwString(markup_length);
        if (GetStringError() != STR_OK) return;  // failed to allocate buffer
        ConvertAnsiToPango(&m_tag_stack, str, markup_str.data());
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
            uiUnixMultilineEntryMarkupAppend(m_log_entry, markup_str.data());
            uiUnixMuntilineEntryScrollToEnd(m_log_entry);
        }
    }
};

Logger g_logger = Logger();

void SetLogEntry(void* log_entry) noexcept {
    g_logger.SetLogEntry(log_entry);
}

void FprintFmt(FILE* out, const char* fmt, ...) noexcept {
    va_list va;
    va_start(va, fmt);
    va_list va2;
    va_copy(va2, va);
    size_t size = vsnprintf(NULL, 0, fmt, va2);
    va_end(va2);
    tuwString buf_str = tuwString(size);
    if (GetStringError() != STR_OK) return;  // failed to allocate buffer
    char* buf = buf_str.data();
    buf[size] = 0;
    vsnprintf(buf, size + 1, fmt, va);
    g_logger.Log(buf);
    fprintf(out, "%s", buf);
    va_end(va);
}
#endif

