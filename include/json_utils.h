// Functions related to json.

#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include "scr_constants.h"
#include "component.h"
#include "map_as_vec.hpp"

enum CmdPredefinedIds: int {
    CMD_ID_PERCENT = -1,
    CMD_ID_CURRENT_DIR = -2
};

constexpr char CMD_TOKEN_PERCENT[] = "";
constexpr char CMD_TOKEN_CURRENT_DIR[] = "__CWD__";

namespace json_utils {
    nlohmann::json LoadJson(const std::string& file);
    bool SaveJson(nlohmann::json& json, const std::string& file);

    nlohmann::json GetDefaultDefinition();
    void CheckVersion(nlohmann::json& definition);
    void CheckDefinition(nlohmann::json& definition);
    void CheckSubDefinition(nlohmann::json& sub_definition);
    void CheckHelpURLs(const nlohmann::json& definition);
}
