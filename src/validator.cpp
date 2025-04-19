#include "str_match.h"
#include "validator.h"
#include "json_utils.h"
#include "env_utils.h"
#include "string_utils.h"

void Validator::Initialize(const tuwjson::Value& j) noexcept {
    m_regex = json_utils::GetString(j, "regex", nullptr);
    m_regex_error = json_utils::GetString(j, "regex_error", nullptr);
    m_wildcard = json_utils::GetString(j, "wildcard", nullptr);
    m_wildcard_error = json_utils::GetString(j, "wildcard_error", nullptr);
    m_not_empty = json_utils::GetBool(j, "not_empty", false);
    m_not_empty_error = json_utils::GetString(
        j, "not_empty_error", "Empty string is NOT allowed.");
    m_exist = json_utils::GetBool(j, "exist", false);
    m_exist_error = json_utils::GetString(
        j, "exist_error", "Path does NOT exist.");
    m_error_msg = "";
}

static int IsUnsupportedPattern(const char *pattern) noexcept {
    // () operators are unsupported in tiny-regex-c
    // https://github.com/matyalatte/tiny-str-match?tab=readme-ov-file#supported-regex-operators
    const char* p = pattern;
    while (*p != '\0') {
        if (*p == '\\') {
            p++;
            if (*p == '\0')
                break;
        } else if (*p == '(' || *p == ')') {
            return 1;
        }
        p++;
    }
    return 0;
}

bool Validator::Validate(const noex::string& str) noexcept {
    if (m_not_empty && str.empty()) {
        m_error_msg = m_not_empty_error;
        return false;
    }
    if (m_wildcard) {
        if (tsm_wildcard_match(m_wildcard, str.c_str()) != TSM_OK) {
            if (m_wildcard_error)
                m_error_msg = m_wildcard_error;
            else
                m_error_msg = noex::concat_cstr("Wildcard match failed for pattern: ", m_wildcard);
            return false;
        }
    }
    if (m_regex) {
        if (IsUnsupportedPattern(m_regex)) {
            m_error_msg = "Regex compile error: () operators are not supported.";
            return false;
        }
        int res = tsm_regex_match(m_regex, str.c_str());
        if (res != TSM_OK && m_regex_error) {
            m_error_msg = m_regex_error;
            return false;
        }
        if (res == TSM_FAIL) {
            m_error_msg = noex::concat_cstr("Regex match failed for pattern: ", m_regex);
            return false;
        } else if (res == TSM_SYNTAX_ERROR) {
            m_error_msg = noex::concat_cstr("Failed to parse regex pattern: ", m_regex);
            return false;
        }
    }
    if (m_exist && !envuPathExists(str.c_str())) {
        m_error_msg = m_exist_error;
        return false;
    }
    m_error_msg = "";
    return true;
}
