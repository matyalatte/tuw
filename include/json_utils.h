// Functions related to json.

#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

namespace json_utils {
    nlohmann::json LoadJson(std::string file);
    bool SaveJson(nlohmann::json& json, std::string file);

    nlohmann::json GetDefaultDefinition();
    void CheckVersion(nlohmann::json& definition);
    void CheckDefinition(nlohmann::json& definition);
    void CheckSubDefinition(nlohmann::json& sub_definition);
    void CheckHelpURLs(const nlohmann::json& definition);
}
