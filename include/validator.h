#pragma once
#include <vector>
#include "rapidjson/document.h"

#include "string_utils.h"

class Validator {
 private:
    tuwString m_regex;
    tuwString m_wildcard;
    bool m_not_empty;
    bool m_exist;
    tuwString m_regex_error;
    tuwString m_wildcard_error;
    tuwString m_not_empty_error;
    tuwString m_exist_error;
    tuwString m_error_msg;

 public:
    Validator() : m_regex(""), m_wildcard(""),
                  m_not_empty(false), m_exist(false),
                  m_error_msg("") {}
    ~Validator() {}
    void Initialize(const rapidjson::Value& j);
    bool Validate(const tuwString& str);
    const tuwString& GetError() const { return m_error_msg; }
};
