#include "json_utils.h"

#include <errno.h>
#include <cstdio>
#include <cassert>

#include "json.h"
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
    errno = 0;
    return _wfopen(wpath.c_str(), wmode.c_str());
}
#endif

noex::string GetFileError(const noex::string& path) noexcept {
    if (errno == ENOENT)
        return "No such file or directory: " + path;
    if (errno == EACCES)
        return "Permission denied: " + path;
    return "Failed to open " + path + " (Errno: " + noex::to_string(errno) + ")";
}

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

noex::string LoadJson(const noex::string& file, tuwjson::Value& json) noexcept {
    FILE* fp = FileOpen(file.c_str(), "rb");
    if (!fp)
        return GetFileError(file);

    char buffer[JSON_SIZE_MAX];
    size_t size = fread(buffer, sizeof(char), JSON_SIZE_MAX - 1, fp);
    fclose(fp);
    if (size > 0)
        buffer[size] = 0;
    else
        buffer[0] = 0;

    tuwjson::Parser parser;
    parser.ParseJson(buffer, &json);
    if (parser.HasError())
        return noex::concat_cstr("Failed to parse JSON: ", parser.GetErrMsg());
    if (!json.IsObject())
        json.SetObject();

    return "";
}

noex::string SaveJson(tuwjson::Value& json, const noex::string& file) noexcept {
    FILE* fp = FileOpen(file.c_str(), "wb");
    if (!fp)
        return GetFileError(file);

    char buffer[JSON_SIZE_MAX];
    tuwjson::Writer writer("    ", 4, true);
    char* end = writer.WriteJson(&json, buffer, JSON_SIZE_MAX);
    if (end)
        fwrite(buffer, sizeof(char), end - buffer, fp);

    fclose(fp);

    if (!end)
        return writer.GetErrMsg();
    return "";
}

const char* GetString(const tuwjson::Value& json, const char* key, const char* def) noexcept {
    if (json.HasMember(key))
        return json[key].GetString();
    return def;
}

bool GetBool(const tuwjson::Value& json, const char* key, bool def) noexcept {
    if (json.HasMember(key))
        return json[key].GetBool();
    return def;
}

int GetInt(const tuwjson::Value& json, const char* key, int def) noexcept {
    if (json.HasMember(key))
        return json[key].GetInt();
    return def;
}

double GetDouble(const tuwjson::Value& json, const char* key, double def) noexcept {
    if (json.HasMember(key))
        return json[key].GetDouble();
    return def;
}

enum class JsonType {
    BOOLEAN,
    INTEGER,
    FLOAT,
    STRING,
    JSON,
    MAX
};

static const bool REQUIRED = false;

static void CheckJsonType(JsonResult& result, const tuwjson::Value& j, const char* key,
        const JsonType& type, const char* name = "", const bool& optional = true) noexcept {
    if (!j.HasMember(key)) {
        if (optional) return;
        result.ok = false;
        result.msg = noex::concat_cstr(name, " requires \"", key) + "\"" + j.GetLineColumnStr();
        return;
    }
    bool valid = false;
    const char* type_name = nullptr;
    const tuwjson::Value& v = j[key];
    switch (type) {
    case JsonType::BOOLEAN:
        valid = v.IsBool();
        type_name = "a boolean";
        break;
    case JsonType::INTEGER:
        valid = v.IsInt();
        type_name = "an int";
        break;
    case JsonType::FLOAT:
        valid = v.IsDouble();
        type_name = "a float";
        break;
    case JsonType::STRING:
        valid = v.IsString();
        type_name = "a string";
        break;
    case JsonType::JSON:
        valid = v.IsObject();
        type_name = "a json object";
        break;
    default:
        assert(false);
        type_name = "";
        break;
    }
    if (!valid) {
        result.ok = false;
        result.msg = noex::concat_cstr("\"", key, "\" should be ") + type_name
            + v.GetLineColumnStr();
    }
}

