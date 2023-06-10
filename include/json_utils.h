// Functions related to json.

#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include "scr_constants.h"

enum ComponentType: int {
    COMP_UNKNOWN = 0,
    COMP_STATIC_TEXT,
    COMP_FILE,
    COMP_FOLDER,
    COMP_CHOICE,
    COMP_CHECK,
    COMP_CHECK_ARRAY,
    COMP_TEXT,
    COMP_INT,
    COMP_FLOAT,
    COMP_MAX
};

namespace json_utils {
    nlohmann::json LoadJson(const std::string& file);
    bool SaveJson(nlohmann::json& json, const std::string& file);

    nlohmann::json GetDefaultDefinition();
    void CheckVersion(nlohmann::json& definition);
    void CheckDefinition(nlohmann::json& definition);
    void CheckSubDefinition(nlohmann::json& sub_definition);
    void CheckHelpURLs(const nlohmann::json& definition);
}
