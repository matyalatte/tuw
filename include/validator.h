#pragma once
#include "json.h"

#include "string_utils.h"

class Validator {
 private:
    noex::string m_regex;
    noex::string m_wildcard;
    bool m_not_empty;
    bool m_exist;
    noex::string m_regex_error;
    noex::string m_wildcard_error;
    noex::string m_not_empty_error;
    noex::string m_exist_error;
    noex::string m_error_msg;

 public:
    Validator() : m_regex(""), m_wildcard(""),
                  m_not_empty(false), m_exist(false),
                  m_error_msg("") {}
    ~Validator() {}
    void Initialize(const tuwjson::Value& j) noexcept;
    bool Validate(const noex::string& str) noexcept;
    const noex::string& GetError() const noexcept { return m_error_msg; }
};
