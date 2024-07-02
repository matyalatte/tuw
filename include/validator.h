#pragma once
#include <string>
#include <vector>
#include "rapidjson/document.h"

class Validator {
 private:
    std::string m_regex;
    std::string m_wildcard;
    bool m_not_empty;
    bool m_exist;
    std::string m_error_msg;

 public:
    Validator() : m_regex(""), m_wildcard(""),
                  m_not_empty(false), m_exist(false),
                  m_error_msg("") {}
    ~Validator() {}
    void Initialize(const rapidjson::Value& j);
    bool Validate(const std::string& str);
    std::string GetError() { return m_error_msg; }
};
