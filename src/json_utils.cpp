#include "json_utils.h"
#include <cstdio>
#include <cassert>

#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"

#include "tuw_constants.h"
#include "string_utils.h"
#include "noex/vector.hpp"

#ifdef _WIN32
FILE* FileOpen(const char* path, const char* mode) noexcept {
    // Use wfopen as fopen might not use utf-8.
    noex::wstring wpath = UTF8toUTF16(path);
    noex::wstring wmode = UTF8toUTF16(mode);
    if (wpath.empty() || wmode.empty())
        return nullptr;
    return _wfopen(wpath.c_str(), wmode.c_str());
}
#endif

namespace json_utils {

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

// JSON parser allows c style comments and trailing commas.
constexpr auto JSONC_FLAGS =
    rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag;

noex::string LoadJson(const noex::string& file, rapidjson::Document& json) noexcept {
    FILE* fp = FileOpen(file.c_str(), "rb");
    if (!fp)
        return "Failed to open " + file;

    char readBuffer[JSON_SIZE_MAX];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::ParseResult ok = json.ParseStream<JSONC_FLAGS>(is);
    fclose(fp);

    if (!ok) {
        return noex::concat_cstr("Failed to parse JSON: ",
                rapidjson::GetParseError_En(ok.Code()),
                " (offset: ") + ok.Offset() + ")";
    }
    if (!json.IsObject())
        json.SetObject();

    return "";
}

noex::string SaveJson(rapidjson::Document& json, const noex::string& file) noexcept {
    FILE* fp = FileOpen(file.c_str(), "wb");
    if (!fp)
        return noex::concat_cstr("Failed to open ", file.c_str(), ".");

    char writeBuffer[JSON_SIZE_MAX];
    rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
    json.Accept(writer);
    fclose(fp);
    return "";
}

noex::string JsonToString(rapidjson::Document& json) noexcept {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    return buffer.GetString();
}

const char* GetString(const rapidjson::Value& json, const char* key, const char* def) noexcept {
    if (json.HasMember(key))
        return json[key].GetString();
    return def;
}

bool GetBool(const rapidjson::Value& json, const char* key, bool def) noexcept {
    if (json.HasMember(key))
        return json[key].GetBool();
    return def;
}

int GetInt(const rapidjson::Value& json, const char* key, int def) noexcept {
    if (json.HasMember(key))
        return json[key].GetInt();
    return def;
}

double GetDouble(const rapidjson::Value& json, const char* key, double def) noexcept {
    if (json.HasMember(key))
        return json[key].GetDouble();
    return def;
}

static noex::string GetLabel(const char* label, const char* key) noexcept {
    noex::string msg;
    if (*label != '\0') {
        msg = noex::concat_cstr("['", label, "']");
    }
    msg += noex::concat_cstr("['", key, "']");
    return msg;
}

enum class JsonType {
    BOOLEAN,
    INTEGER,
    FLOAT,
    STRING,
    JSON,
    MAX
};

static const bool OPTIONAL = true;

static void CheckJsonType(JsonResult& result, const rapidjson::Value& j, const char* key,
        const JsonType& type, const char* label = "", const bool& optional = false) noexcept {
    if (!j.HasMember(key)) {
        if (optional) return;
        result.ok = false;
        result.msg = GetLabel(label, key) + " not found.";
        return;
    }
    bool valid = false;
    const char* type_name = nullptr;
    switch (type) {
    case JsonType::BOOLEAN:
        valid = j[key].IsBool();
        type_name = "a boolean";
        break;
    case JsonType::INTEGER:
        valid = j[key].IsInt();
        type_name = "an int";
        break;
    case JsonType::FLOAT:
        valid = j[key].IsDouble() || j[key].IsInt();
        type_name = "a float";
        break;
    case JsonType::STRING:
        valid = j[key].IsString();
        type_name = "a string";
        break;
    case JsonType::JSON:
        valid = j[key].IsObject();
        type_name = "a json object";
        break;
    default:
        assert(false);
        type_name = "";
        break;
    }
    if (!valid) {
        result.ok = false;
        result.msg = GetLabel(label, key) + noex::concat_cstr(" should be ", type_name, ".");
    }
}

static bool IsJsonArray(rapidjson::Value& j, const char* key,
                        rapidjson::Document::AllocatorType& alloc) noexcept {
    if (!j[key].IsArray()) {
        if (!j[key].IsObject())
            return false;
        rapidjson::Value array(rapidjson::kArrayType);
        array.PushBack(j[key].Move(), alloc);
        j[key] = array;
    }
    for (const rapidjson::Value& el : j[key].GetArray()) {
        if (!el.IsObject())
            return false;
    }
    return true;
}

static bool IsStringArray(rapidjson::Value& j, const char* key,
                            rapidjson::Document::AllocatorType& alloc) noexcept {
    if (!j[key].IsArray()) {
        if (!j[key].IsString())
            return false;
        rapidjson::Value array(rapidjson::kArrayType);
        array.PushBack(j[key].Move(), alloc);
        j[key] = array;
    }
    for (const rapidjson::Value& el : j[key].GetArray()) {
        if (!el.IsString())
            return false;
    }
    return true;
}

static void CheckJsonArrayType(JsonResult& result, rapidjson::Value& j, const char* key,
        const JsonType& type, rapidjson::Document::AllocatorType& alloc,
        const char* label = "", const bool& optional = false) noexcept {
    if (!j.HasMember(key)) {
        if (optional) return;
        result.ok = false;
        result.msg = GetLabel(label, key) + " not found.";
        return;
    }
    bool valid = false;
    const char* type_name = nullptr;
    switch (type) {
    case JsonType::STRING:
        valid = IsStringArray(j, key, alloc);
        type_name = "an array of strings";
        break;
    case JsonType::JSON:
        valid = IsJsonArray(j, key, alloc);
        type_name = "an array of json objects";
        break;
    default:
        assert(false);
        type_name = "";
        break;
    }
    if (!valid) {
        result.ok = false;
        result.msg = GetLabel(label, key) + noex::concat_cstr(" should be ", type_name, ".");
    }
}

// get default definition of gui
void GetDefaultDefinition(rapidjson::Document& definition) noexcept {
    static const char* def_str = "{"
#ifdef _WIN32
        "\"command\":\"dir\","
        "\"button\":\"run 'dir'\","
#else
        "\"command\":\"ls\","
        "\"button\":\"run 'ls'\","
#endif
        "\"components\":[]"
        "}";
    rapidjson::ParseResult ok = definition.Parse(def_str);
    assert(ok);
    (void) ok;  // GCC says it's unused even if you use it for assertion.
    JsonResult result = JSON_RESULT_OK;
    CheckDefinition(result, definition);
    assert(result.ok);
}

static void CorrectKey(
        rapidjson::Value& j,
        const char* false_key, const char* true_key,
        rapidjson::Document::AllocatorType& alloc) noexcept {
    if (!j.HasMember(true_key) && j.HasMember(false_key)) {
        rapidjson::Value n(true_key, alloc);
        j.AddMember(n, j[false_key], alloc);
        j.RemoveMember(false_key);
    }
}

static noex::vector<noex::string>
SplitString(const char* str, const char delimiter) noexcept {
    if (!str)
        return {};

    noex::vector<noex::string> tokens;

    const char* start = str;
    while (*start != '\0') {
        const char* pos = strchr(start, delimiter);
        if (!pos) {
            tokens.emplace_back(start);
            break;
        }
        tokens.emplace_back(start, pos - start);
        start = pos + 1;
    }
    return tokens;
}

static void CheckIndexDuplication(
        JsonResult& result, const noex::vector<noex::string>& component_ids) noexcept {
    size_t size = component_ids.size();
    if (size == 0)
        return;
    for (size_t i = 0; i < size - 1; i++) {
        const noex::string& str = component_ids[i];
        if (str.empty())
            continue;
        for (size_t j = i + 1; j < size; j++) {
            if (str == component_ids[j]) {
                result.ok = false;
                result.msg = noex::concat_cstr("[components][id]"
                                " should not be duplicated in a gui definition. (",
                                str.c_str(), ")");
                return;
            }
        }
    }
}

// split command by "%" symbol, and calculate which component should be inserted there.
static void CompileCommand(JsonResult& result,
                            rapidjson::Value& sub_definition,
                            const noex::vector<noex::string>& comp_ids,
                            rapidjson::Document::AllocatorType& alloc) noexcept {
    noex::vector<noex::string> cmd = SplitString(sub_definition["command"].GetString(), '%');
    noex::vector<noex::string> cmd_ids;
    noex::vector<noex::string> splitted_cmd;
    if (sub_definition.HasMember("command_splitted"))
        sub_definition.RemoveMember("command_splitted");
    rapidjson::Value splitted_cmd_json(rapidjson::kArrayType);

    bool store_ids = false;
    for (const noex::string& token : cmd) {
        if (store_ids) {
            cmd_ids.emplace_back(token);
        } else {
            splitted_cmd.emplace_back(token);
            rapidjson::Value n(token.c_str(), alloc);
            splitted_cmd_json.PushBack(n, alloc);
        }
        store_ids = !store_ids;
    }
    sub_definition.AddMember("command_splitted", splitted_cmd_json, alloc);

    rapidjson::Value& components = sub_definition["components"];;
    rapidjson::Value cmd_int_ids(rapidjson::kArrayType);
    noex::string cmd_str;
    int comp_size = static_cast<int>(comp_ids.size());
    int non_id_comp = 0;
    for (int i = 0; i < static_cast<int>(cmd_ids.size()); i++) {
        cmd_str += splitted_cmd[i];
        const noex::string& id = cmd_ids[i];
        int j;
        if (id == CMD_TOKEN_PERCENT) {
            j = CMD_ID_PERCENT;
            cmd_str.push_back('%');
        } else if (id == CMD_TOKEN_CURRENT_DIR) {
            j = CMD_ID_CURRENT_DIR;
            cmd_str += id;
        } else if (id == CMD_TOKEN_HOME_DIR) {
            j = CMD_ID_HOME_DIR;
            cmd_str += id;
        } else {
            for (j = 0; j < comp_size; j++)
                if (id == comp_ids[j]) break;
            if (j == comp_size) {
                while (non_id_comp < comp_size
                    && (components[non_id_comp]["type_int"] == COMP_STATIC_TEXT
                        || !comp_ids[non_id_comp].empty()
                        || components[non_id_comp]["type_int"] == COMP_EMPTY)) {
                    non_id_comp++;
                }
                j = non_id_comp;
                non_id_comp++;
            }
        }
        if (j < comp_size)
            cmd_int_ids.PushBack(j, alloc);
        if (j >= comp_size)
            cmd_str += "__comp???__";
        else if (j >= 0)
            cmd_str += noex::concat_cstr("__comp", noex::to_string(j).c_str(), "__");
    }
    if (cmd_ids.size() < splitted_cmd.size())
        cmd_str += splitted_cmd.back();

    // Check if the command requires more arguments or ignores some arguments.
    for (int j = 0; j < comp_size; j++) {
        int type_int = components[j]["type_int"].GetInt();
        if (type_int == COMP_STATIC_TEXT || type_int == COMP_EMPTY)
            continue;
        bool found = false;
        for (rapidjson::Value& id : cmd_int_ids.GetArray())
            if (id.GetInt() == j) {
                found = true;
                break;
            }
        if (!found) {
            result.ok = false;
            result.msg = noex::concat_cstr("[\"components\"][", noex::to_string(j).c_str(),
                            "] is unused in the command; ") + cmd_str;
            if (!comp_ids[j].empty())
                result.msg = "The ID of " + result.msg;
            return;
        }
    }
    if (non_id_comp > comp_size) {
        result.ok = false;
        result.msg =
            "The command requires more components for arguments; " + cmd_str;
        return;
    }
    if (sub_definition.HasMember("command_str"))
        sub_definition.RemoveMember("command_str");
    rapidjson::Value v(cmd_str.c_str(), alloc);
    sub_definition.AddMember("command_str", v, alloc);
    if (sub_definition.HasMember("command_ids"))
        sub_definition.RemoveMember("command_ids");
    sub_definition.AddMember("command_ids", cmd_int_ids, alloc);
}

// don't use map. it will make exe larger.
int ComptypeToInt(const char* comptype) noexcept {
    if (strcmp(comptype, "static_text") == 0)
        return COMP_STATIC_TEXT;
    if (strcmp(comptype, "file") == 0)
        return COMP_FILE;
    if (strcmp(comptype, "folder") == 0)
        return COMP_FOLDER;
    if (strcmp(comptype, "dir") == 0)
        return COMP_FOLDER;
    if (strcmp(comptype, "choice") == 0)
        return COMP_COMBO;
    if (strcmp(comptype, "combo") == 0)
        return COMP_COMBO;
    if (strcmp(comptype, "radio") == 0)
        return COMP_RADIO;
    if (strcmp(comptype, "check") == 0)
        return COMP_CHECK;
    if (strcmp(comptype, "check_array") == 0)
        return COMP_CHECK_ARRAY;
    if (strcmp(comptype, "checks") == 0)
        return COMP_CHECK_ARRAY;
    if (strcmp(comptype, "text") == 0)
        return COMP_TEXT;
    if (strcmp(comptype, "text_box") == 0)
        return COMP_TEXT;
    if (strcmp(comptype, "int") == 0)
        return COMP_INT;
    if (strcmp(comptype, "integer") == 0)
        return COMP_INT;
    if (strcmp(comptype, "float") == 0)
        return COMP_FLOAT;
    return COMP_UNKNOWN;
}

void CheckValidator(JsonResult& result, rapidjson::Value& validator,
                    const char* label) noexcept {
    CheckJsonType(result, validator, "regex", JsonType::STRING, label, OPTIONAL);
    CheckJsonType(result, validator, "regex_error", JsonType::STRING, label, OPTIONAL);
    CheckJsonType(result, validator, "wildcard", JsonType::STRING, label, OPTIONAL);
    CheckJsonType(result, validator, "wildcard_error", JsonType::STRING, label, OPTIONAL);
    CheckJsonType(result, validator, "exist", JsonType::BOOLEAN, label, OPTIONAL);
    CheckJsonType(result, validator, "exist_error", JsonType::STRING, label, OPTIONAL);
    CheckJsonType(result, validator, "not_empty", JsonType::BOOLEAN, label, OPTIONAL);
    CheckJsonType(result, validator, "not_empty_error", JsonType::STRING, label, OPTIONAL);
}

// validate one of definitions (["gui"][i]) and store parsed info
void CheckSubDefinition(JsonResult& result, rapidjson::Value& sub_definition,
                        int index,
                        rapidjson::Document::AllocatorType& alloc) noexcept {
    CorrectKey(sub_definition, "window_title", "window_name", alloc);
    CorrectKey(sub_definition, "title", "window_name", alloc);
    CheckJsonType(result, sub_definition, "window_name", JsonType::STRING, "", OPTIONAL);

    if (!sub_definition.HasMember("label")) {
        noex::string default_label = noex::string("GUI ") + index;
        const char* label = GetString(sub_definition, "window_name", default_label.c_str());
        rapidjson::Value n(label, alloc);
        sub_definition.AddMember("label", n, alloc);
    }
    CheckJsonType(result, sub_definition, "label", JsonType::STRING);

    CheckJsonType(result, sub_definition, "button", JsonType::STRING, "", OPTIONAL);

    CheckJsonType(result, sub_definition, "check_exit_code", JsonType::BOOLEAN, "", OPTIONAL);
    CheckJsonType(result, sub_definition, "exit_success", JsonType::INTEGER, "", OPTIONAL);
    CheckJsonType(result, sub_definition, "show_last_line", JsonType::BOOLEAN, "", OPTIONAL);
    CheckJsonType(result, sub_definition,
                    "show_success_dialog", JsonType::BOOLEAN, "", OPTIONAL);
    CheckJsonType(result, sub_definition, "codepage", JsonType::STRING, "", OPTIONAL);
    if (sub_definition.HasMember("codepage")) {
        const char* codepage = sub_definition["codepage"].GetString();
        if (strcmp(codepage, "utf8") != 0 && strcmp(codepage, "utf-8") != 0 &&
                strcmp(codepage, "default") != 0) {
            result.ok = false;
            result.msg = noex::string("Unknown codepage: ") + codepage;
            return;
        }
    }

    CorrectKey(sub_definition, "component", "components", alloc);
    CorrectKey(sub_definition, "component_array", "components", alloc);
    CheckJsonArrayType(result, sub_definition, "components", JsonType::JSON, alloc);

    if (!result.ok) return;

    // check components
    noex::vector<noex::string> comp_ids;
    for (rapidjson::Value& c : sub_definition["components"].GetArray()) {
        // check if type and label exist
        CheckJsonType(result, c, "label", JsonType::STRING, "components");
        if (!result.ok) return;
        const char* label = c["label"].GetString();

        // convert ["type"] from string to enum.
        CheckJsonType(result, c, "type", JsonType::STRING, label);
        if (!result.ok) return;
        const char* type_str = c["type"].GetString();
        int type = ComptypeToInt(type_str);
        if (c.HasMember("type_int"))
            c.RemoveMember("type_int");
        c.AddMember("type_int", type, alloc);
        CorrectKey(c, "item", "items", alloc);
        CorrectKey(c, "item_array", "items", alloc);
        switch (type) {
            case COMP_FILE:
                CheckJsonType(result, c, "extension", JsonType::STRING, label, OPTIONAL);
                /* Falls through. */
            case COMP_FOLDER:
                CheckJsonType(result, c, "button", JsonType::STRING, label, OPTIONAL);
                /* Falls through. */
            case COMP_TEXT:
                CheckJsonType(result, c, "default", JsonType::STRING, label, OPTIONAL);
                break;
            case COMP_COMBO:
            case COMP_RADIO:
                CheckJsonArrayType(result, c, "items", JsonType::JSON, alloc, label);
                if (!result.ok) return;
                for (rapidjson::Value& i : c["items"].GetArray()) {
                    CheckJsonType(result, i, "label", JsonType::STRING, "items");
                    CheckJsonType(result, i, "value", JsonType::STRING, "items", OPTIONAL);
                }
                CheckJsonType(result, c, "default", JsonType::INTEGER, label, OPTIONAL);
                break;
            case COMP_CHECK:
                CheckJsonType(result, c, "value", JsonType::STRING, label, OPTIONAL);
                CheckJsonType(result, c, "default", JsonType::BOOLEAN, label, OPTIONAL);
                break;
            case COMP_CHECK_ARRAY:
                CheckJsonArrayType(result, c, "items", JsonType::JSON, alloc, label);
                if (!result.ok) return;
                for (rapidjson::Value& i : c["items"].GetArray()) {
                    CheckJsonType(result, i, "label", JsonType::STRING, "items");
                    CheckJsonType(result, i, "value", JsonType::STRING, "items", OPTIONAL);
                    CheckJsonType(result, i, "default", JsonType::BOOLEAN, "items", OPTIONAL);
                    CheckJsonType(result, i, "tooltip", JsonType::STRING, "items", OPTIONAL);
                }
                break;
            case COMP_INT:
            case COMP_FLOAT:
                JsonType jtype;
                if (type == COMP_INT) {
                    jtype = JsonType::INTEGER;
                } else {
                    jtype = JsonType::FLOAT;
                    CheckJsonType(result, c, "digits", JsonType::INTEGER, label, OPTIONAL);
                    if (!result.ok) return;
                    if (c.HasMember("digits") && c["digits"].GetInt() < 0) {
                        result.ok = false;
                        result.msg = GetLabel(label, "digits")
                                        + " should be a non-negative integer.";
                    }
                }
                CheckJsonType(result, c, "default", jtype, label, OPTIONAL);
                CheckJsonType(result, c, "min", jtype, label, OPTIONAL);
                CheckJsonType(result, c, "max", jtype, label, OPTIONAL);
                CheckJsonType(result, c, "inc", jtype, label, OPTIONAL);
                CheckJsonType(result, c, "wrap", JsonType::BOOLEAN, label, OPTIONAL);
                break;
            case COMP_UNKNOWN:
                result.ok = false;
                result.msg = noex::string("Unknown component type: ") + type_str;
                break;
        }
        if (!result.ok) return;

        if (c.HasMember("validator")) {
            if (type == COMP_STATIC_TEXT) {
                result.ok = false;
                result.msg = "Static text does not support validator.";
                return;
            }
            CheckJsonType(result, c, "validator", JsonType::JSON, label);
            CheckValidator(result, c["validator"], label);
            if (!result.ok) return;
        }

        CorrectKey(c, "add_quote", "add_quotes", alloc);
        CheckJsonType(result, c, "add_quotes", JsonType::BOOLEAN, label, OPTIONAL);
        CorrectKey(c, "empty_message", "placeholder", alloc);
        CheckJsonType(result, c, "placeholder", JsonType::STRING, label, OPTIONAL);
        CheckJsonType(result, c, "id", JsonType::STRING, label, OPTIONAL);
        CheckJsonType(result, c, "tooltip", JsonType::STRING, label, OPTIONAL);

        CheckJsonType(result, c, "optional", JsonType::BOOLEAN, label, OPTIONAL);
        CheckJsonType(result, c, "prefix", JsonType::STRING, label, OPTIONAL);
        CheckJsonType(result, c, "suffix", JsonType::STRING, label, OPTIONAL);

        bool ignore = false;
        CorrectKey(c, "platform", "platforms", alloc);
        CorrectKey(c, "platform_array", "platforms", alloc);
        CheckJsonArrayType(result, c, "platforms", JsonType::STRING, alloc, label, OPTIONAL);
        if (!result.ok) return;
        if (c.HasMember("platforms")) {
            ignore = true;
            for (rapidjson::Value& v : c["platforms"].GetArray()) {
                if (strcmp(v.GetString(), TUW_CONSTANTS_OS) == 0) {
                    ignore = false;
                    break;
                }
            }
        }

        const char* id = GetString(c, "id", "");
        if (c.HasMember("id")) {
            if (id[0] == '\0') {
                result.ok = false;
                result.msg = GetLabel(label, "id")
                                + " should NOT be an empty string.";
            } else if (id[0] == '_') {
                result.ok = false;
                result.msg = GetLabel(label, "id")
                                + " should NOT start with '_'.";
            }
        }
        if (!result.ok) return;

        if (ignore) {
            comp_ids.emplace_back("");
            c["type_int"].SetInt(COMP_EMPTY);
        } else {
            comp_ids.emplace_back(id);
        }
    }
    CheckIndexDuplication(result, comp_ids);
    if (!result.ok) return;

    // Overwrite ["command"] with ["command_'os'"] if exists.
    const char* command_os_key = "command_" TUW_CONSTANTS_OS;
    if (sub_definition.HasMember(command_os_key)) {
        CheckJsonType(result, sub_definition, command_os_key, JsonType::STRING);
        if (!result.ok) return;
        const char* command_os = sub_definition[command_os_key].GetString();
        if (sub_definition.HasMember("command"))
            sub_definition.RemoveMember("command");
        rapidjson::Value v(command_os, alloc);
        sub_definition.AddMember("command", v, alloc);
    }

    // check sub_definition["command"] and convert it to more useful format.
    CheckJsonType(result, sub_definition, "command", JsonType::STRING);
    if (!result.ok) return;
    CompileCommand(result, sub_definition, comp_ids, alloc);
}

// vX.Y.Z -> 10000*X + 100 * Y + Z
static int VersionStringToInt(JsonResult& result, const char* string) noexcept {
    noex::vector<noex::string> version_strings =
        SplitString(string, '.');
    int digit = 10000;
    int version_int = 0;
    for (const noex::string& str : version_strings) {
        if (str.length() == 0 || str.length() > 2) {
            result.ok = false;
            result.msg = noex::concat_cstr("Can NOT convert '", string, "' to int.");
            return 0;
        }
        if (str.length() == 1) {
            version_int += digit * (str[0] - 48);
        } else {  // length() == 2
            version_int += digit * (str[0] - 48) * 10;
            version_int += digit * (str[1] - 48);
        }
        if (digit == 1)
            break;
        digit /= 100;
    }
    return version_int;
}

void CheckVersion(JsonResult& result, rapidjson::Document& definition) noexcept {
    CorrectKey(definition, "recommended_version", "recommended", definition.GetAllocator());
    if (definition.HasMember("recommended")) {
        CheckJsonType(result, definition, "recommended", JsonType::STRING);
        if (!result.ok) return;
        int recom_int = VersionStringToInt(result, definition["recommended"].GetString());
        if (definition.HasMember("not_recommended")) definition.RemoveMember("not_recommended");
        definition.AddMember("not_recommended",
                                tuw_constants::VERSION_INT != recom_int,
                                definition.GetAllocator());
    }
    CorrectKey(definition, "minimum_required_version",
                "minimum_required", definition.GetAllocator());
    if (definition.HasMember("minimum_required")) {
        CheckJsonType(result, definition, "minimum_required", JsonType::STRING);
        if (!result.ok) return;
        const char* required = definition["minimum_required"].GetString();
        int required_int = VersionStringToInt(result, required);
        if (tuw_constants::VERSION_INT < required_int) {
            result.ok = false;
            result.msg = noex::concat_cstr("Version ", required, " is required.");
        }
    }
}

void CheckDefinition(JsonResult& result, rapidjson::Document& definition) noexcept {
    rapidjson::Document::AllocatorType& alloc = definition.GetAllocator();
    if (!definition.HasMember("gui")) {
        // definition["gui"] = definition
        rapidjson::Value n(rapidjson::kObjectType);
        n.CopyFrom(definition, alloc);
        definition.AddMember("gui", n, alloc);
    }
    CheckJsonArrayType(result, definition, "gui", JsonType::JSON, alloc);
    if (!result.ok) return;
    if (definition["gui"].Size() == 0) {
        result.ok = false;
        result.msg = "The size of [\"gui\"] should NOT be zero.";
    }

    int i = 0;
    for (rapidjson::Value& sub_d : definition["gui"].GetArray()) {
        if (!result.ok) return;
        CheckSubDefinition(result, sub_d, i, alloc);
        i++;
    }
}

void CheckHelpURLs(JsonResult& result, rapidjson::Document& definition) noexcept {
    if (!definition.HasMember("help")) return;
    CheckJsonArrayType(result, definition, "help", JsonType::JSON, definition.GetAllocator());
    if (!result.ok) return;
    for (const rapidjson::Value& h : definition["help"].GetArray()) {
        CheckJsonType(result, h, "type", JsonType::STRING);
        CheckJsonType(result, h, "label", JsonType::STRING);
        if (!result.ok) return;
        const char* type = h["type"].GetString();
        if (strcmp(type, "url") == 0) {
            CheckJsonType(result, h, "url", JsonType::STRING);
        } else if (strcmp(type, "file") == 0) {
            CheckJsonType(result, h, "path", JsonType::STRING);
        } else {
            result.ok = false;
            result.msg = noex::concat_cstr("Unsupported help type: ", type);
            return;
        }
    }
}

}  // namespace json_utils
