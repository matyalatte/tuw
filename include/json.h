#pragma once

#include <assert.h>
#include "noex/string.hpp"
#include "noex/vector.hpp"
#include "noex/new.hpp"

namespace tuwjson {

/*
 * JSON module which supports RFC8259 (https://datatracker.ietf.org/doc/html/rfc8259)
 * without unicode escape (\uXXXX).
 * It also supports C style comments and trailing commas.
 * Note that this module focuses on parsing small data with small code.
 * You should use another JSON library if the performance is a consideration for you.
 */

enum Type : int {
    JSON_TYPE_NULL = 0,
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_STRING,
    JSON_TYPE_INT,
    JSON_TYPE_DOUBLE,
    JSON_TYPE_BOOL,  // true or false
    JSON_TYPE_END,  // null terminator
    JSON_TYPE_UNKNOWN,  // unsupported type
    JSON_TYPE_MAX,
};

class Value;

class Item {
 public:
    noex::string key;
    Value* val;

    Item() noexcept : key(), val() {
        val = noex::new_ref<Value>();
    }
    Item(Item&& item) noexcept :
            key(static_cast<noex::string&&>(item.key)), val(item.val) {
        item.val = nullptr;
    }

    ~Item() noexcept {
        noex::del_ref(val);
    }
};

typedef noex::vector<Item> Object;
typedef noex::vector<Value> Array;

class Value {
 private:
    Type m_type;
    size_t m_line_count;
    size_t m_column;
    union {
        Object* m_object;
        Array* m_array;
        noex::string* m_string;
        int m_int;
        double m_double;
        bool m_bool;
    } u;

 public:
    Value() noexcept :
        m_type(JSON_TYPE_NULL), m_line_count(0), m_column(0) {}
    Value(Value&& val) noexcept :
        m_type(val.m_type), m_line_count(val.m_line_count),
        m_column(val.m_column), u(val.u) {
        val.m_type = JSON_TYPE_NULL;
    }
    Value& MoveFrom(Value& val) noexcept {
        FreeValue();
        m_type = val.m_type;
        m_line_count = val.m_line_count;
        m_column = val.m_column;
        u = val.u;
        val.m_type = JSON_TYPE_NULL;
        return *this;
    }
    Value& operator=(Value&& val) noexcept {
        return MoveFrom(val);
    }
    ~Value() noexcept {
        FreeValue();
    }

    Type GetType() const noexcept {
        return m_type;
    }

    bool HasOffset() const noexcept {
        return m_line_count > 0;
    }
    void GetLineColumn(size_t* line_count, size_t* column) const noexcept {
        *line_count = m_line_count;
        *column = m_column;
    }
    void SetLineColumn(size_t line_count, size_t column) noexcept {
        m_line_count = line_count;
        m_column = column;
    }
    noex::string GetLineColumnStr() const noexcept;

    void FreeValue() noexcept;
    void CopyFrom(const Value& val) noexcept;
    void Swap(Value& val) noexcept;

    bool operator==(const Value& val) const noexcept;
    inline bool operator!=(const Value& val) const noexcept {
        return !(*this == val);
    }

    // object
    inline bool IsObject() const noexcept {
        return m_type == JSON_TYPE_OBJECT;
    }
    void SetObject() noexcept;
    Object* GetObject() const noexcept {
        assert(IsObject());
        return u.m_object;
    }
    bool HasMember(const char* key) const noexcept;
    inline bool HasMember(const noex::string& key) const noexcept {
        return HasMember(key.c_str());
    }
    Value& At(const char* key) const noexcept;
    inline Value& At(const noex::string& key) const noexcept {
        return At(key.c_str());
    }
    inline Value& operator[](const char* key) const noexcept {
        return At(key);
    }
    inline Value& operator[](const noex::string& key) const noexcept {
        return At(key.c_str());
    }
    void ReplaceKey(const char* key, const char* new_key) noexcept;
    void ConvertToObject(const char* key) noexcept;

