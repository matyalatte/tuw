// Functions related to json.

#pragma once
#include <string>
#include "rapidjson/document.h"
#include "error_state.h"

enum CmdPredefinedIds: int {
    CMD_ID_PERCENT = -1,
    CMD_ID_CURRENT_DIR = -2,
    CMD_ID_HOME_DIR = -3
};

constexpr char CMD_TOKEN_PERCENT[] = "";
constexpr char CMD_TOKEN_CURRENT_DIR[] = "__CWD__";
constexpr char CMD_TOKEN_HOME_DIR[] = "__HOME__";

namespace json_utils {

// Max binary size for JSON files.
#define JSON_SIZE_MAX 128 * 1024

void LoadJson(const std::string& file, rapidjson::Document& json, ErrorState* result);
void SaveJson(rapidjson::Document& json, const std::string& file, ErrorState* result);
std::string JsonToString(rapidjson::Document& json);

const char* GetString(const rapidjson::Value& json, const char* key, const char* def);
bool GetBool(const rapidjson::Value& json, const char* key, bool def);
int GetInt(const rapidjson::Value& json, const char* key, int def);
double GetDouble(const rapidjson::Value& json, const char* key, double def);

void GetDefaultDefinition(rapidjson::Document& definition);
void CheckVersion(ErrorState* result, rapidjson::Document& definition);
void CheckDefinition(ErrorState* result, rapidjson::Document& definition);
void CheckSubDefinition(ErrorState* result, rapidjson::Value& sub_definition,
                        rapidjson::Document::AllocatorType& alloc);
void CheckHelpURLs(ErrorState* result, const rapidjson::Document& definition);

}  // namespace json_utils
