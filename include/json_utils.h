// Functions related to json.

#pragma once
#include "json.h"
#include "string_utils.h"

enum ComponentType: int {
    COMP_UNKNOWN = 0,
    COMP_EMPTY,
    COMP_STATIC_TEXT,
    COMP_FILE,
    COMP_FOLDER,
    COMP_COMBO,
    COMP_RADIO,
    COMP_CHECK,
    COMP_CHECK_ARRAY,
    COMP_TEXT,
    COMP_INT,
    COMP_FLOAT,
    COMP_MAX
};

enum CmdPredefinedIds: int {
    CMD_ID_PERCENT = -1,
    CMD_ID_CURRENT_DIR = -2,
    CMD_ID_HOME_DIR = -3
};

constexpr char CMD_TOKEN_PERCENT[] = "";
constexpr char CMD_TOKEN_CURRENT_DIR[] = "__CWD__";
constexpr char CMD_TOKEN_HOME_DIR[] = "__HOME__";

#ifdef _WIN32
FILE* FileOpen(const char* path, const char* mode) noexcept;
#else
#define FileOpen(path, mode) fopen(path, mode)
#endif
noex::string GetFileError(const noex::string& path) noexcept;

namespace json_utils {

struct JsonResult {
    bool ok;
    noex::string msg;
};

#define JSON_RESULT_OK { true, "" }

// Max binary size for JSON files.
#define JSON_SIZE_MAX 128 * 1024

// Returns an empty string if succeed. An error message otherwise.
noex::string LoadJson(const noex::string& file, tuwjson::Value& json) noexcept;
noex::string SaveJson(tuwjson::Value& json, const noex::string& file) noexcept;

const char* GetString(const tuwjson::Value& json, const char* key, const char* def) noexcept;
bool GetBool(const tuwjson::Value& json, const char* key, bool def) noexcept;
int GetInt(const tuwjson::Value& json, const char* key, int def) noexcept;
double GetDouble(const tuwjson::Value& json, const char* key, double def) noexcept;

void GetDefaultDefinition(tuwjson::Value& definition) noexcept;
void CheckVersion(JsonResult& result, tuwjson::Value& definition) noexcept;
void CheckDefinition(JsonResult& result, tuwjson::Value& definition) noexcept;
void CheckSubDefinition(JsonResult& result, tuwjson::Value& sub_definition,
                        int index) noexcept;
void CheckHelpURLs(JsonResult& result, tuwjson::Value& definition) noexcept;

}  // namespace json_utils