    // array
    inline bool IsArray() const noexcept {
        return m_type == JSON_TYPE_ARRAY;
    }
    void SetArray() noexcept;
    Array* GetArray() const noexcept {
        assert(IsArray());
        return u.m_array;
    }
    Value& At(size_t id) const noexcept {
        assert(IsArray());
        assert(u.m_array->size() > id);
        return u.m_array->at(id);
    }
    inline Value& At(int id) const noexcept {
        return At(static_cast<size_t>(id));
    }
    inline Value& operator[](size_t id) const noexcept {
        return At(id);
    }
    inline Value& operator[](int id) const noexcept {
        return At(id);
    }
    Value* begin() const noexcept {
        assert(IsArray());
        return u.m_array->begin();
    }
    Value* end() const noexcept {
        assert(IsArray());
        return u.m_array->end();
    }
    size_t Size() const noexcept;
    inline bool IsEmpty() const noexcept {
        return Size() == 0;
    }
    void ConvertToArray() noexcept;
    inline void MoveAndPush(Value& val) noexcept {
        assert(IsArray());
        u.m_array->push_back(static_cast<Value&&>(val));
    }

    // string
    inline bool IsString() const noexcept {
        return m_type == JSON_TYPE_STRING;
    }
    void SetString() noexcept;
    void SetString(const char* val) noexcept {
        SetString();
        if (u.m_string)
            *u.m_string = val;
    }
    void SetString(const noex::string& str) noexcept {
        SetString();
        if (u.m_string)
            *u.m_string = str;
    }
    void SetString(noex::string&& str) noexcept {
        SetString();
        if (u.m_string)
            *u.m_string = str;
    }
    const char* GetString() const noexcept {
        assert(m_type == JSON_TYPE_STRING);
        return u.m_string->c_str();
    }

    // int
    inline bool IsInt() const noexcept {
        return m_type == JSON_TYPE_INT;
    }
    void SetInt(int val) noexcept {
        FreeValue();
        m_type = JSON_TYPE_INT;
        u.m_int = val;
    }
    int GetInt() const noexcept {
        assert(IsInt());
        return u.m_int;
    }

    // double
    inline bool IsDouble() const noexcept {
        return m_type == JSON_TYPE_INT || m_type == JSON_TYPE_DOUBLE;
    }
    void SetDouble(double val) noexcept {
        FreeValue();
        m_type = JSON_TYPE_DOUBLE;
        u.m_double = val;
    }
    double GetDouble() const noexcept {
        assert(IsDouble());
        if (IsInt())
            return static_cast<double>(u.m_int);
        return u.m_double;
    }

    // bool
    inline bool IsBool() const noexcept {
        return m_type == JSON_TYPE_BOOL;
    }
    void SetBool(bool val) noexcept {
        FreeValue();
        m_type = JSON_TYPE_BOOL;
        u.m_bool = val;
    }
    bool GetBool() const noexcept {
        assert(IsBool());
        return u.m_bool;
    }

    // null
    inline bool IsNull() const noexcept {
        return m_type == JSON_TYPE_NULL;
    }
    void SetNull() noexcept {
        FreeValue();
        m_type = JSON_TYPE_NULL;
    }
};

enum Error : int {
    JSON_OK = 0,
    JSON_ERR_ALLOC,  // Allocation error
    JSON_ERR_UNKNOWN_LITERAL,  // Unknown literal value detected.
    JSON_ERR_INVALID_UTF,  // Invalid UTF8 character detected.
    JSON_ERR_INVALID_INT,  // Failed to parse an integer.
    JSON_ERR_INVALID_DOUBLE,  // Failed to parse a double.
    JSON_ERR_INVALID_COMMA,  // There is a comma in the wrong position.
    JSON_ERR_CONTROL_CHAR,  // There is a control character in a string.
    JSON_ERR_INVALID_ESCAPE,  // Invalid escaped character. \c
    JSON_ERR_UNICODE_ESCAPE,  // Unicode escape is not supported. \uxxxx
    JSON_ERR_UNCLOSED_STR,  // " is missing.
    JSON_ERR_UNCLOSED_COMMENT,  // */ is missing.
    JSON_ERR_UNCLOSED_ARRAY,  // , or ] is missing.
    JSON_ERR_EXPECTED_COLON,  // : is missing.
    JSON_ERR_INVALID_KEY,  // String key is missing.
    JSON_ERR_DUPLICATED_KEY,  // There is a duplicated key.
    JSON_ERR_UNCLOSED_OBJECT,  // , or } is missing.
    JSON_ERR_PARSER_MAX,
    JSON_ERR_SMALL_BUFFER,  // Writer wants larger buffer.
    JSON_ERR_NUMBER_FORMAT,  // Failed to convert number to string.
    JSON_ERR_UNEXPECTED,  // Unexpected error.
    JSON_ERR_MAX,
};

class Parser {
 private:
    const char* m_ptr;
    const char* m_line_ptr;
    size_t m_line_count;
    Error m_err;
    noex::string m_err_msg;

