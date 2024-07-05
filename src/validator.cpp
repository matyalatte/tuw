#include "str_match.h"
#include "validator.h"
#include "json_utils.h"
#include "env_utils.h"

void Validator::Initialize(const rapidjson::Value& j) {
    m_regex = json_utils::GetString(j, "regex", "");
    m_regex_error = json_utils::GetString(j, "regex_error", "");
    m_wildcard = json_utils::GetString(j, "wildcard", "");
    m_wildcard_error = json_utils::GetString(j, "wildcard_error", "");
    m_not_empty = json_utils::GetBool(j, "not_empty", false);
    m_not_empty_error = json_utils::GetString(j, "not_empty_error", "");
    m_exist = json_utils::GetBool(j, "exist", false);
    m_exist_error = json_utils::GetString(j, "exist_error", "");
    m_error_msg = "";
}

static int IsUnsupportedPattern(const char *pattern) {
    // () operators are unsupported in tiny-regex-c
    // https://github.com/matyalatte/tiny-str-match?tab=readme-ov-file#supported-regex-operators
    const char* p = pattern;
    int escaped = 0;
    while (*p != '\0') {
        if (*p == '\\') {
            escaped = 1;
            p++;
            continue;
        }
        if (!escaped && (*p == '(' || *p == ')')) {
            return 1;
        }
        escaped = 0;
        p++;
    }
    return 0;
}

bool Validator::Validate(const std::string& str) {
    if (m_wildcard != "") {
        if (tsm_wildcard_match(m_wildcard.c_str(), str.c_str()) != TSM_OK) {
            if (m_wildcard_error == "")
                m_error_msg = "Wildcard match failed for pattern: " + m_wildcard;
            else
                m_error_msg = m_wildcard_error;
            return false;
        }
    }
    if (m_regex != "") {
        if (IsUnsupportedPattern(m_regex.c_str())) {
            m_error_msg = "Sorry, () operators are not supported.";
            return false;
        }
        int res = tsm_regex_match(m_regex.c_str(), str.c_str());
        if (res != TSM_OK && m_regex_error != "") {
            m_error_msg = m_regex_error;
            return false;
        }
        if (res == TSM_FAIL) {
            m_error_msg = "Regex match failed for pattern: " + m_regex;
            return false;
        } else if (res == TSM_SYNTAX_ERROR) {
            m_error_msg = "Failed to parse regex pattern: " + m_regex;
            return false;
        }
    }
    if (m_not_empty && str == "") {
        if (m_not_empty_error == "")
            m_error_msg = "Empty string is NOT allowed.";
        else
            m_error_msg = m_not_empty_error;
        return false;
    }
    if (m_exist && !envuPathExists(str.c_str())) {
        if (m_exist_error == "")
            m_error_msg = "Path does NOT exist.";
        else
            m_error_msg = m_exist_error;
        return false;
    }
    m_error_msg = "";
    return true;
}