static bool IsJsonArray(tuwjson::Value& j, const char* key) noexcept {
    tuwjson::Value& val = j[key];
    if (!val.IsArray()) {
        if (!val.IsObject())
            return false;
        val.ConvertToArray();
    }
    for (const tuwjson::Value& el : val) {
        if (!el.IsObject())
            return false;
    }
    return true;
}

static bool IsStringArray(tuwjson::Value& j, const char* key) noexcept {
    tuwjson::Value& val = j[key];
    if (!val.IsArray()) {
        if (!val.IsString())
            return false;
        val.ConvertToArray();
    }
    for (const tuwjson::Value& el : val) {
        if (!el.IsString())
            return false;
    }
    return true;
}

static void CheckJsonArrayType(JsonResult& result, tuwjson::Value& j, const char* key,
        const JsonType& type,
        const char* name = "", const bool& optional = true) noexcept {
    if (!j.HasMember(key)) {
        if (optional) return;
        result.ok = false;
        result.msg = noex::concat_cstr(name, " requires \"", key) + "\"" + j.GetLineColumnStr();
        return;
    }
    bool valid = false;
    const char* type_name = nullptr;
    switch (type) {
    case JsonType::STRING:
        valid = IsStringArray(j, key);
        type_name = "an array of strings";
        break;
    case JsonType::JSON:
        valid = IsJsonArray(j, key);
        type_name = "an array of json objects";
        break;
    default:
        assert(false);
        type_name = "";
        break;
    }
    if (!valid) {
        result.ok = false;
        result.msg = noex::concat_cstr("\"", key, "\" should be ") + type_name
            + j[key].GetLineColumnStr();
    }
}

// get default definition of gui
void GetDefaultDefinition(tuwjson::Value& definition) noexcept {
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
    tuwjson::Parser parser;
    tuwjson::Error ok = parser.ParseJson(def_str, &definition);
    assert(ok == tuwjson::JSON_OK);
    (void) ok;  // GCC says it's unused even if you use it for assertion.
    JsonResult result = JSON_RESULT_OK;
    CheckDefinition(result, definition);
    assert(result.ok);
}

static void CorrectKey(
        tuwjson::Value& j,
        const char* false_key, const char* true_key) noexcept {
    if (!j.HasMember(true_key) && j.HasMember(false_key))
        j.ReplaceKey(false_key, true_key);
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
                result.msg = noex::concat_cstr(
                    "Component IDs should not be duplicated in a gui definition. (",
                    str.c_str(), ")");
                return;
            }
        }
    }
}

// split command by "%" symbol, and calculate which component should be inserted there.
static void CompileCommand(JsonResult& result,
                            tuwjson::Value& sub_definition,
                            const noex::vector<noex::string>& comp_ids) noexcept {
    noex::vector<noex::string> cmd = SplitString(sub_definition["command"].GetString(), '%');
    noex::vector<noex::string> cmd_ids;
    noex::vector<noex::string> splitted_cmd;

    tuwjson::Value splitted_cmd_json;
    splitted_cmd_json.SetArray();

    bool store_ids = false;
    for (const noex::string& token : cmd) {
        if (store_ids) {
            cmd_ids.emplace_back(token);
        } else {
            splitted_cmd.emplace_back(token);
            tuwjson::Value n;
            n.SetString(token);
            splitted_cmd_json.MoveAndPush(n);
        }
        store_ids = !store_ids;
    }
    sub_definition["command_splitted"].MoveFrom(splitted_cmd_json);

    tuwjson::Value& components = sub_definition["components"];;
    tuwjson::Value cmd_int_ids;
    cmd_int_ids.SetArray();
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
                    && (components[non_id_comp]["type_int"].GetInt() == COMP_STATIC_TEXT
                        || !comp_ids[non_id_comp].empty()
                        || components[non_id_comp]["type_int"].GetInt() == COMP_EMPTY)) {
                    non_id_comp++;
                }
                j = non_id_comp;
                non_id_comp++;
            }
        }
        if (j < comp_size) {
            tuwjson::Value n;
            n.SetInt(j);
            cmd_int_ids.MoveAndPush(n);
        }
        if (j >= comp_size)
            cmd_str += "__comp???__";
        else if (j >= 0)
            cmd_str += noex::concat_cstr("__comp", noex::to_string(j).c_str(), "__");
    }
    if (cmd_ids.size() < splitted_cmd.size())
        cmd_str += splitted_cmd.back();

    // Check if the command requires more arguments or ignores some arguments.
    for (int j = 0; j < comp_size; j++) {
        tuwjson::Value& v = components[j];
        int type_int = v["type_int"].GetInt();
        if (type_int == COMP_STATIC_TEXT || type_int == COMP_EMPTY)
            continue;
        bool found = false;
        for (tuwjson::Value& id : cmd_int_ids)
            if (id.GetInt() == j) {
                found = true;
                break;
            }
        if (!found) {
            result.ok = false;

            if (comp_ids[j].empty() || !v.HasMember("id")) {
                result.msg = noex::concat_cstr("[\"components\"][", noex::to_string(j).c_str(), "]")
                    + v.GetLineColumnStr() + " is unused in the command; " + cmd_str;
            } else {
                tuwjson::Value& vid = v["id"];
                result.msg = noex::concat_cstr("component id \"", vid.GetString(), "\"")
                    + vid.GetLineColumnStr() + " is unused in the command; " + cmd_str;
            }
            return;
        }
    }
    if (non_id_comp > comp_size) {
        result.ok = false;
        result.msg =
            "The command requires more components for arguments; " + cmd_str;
        return;
    }
    tuwjson::Value v;
    v.SetString(cmd_str);
    sub_definition["command_str"].MoveFrom(v);
    sub_definition["command_ids"].MoveFrom(cmd_int_ids);
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

