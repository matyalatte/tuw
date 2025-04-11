#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include "json.h"
#include "noex/error.hpp"
#include "noex/new.hpp"

namespace tuwjson {

// Value

bool Value::operator==(const Value& val) const noexcept {
    if (m_type != val.m_type)
        return false;
    if (m_type == JSON_TYPE_OBJECT) {
        if (val.Size() != Size())
            return false;
        for (const Item& item : *val.u.m_object) {
            if (!HasMember(item.key))
                return false;
            if (*item.val != At(item.key))
                return false;
        }
    } else if (m_type == JSON_TYPE_ARRAY) {
        if (val.Size() != Size())
            return false;
        for (size_t i = 0; i < Size(); i++) {
            if (At(i) != val[i])
                return false;
        }
    } else if (m_type == JSON_TYPE_STRING) {
        return *u.m_string == *val.u.m_string;
    } else if (m_type == JSON_TYPE_INT) {
        return GetInt() == val.GetInt();
    } else if (m_type == JSON_TYPE_DOUBLE) {
        double diff = GetDouble() - val.GetDouble();
        if (diff < 0)
            diff = -diff;
        return diff < 0.00001;
    } else if (m_type == JSON_TYPE_BOOL) {
        return GetBool() == val.GetBool();
    }
    return true;
}

static noex::string LineColumnToStr(size_t line_count, size_t column) noexcept {
    if (line_count <= 0)
        return "";
    return " (line: " + noex::to_string(line_count) +
            ", column: " + noex::to_string(column) + ")";
}

noex::string Value::GetLineColumnStr() const noexcept {
    return LineColumnToStr(m_line_count, m_column);
}

void Value::FreeValue() noexcept {
    if (m_type == JSON_TYPE_OBJECT && u.m_object) {
        noex::del_ref(u.m_object);
    } else if (m_type == JSON_TYPE_ARRAY && u.m_array) {
        noex::del_ref(u.m_array);
    } else if (m_type == JSON_TYPE_STRING && u.m_string) {
        noex::del_ref(u.m_string);
    }
}

void Value::CopyFrom(const Value& val) noexcept {
    Type type = val.m_type;
    m_line_count = val.m_line_count;
    m_column = val.m_column;
    if (type == JSON_TYPE_OBJECT) {
        SetObject();
        for (const Item& item : *val.u.m_object) {
            Value& new_v = At(item.key);
            new_v.CopyFrom(*item.val);
        }
    } else if (type == JSON_TYPE_ARRAY) {
        SetArray();
        for (const Value& v : val) {
            Value new_v;
            new_v.CopyFrom(v);
            u.m_array->push_back(static_cast<Value&&>(new_v));
        }
    } else if (type == JSON_TYPE_STRING) {
        SetString(*val.u.m_string);
    } else if (type == JSON_TYPE_INT) {
        SetInt(val.u.m_int);
    } else if (type == JSON_TYPE_DOUBLE) {
        SetDouble(val.u.m_double);
    } else if (type == JSON_TYPE_BOOL) {
        SetBool(val.u.m_bool);
    }
    return;
}

void Value::Swap(Value& val) noexcept {
    Value tmp;
    tmp.MoveFrom(val);
    val.MoveFrom(*this);
    MoveFrom(tmp);
}

void Value::SetObject() noexcept {
    FreeValue();
    m_type = JSON_TYPE_OBJECT;
    u.m_object = noex::new_ref<Object>();
}

static bool object_has_member(const Object* obj, const char* key) {
    for (const Item& item : *obj) {
        if (item.key == key)
            return true;
    }
    return false;
}

bool Value::HasMember(const char* key) const noexcept {
    assert(m_type == JSON_TYPE_OBJECT);
    return object_has_member(u.m_object, key);
}

Value& Value::At(const char* key) const noexcept {
    assert(m_type == JSON_TYPE_OBJECT);
    static Value dummy{};
    for (Item& item : *u.m_object) {
        if (item.key == key) {
            if (item.val)
                return *item.val;
            break;
        }
    }
    Item item;
    item.key = key;
    u.m_object->push_back(static_cast<Item&&>(item));
    return *u.m_object->back().val;
}

void Value::ReplaceKey(const char* key, const char* new_key) noexcept {
    assert(m_type == JSON_TYPE_OBJECT);
    for (Item& item : *u.m_object) {
        if (item.key == key) {
            item.key = new_key;
            break;
        }
    }
}

void Value::ConvertToObject(const char* key) noexcept {
    Object* object = u.m_object;
    Type type = m_type;
    u.m_object = nullptr;
    m_type = JSON_TYPE_NULL;
    SetObject();
    Item item;
    item.key = key;
    item.val->u.m_object = object;
    item.val->m_type = type;
    u.m_object->push_back(static_cast<Item&&>(item));
}

size_t Value::Size() const noexcept {
    assert(m_type == JSON_TYPE_OBJECT || m_type == JSON_TYPE_ARRAY);
    if (m_type == JSON_TYPE_OBJECT)
        return u.m_object->size();
    if (m_type == JSON_TYPE_ARRAY)
        return u.m_array->size();
    return 0;
}

void Value::SetArray() noexcept {
    FreeValue();
    m_type = JSON_TYPE_ARRAY;
    u.m_array = noex::new_ref<Array>();
}

void Value::ConvertToArray() noexcept {
    Array* array = u.m_array;
    Type type = m_type;
    u.m_array = nullptr;
    m_type = JSON_TYPE_NULL;
    SetArray();
    Value val;
    val.u.m_array = array;
    val.m_type = type;
    u.m_array->push_back(static_cast<Value&&>(val));
}

void Value::SetString() noexcept {
    FreeValue();
    m_type = JSON_TYPE_STRING;
    u.m_string = noex::new_ref<noex::string>();
}

// Parser
bool Parser::SkipToValue() noexcept {
    while (true) {
        SkipSpaces();
        if (Peek() == '/' && Peek(1) == '/') {
            while (Peek() && Peek() != '\n')
                ConsumeNonSpace();
        } else if (Peek() == '/' && Peek(1) == '*') {
            ConsumeNonSpace(2);
            while (Peek() && !(Peek() == '*' && Peek(1) == '/')) {
                Consume();
            }
            if (Peek() == '*' && Peek(1) == '/') {
                ConsumeNonSpace(2);
            } else {
                m_err = JSON_ERR_UNCLOSED_COMMENT;
                return false;
            }
        } else {
            break;
        }
    }
    return true;
}

Type Parser::PeekValueType() const noexcept {
    char c = Peek();
    if (c == '{')
        return JSON_TYPE_OBJECT;
    if (c == '[')
        return JSON_TYPE_ARRAY;
    if (c == '"')
        return JSON_TYPE_STRING;
    if (c == '-' || ('0' <= c && c <= '9')) {
        const char* s = m_ptr + 1;
        while ('0' <= *s && *s <= '9')
            s++;
        if (*s != '.' && *s != 'e' && *s != 'E')
            return JSON_TYPE_INT;
        if (*s == '.') {
            s++;
            while ('0' <= *s && *s <= '9')
                s++;
        }
        if (*s == 'e' || *s == 'E') {
            s++;
            if (*s == '-')
                s++;
            while ('0' <= *s && *s <= '9')
                s++;
        }
        if (IsLiteralEnd(*s))
            return JSON_TYPE_DOUBLE;
        return JSON_TYPE_UNKNOWN;
    }
    if (IsLiteral(m_ptr, "null")) {
        return JSON_TYPE_NULL;
    }
    if (IsLiteral(m_ptr, "true") || IsLiteral(m_ptr, "false")) {
        return JSON_TYPE_BOOL;
    }
    return JSON_TYPE_UNKNOWN;
}

#define CONTROL_CHAR_MAX 0x1F  // control characters 0x00 ~ 0x1F
#define is_control_char(c) (static_cast<uint8_t>(c) <= CONTROL_CHAR_MAX)

noex::string Parser::ParseString() noexcept {
    const char* s = m_ptr + 1;
    size_t len = 0;
    while (*s && *s != '\n' && *s != '"') {
        if (*s == '\\' && s[1]) {
            s++;
        }
        len++;
        s++;
    }
    if (!*s || *s == '\n') {
        m_err = JSON_ERR_UNCLOSED_STR;
        m_ptr = s - 1;
        return "";
    }
    noex::string buf(len);
    char* cstr = buf.data();
    if (!cstr) {
        m_err = JSON_ERR_ALLOC;
        return "";
    }
    ConsumeNonSpace();
    while (Peek() != '"') {
        char c = Peek();
        if (c == '\\') {
            ConsumeNonSpace();
            c = Peek();
            if (c == '"' || c == '\\' || c == '/') {
                *cstr = c;
            } else if (c == 'b') {
                *cstr = '\b';
            } else if (c == 'f') {
                *cstr = '\f';
            } else if (c == 'n') {
                *cstr = '\n';
            } else if (c == 'r') {
                *cstr = '\r';
            } else if (c == 't') {
                *cstr = '\t';
            } else {
                if (c == 'u')
                    m_err = JSON_ERR_UNICODE_ESCAPE;
                else if (is_control_char(c))
                    m_err = JSON_ERR_CONTROL_CHAR;
                else
                    m_err = JSON_ERR_INVALID_ESCAPE;
                return "";
            }
        } else if (is_control_char(c)) {
            m_err = JSON_ERR_CONTROL_CHAR;
            return "";
        } else {
            *cstr = c;
        }
        cstr++;
        ConsumeNonSpace();
    }
    ConsumeNonSpace();
    return buf;
}

int Parser::ParseInt() noexcept {
    char* endptr;
    errno = 0;
    int value = static_cast<int>(strtol(m_ptr, &endptr, 10));
    if (errno) {
        m_err = JSON_ERR_INVALID_INT;
        return 0;
    }
    ConsumeNonSpace(static_cast<size_t>(endptr - m_ptr));
    return value;
}

double Parser::ParseDouble() noexcept {
    char* endptr;
    errno = 0;
    double value = strtod(m_ptr, &endptr);
    if (errno) {
        m_err = JSON_ERR_INVALID_DOUBLE;
        return 0.0;
    }
    ConsumeNonSpace(static_cast<size_t>(endptr - m_ptr));
    return value;
}

void Parser::ParseArray(Array* array) noexcept {
    bool comma_exists = true;
    while (true) {
        if (!SkipToValue())
            return;
        if (!Peek()) {
            m_err = JSON_ERR_UNCLOSED_ARRAY;
            return;
        }
        if (Peek() == ']') {
            ConsumeNonSpace();
            break;
        }
        if (!comma_exists) {
            m_err = JSON_ERR_UNCLOSED_ARRAY;
            return;
        }
        Value val;
        ParseValue(&val);
        array->push_back(static_cast<Value&&>(val));
        if (noex::get_error_no() != noex::OK)
            m_err = JSON_ERR_ALLOC;
        if (HasError())
            return;
        if (!SkipToValue())
            return;
        comma_exists = Peek() == ',';
        if (comma_exists)
            ConsumeNonSpace();
    }
}

void Parser::ParseObject(Object* object) noexcept {
    bool comma_exists = true;
    while (true) {
        if (!SkipToValue())
            return;
        if (!Peek()) {
            m_err = JSON_ERR_UNCLOSED_OBJECT;
            return;
        }
        if (Peek() == '}') {
            ConsumeNonSpace();
            break;
        }
        if (!comma_exists) {
            m_err = JSON_ERR_UNCLOSED_OBJECT;
            return;
        }
        if (PeekValueType() != JSON_TYPE_STRING) {
            m_err = JSON_ERR_INVALID_KEY;
            return;
        }
        Item item;
        const char* str_ptr = m_ptr;
        item.key = ParseString();
        if (HasError())
            return;
        if (object_has_member(object, item.key.c_str())) {
            m_ptr = str_ptr;
            m_err = JSON_ERR_DUPLICATED_KEY;
            return;
        }
        if (!SkipToValue())
            return;
        if (Peek() != ':') {
            m_err = JSON_ERR_EXPECTED_COLON;
            return;
        }
        ConsumeNonSpace();

        ParseValue(item.val);
        object->push_back(static_cast<Item&&>(item));
        if (noex::get_error_no() != noex::OK)
            m_err = JSON_ERR_ALLOC;
        if (HasError())
            return;
        if (!SkipToValue())
            return;
        comma_exists = Peek() == ',';
        if (comma_exists)
            ConsumeNonSpace();
    }
}

void Parser::ParseValue(Value* value) noexcept {
    if (!value)
        return;
    if (!SkipToValue())
        return;
    Type type = PeekValueType();
    value->SetLineColumn(
        m_line_count, static_cast<size_t>(m_ptr - m_line_ptr) + 1);
    if (type == JSON_TYPE_OBJECT) {
        ConsumeNonSpace();
        value->SetObject();
        Object* object = value->GetObject();
        if (!object) {
            m_err = JSON_ERR_ALLOC;
            return;
        }
        ParseObject(object);
    } else if (type == JSON_TYPE_ARRAY) {
        ConsumeNonSpace();
        value->SetArray();
        Array* array = value->GetArray();
        if (!array) {
            m_err = JSON_ERR_ALLOC;
            return;
        }
        ParseArray(array);
    } else if (type == JSON_TYPE_STRING) {
        value->SetString(ParseString());
    } else if (type == JSON_TYPE_INT) {
        value->SetInt(ParseInt());
    } else if (type == JSON_TYPE_DOUBLE) {
        value->SetDouble(ParseDouble());
    } else if (type == JSON_TYPE_BOOL) {
        bool val = Peek() == 't';
        value->SetBool(val);
        ConsumeNonSpace(val ? 4 : 5);
    } else if (type == JSON_TYPE_NULL) {
        ConsumeNonSpace(4);
    } else if (type == JSON_TYPE_UNKNOWN) {
        char c = Peek();
        if (c == ',')
            m_err = JSON_ERR_INVALID_COMMA;
        else if (c)
            m_err = JSON_ERR_UNKNOWN_LITERAL;
    }
}

#define ASCII_MAX 0x7F  // ascii 0x00 ~ 0x7F
#define MULTIBYTE_SEQ_MAX 0xBF  // sequences for multibyte characters 0x80 ~ 0xBF
#define TWO_BYTE_MIN 0xC2  // two-byte characters 0xC2 ~
#define TWO_BYTE_MAX 0xDF  // two-byte characters ~ 0xDF
#define THREE_BYTE_MAX 0xEF  // three-byte characters 0xE0 ~ 0xEF
#define FOUR_BYTE_MAX 0xF4  // four-byte characters 0xF0 ~ 0xF4
// unused codes 0xF5 ~ 0xFF

#define is_multibyte_seq(c) ((ASCII_MAX < (c)) && ((c) <= MULTIBYTE_SEQ_MAX))

bool Parser::ValidateUTF8(const char* str) noexcept {
    size_t line_count = 1;
    const char* ptr = str;
    const char* line_ptr = str;
    size_t multibyte_seq = 0;
    bool valid = true;
    while (*ptr) {
        uint8_t c = static_cast<uint8_t>(*ptr);
        if (multibyte_seq <= 0) {
            if (c <= ASCII_MAX) {
            } else if (c < TWO_BYTE_MIN) {
                valid = false;
                break;
            } else if (c <= TWO_BYTE_MAX) {
                multibyte_seq = 1;
            } else if (c <= THREE_BYTE_MAX) {
                multibyte_seq = 2;
            } else if (c <= FOUR_BYTE_MAX) {
                multibyte_seq = 3;
            } else {
                valid = false;
                break;
            }
        } else {
            if (!is_multibyte_seq(c)) {
                valid = false;
                break;
            }
            multibyte_seq--;
        }
        if (c == '\n') {
            line_count++;
            line_ptr = ptr;
        }
        ptr++;
    }
    if (!valid) {
        m_line_count = line_count;
        m_line_ptr = line_ptr;
        m_ptr = ptr;
        m_err = JSON_ERR_INVALID_UTF;
    }
    return valid;
}

Error Parser::ParseJson(const char* json, Value* root) noexcept {
    Init(json);
    if (!ValidateUTF8(json))
        return m_err;
    ParseValue(root);
    return m_err;
}

static const char* get_def_err_msg(Error err) noexcept {
    if (err == JSON_ERR_UNKNOWN_LITERAL)
        return "unknown literal detected";
    if (err == JSON_ERR_INVALID_UTF)
        return "invalid UTF8 character detected";
    if (err == JSON_ERR_INVALID_INT)
        return "failed to parse an integer";
    if (err == JSON_ERR_INVALID_DOUBLE)
        return "failed to parse a double number";
    if (err == JSON_ERR_INVALID_COMMA)
        return "there is a comma in the wrong position";
    if (err == JSON_ERR_CONTROL_CHAR)
        return "there is a control character in a string";
    if (err == JSON_ERR_INVALID_ESCAPE)
        return "invalid escaped character: \\";
    if (err == JSON_ERR_UNICODE_ESCAPE)
        return "unicode escape (\\uXXXX) is not supported. use UTF-8 characters instead";
    if (err == JSON_ERR_UNCLOSED_STR)
        return "string is not closed with '\"'";
    if (err == JSON_ERR_UNCLOSED_COMMENT)
        return "multiline comment is not closed";
    if (err == JSON_ERR_UNCLOSED_ARRAY)
        return "comma ',' or closing bracket ']' is missing";
    if (err == JSON_ERR_UNCLOSED_OBJECT)
        return "comma ',' or closing brace '}' is missing";
    if (err == JSON_ERR_EXPECTED_COLON)
        return "colon ':' is missing";
    if (err == JSON_ERR_INVALID_KEY)
        return "string key is missing";
    if (err == JSON_ERR_DUPLICATED_KEY)
        return "there is a duplicated key";
    return "";
}

const char* Parser::GetErrMsg() noexcept {
    if (m_err == JSON_OK || m_err >= JSON_ERR_MAX)
        return "";
    if (m_err == JSON_ERR_ALLOC)
        return "Memory allocation error.";

    m_err_msg = get_def_err_msg(m_err);
    if (m_err == JSON_ERR_INVALID_ESCAPE)
        m_err_msg.push_back(*m_ptr);

    size_t column = static_cast<size_t>(m_ptr - m_line_ptr) + 1;
    m_err_msg += LineColumnToStr(m_line_count, column);
    return m_err_msg.c_str();
}

void Writer::WriteChar(char c) noexcept {
    if (!m_buf)
        return;
    if (m_buf_size <= 1) {
        m_buf = nullptr;
        return;
    }
    *m_buf = c;
    m_buf++;
    m_buf_size--;
}

void Writer::WriteBytes(const char* bytes, size_t size) noexcept {
    if (!m_buf)
        return;
    if (m_buf_size <= size) {
        m_buf = nullptr;
        return;
    }
    memcpy(m_buf, bytes, size);
    m_buf += size;
    m_buf_size -= size;
}

void Writer::WriteIndent() noexcept {
    for (size_t i = 0; i < m_depth; i++)
        WriteBytes(m_indent_ptr, m_indent_size);
}

void Writer::WriteLinefeed() noexcept {
    if (m_use_linefeed)
        WriteChar('\n');
}

static bool need_escape(char c) noexcept {
    return c == '"' || c == '\\' || c == '\b' ||
            c == '\f' || c == '\n' || c == '\r' || c == '\t';
}

void Writer::WriteString(const char* str) noexcept {
    WriteChar('"');
    while (*str && m_buf) {
        char c = *str;
        if (need_escape(c))
            WriteChar('\\');
        if (c == '\b') {
            WriteChar('b');
        } else if (c == '\f') {
            WriteChar('f');
        } else if (c == '\n') {
            WriteChar('n');
        } else if (c == '\r') {
            WriteChar('r');
        } else if (c == '\t') {
            WriteChar('t');
        } else {
            WriteChar(c);
        }
        str++;
    }
    WriteChar('"');
}

void Writer::WriteObject(const Object* obj) noexcept {
    WriteChar('{');
    WriteLinefeed();
    m_depth++;
    WriteIndent();
    size_t object_size = obj->size();
    for (size_t i = 0; i < object_size; i++) {
        Item& item = obj->at(i);
        WriteString(item.key.c_str());
        WriteBytes(": ", 2);
        WriteValue(item.val);
        if (i + 1 < object_size) {
            WriteChar(',');
            WriteLinefeed();
            WriteIndent();
        }
    }
    m_depth--;
    WriteLinefeed();
    WriteIndent();
    WriteChar('}');
}

void Writer::WriteArray(const Array* ary) noexcept {
    WriteChar('[');
    WriteLinefeed();
    m_depth++;
    WriteIndent();
    size_t array_size = ary->size();
    for (size_t i = 0; i < array_size; i++) {
        WriteValue(&ary->at(i));
        if (i + 1 < array_size) {
            WriteChar(',');
            WriteLinefeed();
            WriteIndent();
        }
    }
    m_depth--;
    WriteLinefeed();
    WriteIndent();
    WriteChar(']');
}

void Writer::WriteValue(const Value* val) noexcept {
    Type type = val->GetType();
    if (type == JSON_TYPE_OBJECT) {
        WriteObject(val->GetObject());
    } else if (type == JSON_TYPE_ARRAY) {
        WriteArray(val->GetArray());
    } else if (type == JSON_TYPE_STRING) {
        WriteString(val->GetString());
    } else if (type == JSON_TYPE_INT) {
        noex::string str = noex::to_string(val->GetInt());
        WriteBytes(str.c_str(), str.size());
    } else if (type == JSON_TYPE_DOUBLE) {
        noex::string str = noex::to_string(val->GetDouble());
        WriteBytes(str.c_str(), str.size());
    } else if (type == JSON_TYPE_BOOL) {
        if (val->GetBool())
            WriteBytes("true", 4);
        else
            WriteBytes("false", 5);
    } else if (type == JSON_TYPE_NULL) {
        WriteBytes("null", 4);
    }
}

char* Writer::WriteJson(const Value* root, char* buf, size_t buf_size) noexcept {
    m_buf = buf;
    m_buf_size = buf_size;
    WriteValue(root);
    WriteLinefeed();
    if (m_buf)
        *m_buf = '\0';
    return m_buf;
}

}  // namespace tuwjson
