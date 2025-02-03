// Functions related to json.

#pragma once
#include "rapidjson/document.h"
#include "string_utils.h"

enum CmdPredefinedIds: int {
    CMD_ID_PERCENT = -1,
    CMD_ID_CURRENT_DIR = -2,
    CMD_ID_HOME_DIR = -3
};

constexpr char CMD_TOKEN_PERCENT[] = "";
constexpr char CMD_TOKEN_CURRENT_DIR[] = "__CWD__";
constexpr char CMD_TOKEN_HOME_DIR[] = "__HOME__";

#ifdef _WIN32
FILE* FileOpen(const char* path, const char* perm) noexcept;
#else
#define FileOpen(path, perm) fopen(path, perm)
#endif

namespace json_utils {

struct JsonResult {
    bool ok;
    noex::string msg;
};

#define JSON_RESULT_OK { true, "" }

// Max binary size for JSON files.
#define JSON_SIZE_MAX 128 * 1024

JsonResult LoadJson(const noex::string& file, rapidjson::Document& json) noexcept;
JsonResult SaveJson(rapidjson::Document& json, const noex::string& file) noexcept;
noex::string JsonToString(rapidjson::Document& json) noexcept;

const char* GetString(const rapidjson::Value& json, const char* key, const char* def) noexcept;
bool GetBool(const rapidjson::Value& json, const char* key, bool def) noexcept;
int GetInt(const rapidjson::Value& json, const char* key, int def) noexcept;
double GetDouble(const rapidjson::Value& json, const char* key, double def) noexcept;

void GetDefaultDefinition(rapidjson::Document& definition) noexcept;
void CheckVersion(JsonResult& result, rapidjson::Document& definition) noexcept;
void CheckDefinition(JsonResult& result, rapidjson::Document& definition) noexcept;
void CheckSubDefinition(JsonResult& result, rapidjson::Value& sub_definition,
                        int index,
                        rapidjson::Document::AllocatorType& alloc) noexcept;
void CheckHelpURLs(JsonResult& result, rapidjson::Document& definition) noexcept;

}  // namespace json_utils
