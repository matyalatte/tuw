// Functions related to json.

#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include "scr_constants.h"

namespace json_utils {
    nlohmann::json LoadJson(const std::string& file);
    bool SaveJson(nlohmann::json& json, const std::string& file);

    nlohmann::json GetDefaultDefinition();
    void CheckVersion(nlohmann::json& definition);
    void CheckDefinition(nlohmann::json& definition);
    void CheckSubDefinition(nlohmann::json& sub_definition);
    void CheckHelpURLs(const nlohmann::json& definition);
}