    inline char Peek(size_t pos = 0) const noexcept {
        return *(m_ptr + pos);
    }

    void Consume() noexcept {
        if (Peek() == '\n') {
            m_line_count++;
            m_line_ptr = m_ptr + 1;
        }
        m_ptr++;
    }

    static inline bool IsSpace(char c) noexcept {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    static inline bool IsLiteralEnd(char c) noexcept {
        return !c || IsSpace(c) || c == ',' || c == ']' || c == '}';
    }

    static bool IsLiteral(const char* actual, const char* expected) noexcept {
        while (*actual && *expected) {
            if (*actual != *expected)
                return false;
            actual++;
            expected++;
        }
        return IsLiteralEnd(*actual) && !*expected;
    }

    inline void ConsumeNonSpace(size_t len = 1) noexcept {
        m_ptr += len;
    }

    void SkipSpaces() noexcept {
        while (IsSpace(Peek()))
            Consume();
    }

    bool SkipToValue() noexcept;
    Type PeekValueType() const noexcept;
    noex::string ParseString() noexcept;
    int ParseInt() noexcept;
    double ParseDouble() noexcept;
    void ParseArray(Array* array) noexcept;
    void ParseObject(Object* object) noexcept;
    void ParseValue(Value* value) noexcept;
    bool ValidateUTF8(const char* str) noexcept;
    inline size_t GetColumn() const noexcept {
        return static_cast<size_t>(m_ptr - m_line_ptr) + 1;
    }

 public:
    Parser() noexcept {
        Init(nullptr);
    }

    void Init(const char* str) noexcept {
        m_ptr = str;
        m_line_ptr = str;
        m_line_count = 1;
        m_err = JSON_OK;
        m_err_msg = "";
    }

    inline bool HasError() const noexcept {
        return m_err != JSON_OK;
    }
    inline void SetError(Error error) noexcept {
        assert(m_err == JSON_OK);
        m_err = error;
    }
    inline Error GetError() const noexcept {
        return m_err;
    }

    Error ParseJson(const char* json, Value* root) noexcept;
    inline Error ParseJson(const noex::string& json, Value* root) noexcept {
        return ParseJson(json.c_str(), root);
    }

    const char* GetErrMsg() noexcept;
};

class Writer {
 private:
    const char* m_indent_ptr;
    size_t m_indent_size;
    bool m_use_linefeed;
    size_t m_depth;

    char* m_buf;
    size_t m_buf_size;

    Error m_err;

    void WriteChar(char c) noexcept;
    void WriteBytes(const char* bytes, size_t size) noexcept;
    void WriteIndent() noexcept;
    void WriteLinefeed() noexcept;
    void WriteString(const char* str) noexcept;
    void WriteObject(const Object* obj) noexcept;
    void WriteArray(const Array* ary) noexcept;
    void WriteValue(const Value* val) noexcept;

 public:
    Writer() noexcept {
        Init(nullptr, 0, false);
    }
    Writer(const char* indent_ptr, size_t indent_size, bool use_linefeed) noexcept {
        Init(indent_ptr, indent_size, use_linefeed);
    }

    void Init(const char* indent_ptr, size_t indent_size, bool use_linefeed) noexcept {
        m_indent_ptr = indent_ptr;
        m_indent_size = indent_size;
        m_use_linefeed = use_linefeed;
        m_depth = 0;
        m_buf = nullptr;
        m_buf_size = 0;
        m_err = JSON_OK;
    }

    // returns a pointer to null terminator when succeed. returns null when failed.
    char* WriteJson(const Value* root, char* buf, size_t buf_size) noexcept;

    inline bool HasError() const noexcept {
        return m_err != JSON_OK;
    }
    inline void SetError(Error error) noexcept {
        assert(m_err == JSON_OK);
        m_err = error;
    }
    inline Error GetError() const noexcept {
        return m_err;
    }
    const char* GetErrMsg() noexcept;
};

}  // namespace tuwjson