void CheckValidator(JsonResult& result, tuwjson::Value& validator) noexcept {
    CheckJsonType(result, validator, "regex", JsonType::STRING);
    CheckJsonType(result, validator, "regex_error", JsonType::STRING);
    CheckJsonType(result, validator, "wildcard", JsonType::STRING);
    CheckJsonType(result, validator, "wildcard_error", JsonType::STRING);
    CheckJsonType(result, validator, "exist", JsonType::BOOLEAN);
    CheckJsonType(result, validator, "exist_error", JsonType::STRING);
    CheckJsonType(result, validator, "not_empty", JsonType::BOOLEAN);
    CheckJsonType(result, validator, "not_empty_error", JsonType::STRING);
}

// validate one of definitions (["gui"][i]) and store parsed info
void CheckSubDefinition(JsonResult& result, tuwjson::Value& sub_definition,
                        int index) noexcept {
    CorrectKey(sub_definition, "window_title", "window_name");
    CorrectKey(sub_definition, "title", "window_name");
    CheckJsonType(result, sub_definition, "window_name", JsonType::STRING);

    if (!sub_definition.HasMember("label")) {
        noex::string default_label = noex::string("GUI ") + index;
        const char* label = GetString(sub_definition, "window_name", default_label.c_str());
        sub_definition["label"].SetString(label);
    }
    CheckJsonType(result, sub_definition, "label", JsonType::STRING, "gui definition", REQUIRED);

    CheckJsonType(result, sub_definition, "button", JsonType::STRING);

    CheckJsonType(result, sub_definition, "check_exit_code", JsonType::BOOLEAN);
    CheckJsonType(result, sub_definition, "exit_success", JsonType::INTEGER);
    CheckJsonType(result, sub_definition, "show_last_line", JsonType::BOOLEAN);
    CheckJsonType(result, sub_definition,
                    "show_success_dialog", JsonType::BOOLEAN);
    CheckJsonType(result, sub_definition, "codepage", JsonType::STRING);
    if (sub_definition.HasMember("codepage")) {
        const char* codepage = sub_definition["codepage"].GetString();
        if (strcmp(codepage, "utf8") != 0 && strcmp(codepage, "utf-8") != 0 &&
                strcmp(codepage, "default") != 0) {
            result.ok = false;
            result.msg = noex::string("Unknown codepage: ") + codepage;
            return;
        }
    }

    CorrectKey(sub_definition, "component", "components");
    CorrectKey(sub_definition, "component_array", "components");
    CheckJsonArrayType(result, sub_definition, "components",
        JsonType::JSON, "gui definition", REQUIRED);

    if (!result.ok) return;

    // check components
    noex::vector<noex::string> comp_ids;
    for (tuwjson::Value& c : sub_definition["components"]) {
        // check if type and label exist
        CheckJsonType(result, c, "label", JsonType::STRING, "component", REQUIRED);
        CheckJsonType(result, c, "type", JsonType::STRING, "component", REQUIRED);
        if (!result.ok) return;

        // convert ["type"] from string to enum.
        const char* type_str = c["type"].GetString();
        int type = ComptypeToInt(type_str);

        // TODO: throw an error for missing ids.
        if (type != COMP_STATIC_TEXT && !c.HasMember("id")) {
            PrintFmt(
                "[CheckDefinition] DeprecationWarning: "
                "\"id\" is missing in [\"components\"][%d]%s."
                " Support for components without \"id\" will be removed in a future version.\n",
                comp_ids.size(), c.GetLineColumnStr().c_str());
        }

        c["type_int"].SetInt(type);
        CorrectKey(c, "item", "items");
        CorrectKey(c, "item_array", "items");
        switch (type) {
            case COMP_FILE:
                CheckJsonType(result, c, "extension", JsonType::STRING);
                /* Falls through. */
            case COMP_FOLDER:
                CheckJsonType(result, c, "button", JsonType::STRING);
                /* Falls through. */
            case COMP_TEXT:
                CheckJsonType(result, c, "default", JsonType::STRING);
                break;
            case COMP_COMBO:
            case COMP_RADIO:
                CheckJsonArrayType(result, c, "items",
                    JsonType::JSON, "radio type component", REQUIRED);
                if (!result.ok) return;
                for (tuwjson::Value& i : c["items"]) {
                    CheckJsonType(result, i, "label", JsonType::STRING, "radio item", REQUIRED);
                    CheckJsonType(result, i, "value", JsonType::STRING);
                }
                CheckJsonType(result, c, "default", JsonType::INTEGER);
                break;
            case COMP_CHECK:
                CheckJsonType(result, c, "value", JsonType::STRING);
                CheckJsonType(result, c, "default", JsonType::BOOLEAN);
                break;
            case COMP_CHECK_ARRAY:
                CheckJsonArrayType(result, c, "items", JsonType::JSON, "check array", REQUIRED);
                if (!result.ok) return;
                for (tuwjson::Value& i : c["items"]) {
                    CheckJsonType(result, i, "label", JsonType::STRING, "check box", REQUIRED);
                    CheckJsonType(result, i, "value", JsonType::STRING);
                    CheckJsonType(result, i, "default", JsonType::BOOLEAN);
                    CheckJsonType(result, i, "tooltip", JsonType::STRING);
                }
                break;
            case COMP_INT:
            case COMP_FLOAT:
                JsonType jtype;
                if (type == COMP_INT) {
                    jtype = JsonType::INTEGER;
                } else {
                    jtype = JsonType::FLOAT;
                    CheckJsonType(result, c, "digits", JsonType::INTEGER);
                    if (!result.ok) return;
                    if (c.HasMember("digits") && c["digits"].GetInt() < 0) {
                        result.ok = false;
                        result.msg = "\"digits\" should be a non-negative integer."
                                    + c["digits"].GetLineColumnStr();
                    }
                }
                CheckJsonType(result, c, "default", jtype);
                CheckJsonType(result, c, "min", jtype);
                CheckJsonType(result, c, "max", jtype);
                CheckJsonType(result, c, "inc", jtype);
                CheckJsonType(result, c, "wrap", JsonType::BOOLEAN);
                break;
            case COMP_UNKNOWN:
                result.ok = false;
                result.msg = noex::string("Unknown component type: ")
                    + type_str + c["type"].GetLineColumnStr();
                break;
        }
        if (!result.ok) return;

        if (c.HasMember("validator")) {
            if (type == COMP_STATIC_TEXT) {
                result.ok = false;
                result.msg = "Static text does not support validator."
                    + c["validator"].GetLineColumnStr();
                return;
            }
            CheckJsonType(result, c, "validator", JsonType::JSON);
            CheckValidator(result, c["validator"]);
            if (!result.ok) return;
        }

        CorrectKey(c, "add_quote", "add_quotes");
        CheckJsonType(result, c, "add_quotes", JsonType::BOOLEAN);
        CorrectKey(c, "empty_message", "placeholder");
        CheckJsonType(result, c, "placeholder", JsonType::STRING);
        CheckJsonType(result, c, "id", JsonType::STRING);
        CheckJsonType(result, c, "tooltip", JsonType::STRING);

        CheckJsonType(result, c, "optional", JsonType::BOOLEAN);
        CheckJsonType(result, c, "prefix", JsonType::STRING);
        CheckJsonType(result, c, "suffix", JsonType::STRING);

        bool ignore = false;
        CorrectKey(c, "platform", "platforms");
        CorrectKey(c, "platform_array", "platforms");
        CheckJsonArrayType(result, c, "platforms", JsonType::STRING);
        if (!result.ok) return;
        if (c.HasMember("platforms")) {
            ignore = true;
            for (tuwjson::Value& v : c["platforms"]) {
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
                result.msg = "\"id\" should NOT be an empty string."
                            + c["id"].GetLineColumnStr();
            } else if (id[0] == '_') {
                result.ok = false;
                result.msg = "\"id\" should NOT start with '_'."
                            + c["id"].GetLineColumnStr();
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
        sub_definition["command"].SetString(command_os);
    }

    // check sub_definition["command"] and convert it to more useful format.
    CheckJsonType(result, sub_definition, "command", JsonType::STRING, "gui definition", REQUIRED);
    if (!result.ok) return;
    CompileCommand(result, sub_definition, comp_ids);
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

void CheckVersion(JsonResult& result, tuwjson::Value& definition) noexcept {
    CorrectKey(definition, "recommended_version", "recommended");
    if (definition.HasMember("recommended")) {
        CheckJsonType(result, definition, "recommended", JsonType::STRING);
        if (!result.ok) return;
        int recom_int = VersionStringToInt(result, definition["recommended"].GetString());
        definition["not_recommended"].SetBool(tuw_constants::VERSION_INT != recom_int);
    }
    CorrectKey(definition, "minimum_required_version", "minimum_required");
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

void CheckDefinition(JsonResult& result, tuwjson::Value& definition) noexcept {
    if (!definition.HasMember("gui")) {
        // definition["gui"] = definition
        definition.ConvertToObject("gui");
    }
    CheckJsonArrayType(result, definition, "gui", JsonType::JSON);
    if (!result.ok) return;
    if (definition["gui"].Size() == 0) {
        result.ok = false;
        result.msg = "The size of [\"gui\"] should NOT be zero."
            + definition["gui"].GetLineColumnStr();;
    }

    int i = 0;
    for (tuwjson::Value& sub_d : definition["gui"]) {
        if (!result.ok) return;
        CheckSubDefinition(result, sub_d, i);
        i++;
    }
}

void CheckHelpURLs(JsonResult& result, tuwjson::Value& definition) noexcept {
    if (!definition.HasMember("help")) return;
    CheckJsonArrayType(result, definition, "help", JsonType::JSON);
    if (!result.ok) return;
    for (const tuwjson::Value& h : definition["help"]) {
        CheckJsonType(result, h, "type", JsonType::STRING, "help document", REQUIRED);
        CheckJsonType(result, h, "label", JsonType::STRING, "help document", REQUIRED);
        if (!result.ok) return;
        const char* type = h["type"].GetString();
        if (strcmp(type, "url") == 0) {
            CheckJsonType(result, h, "url", JsonType::STRING, "URL type document", REQUIRED);
        } else if (strcmp(type, "file") == 0) {
            CheckJsonType(result, h, "path", JsonType::STRING, "file type document", REQUIRED);
        } else {
            result.ok = false;
            result.msg = noex::concat_cstr("Unsupported help type: ", type)
                + h["type"].GetLineColumnStr();;
            return;
        }
    }
}

}  // namespace json_utils
