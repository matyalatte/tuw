// Functions related to json.

#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"
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
    void LoadJson(const std::string& file, rapidjson::Document& json);
    bool SaveJson(rapidjson::Document& json, const std::string& file);
    std::string JsonToString(rapidjson::Document& json);

    std::string GetString(const rapidjson::Value& json, const char* key, const char* def);
    bool GetBool(const rapidjson::Value& json, const char* key, bool def);
    int GetInt(const rapidjson::Value& json, const char* key, int def);
    double GetDouble(const rapidjson::Value& json, const char* key, double def);

    void GetDefaultDefinition(rapidjson::Document& definition);
    void CheckVersion(rapidjson::Document& definition);
    void CheckDefinition(rapidjson::Document& definition);
    void CheckSubDefinition(rapidjson::Value& sub_definition,
                            rapidjson::Document::AllocatorType& alloc);
    void CheckHelpURLs(const rapidjson::Document& definition);
}  // namespace json_utils
