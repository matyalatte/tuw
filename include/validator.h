#pragma once
#include "json.h"

#include "string_utils.h"

class Validator {
 private:
    const char* m_regex;
    const char* m_wildcard;
    bool m_not_empty;
    bool m_exist;
    const char* m_regex_error;
    const char* m_wildcard_error;
    const char* m_not_empty_error;
    const char* m_exist_error;
    noex::string m_error_msg;

 public:
    Validator() : m_regex(nullptr), m_wildcard(nullptr),
                  m_not_empty(false), m_exist(false),
                  m_error_msg("") {}
    ~Validator() {}
    void Initialize(const tuwjson::Value& j) noexcept;
    bool Validate(const noex::string& str) noexcept;
    const noex::string& GetError() const noexcept { return m_error_msg; }
};
