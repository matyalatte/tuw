#include <regex>
#include "validator.h"
#include "json_utils.h"
#include "env_utils.h"

void Validator::Initialize(const rapidjson::Value& j) {
    m_regex = json_utils::GetString(j, "regex", "");
    m_wildcard = json_utils::GetString(j, "wildcard", "");
    m_not_empty = json_utils::GetBool(j, "not_empty", false);
    m_exist = json_utils::GetBool(j, "exist", false);
    m_error_msg = "";
}

static std::string WildcardToRegex(const std::string& pattern) {
    std::regex star_replace("\\*");
    std::regex questionmark_replace("\\?");
    std::string wildcard_pattern = std::regex_replace(
        std::regex_replace(pattern, star_replace, ".*"),
        questionmark_replace, ".");
    return "^" + wildcard_pattern + "$";
}

bool Validator::Validate(const std::string& str) {
    if (m_wildcard != "") {
        std::regex rgx(WildcardToRegex(m_wildcard));
        if (!std::regex_match(str, rgx)) {
            m_error_msg = "Wildcard match failed for pattern: " + m_wildcard;
            return false;
        }
    }
    if (m_regex != "") {
        std::regex rgx(m_regex);
        if (!std::regex_match(str, rgx)) {
            m_error_msg = "Regular expression match failed for pattern: " + m_regex;
            return false;
        }
    }
    if (m_not_empty && str == "") {
        m_error_msg = "Empty string is NOT allowed.";
        return false;
    }
    if (m_exist && !envuPathExists(str.c_str())) {
        m_error_msg = "Path does NOT exist.";
        return false;
    }
    m_error_msg = "";
    return true;
}
