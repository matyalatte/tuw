#include "json_utils.h"
#include <cstdio>
#include <cassert>
#include <vector>

#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"

#include "tuw_constants.h"

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

    JsonResult LoadJson(const std::string& file, rapidjson::Document& json) {
        FILE* fp = fopen(file.c_str(), "rb");
        if (!fp)
            return { false, "Failed to open " + file };

        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        rapidjson::ParseResult ok = json.ParseStream(is);
        fclose(fp);

        if (!ok) {
            std::string msg("Failed to parse JSON: ");
            msg += std::string(rapidjson::GetParseError_En(ok.Code()))
                   + " (offset: " + std::to_string(ok.Offset()) + ")";
            return { false, msg };
        }
        if (!json.IsObject())
            json.SetObject();

        return JSON_RESULT_OK;
    }

    JsonResult SaveJson(rapidjson::Document& json, const std::string& file) {
        FILE* fp = fopen(file.c_str(), "wb");
        if (!fp)
            return { false, "Failed to open " + file + "." };

        char writeBuffer[65536];
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
        json.Accept(writer);
        fclose(fp);
        return JSON_RESULT_OK;
    }

    std::string JsonToString(rapidjson::Document& json) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
        return std::string(buffer.GetString());
    }

    const char* GetString(const rapidjson::Value& json, const char* key, const char* def) {
        if (json.HasMember(key))
            return json[key].GetString();
        return def;
    }

    bool GetBool(const rapidjson::Value& json, const char* key, bool def) {
        if (json.HasMember(key))
            return json[key].GetBool();
        return def;
    }

    int GetInt(const rapidjson::Value& json, const char* key, int def) {
        if (json.HasMember(key))
            return json[key].GetInt();
        return def;
    }

    double GetDouble(const rapidjson::Value& json, const char* key, double def) {
        if (json.HasMember(key))
            return json[key].GetDouble();
        return def;
    }

    static std::string GetLabel(const std::string& label, const std::string& key) {
        // Todo: std::string to wchar for windows
        std::string msg = "['" + key + "']";
        if (label != "") {
            msg = "['" + label + "']" + msg;
        }
        return msg;
    }

    enum class JsonType {
        BOOLEAN,
        INTEGER,
        FLOAT,
        STRING,
        STRING_ARRAY,
        JSON_ARRAY,
        MAX
    };

    static bool IsJsonArray(const rapidjson::Value& j, const std::string& key) {
        if (!j[key].IsArray()) { return false; }
        for (const rapidjson::Value& el : j[key].GetArray()) {
            if (!el.IsObject())
                return false;
        }
        return true;
    }

    static bool IsStringArray(const rapidjson::Value& j, const std::string& key) {
        if (!j[key].IsArray()) { return false; }
        for (const rapidjson::Value& el : j[key].GetArray()) {
            if (!el.IsString())
                return false;
        }
        return true;
    }

    static const bool CAN_SKIP = true;

    static void CheckJsonType(JsonResult& result, const rapidjson::Value& j, const std::string& key,
        const JsonType& type, const std::string& label = "", const bool& canSkip = false) {
        if (!j.HasMember(key)) {
            if (canSkip) return;
            result.ok = false;
            result.msg = GetLabel(label, key) + " not found.";
            return;
        }
        bool valid = false;
        std::string type_name;
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
        case JsonType::STRING_ARRAY:
            valid = IsStringArray(j, key);
            type_name = "an array of strings";
            break;
        case JsonType::JSON_ARRAY:
            valid = IsJsonArray(j, key);
            type_name = "an array of json objects";
            break;
        default:
            break;
        }
        if (!valid) {
            result.ok = false;
            result.msg = GetLabel(label, key) + " should be " + type_name + ".";
        }
    }

    // get default definition of gui
    void GetDefaultDefinition(rapidjson::Document& definition) {
        static const char* def_str = "{\"gui\":[{"
            "\"label\":\"Default GUI\","
    #ifdef _WIN32
            "\"command\":\"dir\","
            "\"command_str\":\"dir\","
            "\"command_splitted\":[\"dir\"],"
            "\"button\":\"run 'dir'\","
    #else
            "\"command\":\"ls\","
            "\"command_str\":\"ls\","
            "\"command_splitted\":[\"ls\"],"
            "\"button\":\"run 'ls'\","
    #endif
            "\"components\":[],"
            "\"command_ids\":[]"
            "}]}";
        rapidjson::ParseResult ok = definition.Parse(def_str);
        assert(ok);
    }

    static void CorrectKey(rapidjson::Value& j,
                           const std::string& false_key,
                           const std::string& true_key,
                           rapidjson::Document::AllocatorType& alloc) {
        if (!j.HasMember(true_key) && j.HasMember(false_key)) {
            rapidjson::Value n(true_key.c_str(), alloc);
            j.AddMember(n, j[false_key], alloc);
            j.RemoveMember(false_key);
        }
    }

    static std::vector<std::string> SplitString(const std::string& s,
                                                const std::string& delimiter) {
        std::vector<std::string> tokens = std::vector<std::string>(0);
        std::string token;
        size_t delim_length = delimiter.length();
        if (delim_length == 0) {
            tokens.push_back(s);
            return tokens;
        }

        size_t offset = std::string::size_type(0);
        while (s.length() > offset) {
            size_t pos = s.find(delimiter, offset);
            if (pos == std::string::npos) {
                tokens.push_back(s.substr(offset));
                break;
            }
            tokens.push_back(s.substr(offset, pos - offset));
            offset = pos + delim_length;
        }
        return tokens;
    }

    static void CheckIndexDuplication(JsonResult& result,
                                      const std::vector<std::string>& component_ids) {
        size_t size = component_ids.size();
        if (size == 0)
            return;
        for (size_t i = 0; i < size - 1; i++) {
            std::string str = component_ids[i];
            if (str == "") { continue; }
            for (size_t j = i + 1; j < size; j++) {
                if (str == component_ids[j]) {
                    result.ok = false;
                    result.msg = GetLabel("components", "id")
                                 + " should not be duplicated in a gui definition. ("
                                 + str + ")";
                    return;
                }
            }
        }
    }

    // split command by "%" symbol, and calculate which component should be inserted there.
    static void CompileCommand(JsonResult& result,
                               rapidjson::Value& sub_definition,
                               const std::vector<std::string>& comp_ids,
                               rapidjson::Document::AllocatorType& alloc) {
        std::vector<std::string> cmd = SplitString(sub_definition["command"].GetString(),
                                                   { '%' });
        std::vector<std::string> cmd_ids = std::vector<std::string>(0);
        std::vector<std::string> splitted_cmd = std::vector<std::string>(0);
        if (sub_definition.HasMember("command_splitted"))
            sub_definition.RemoveMember("command_splitted");
        rapidjson::Value splitted_cmd_json(rapidjson::kArrayType);

        bool store_ids = false;
        for (const std::string& token : cmd) {
            if (store_ids) {
                cmd_ids.push_back(token);
            } else {
                splitted_cmd.push_back(token);
                rapidjson::Value n(token.c_str(), alloc);
                splitted_cmd_json.PushBack(n, alloc);
            }
            store_ids = !store_ids;
        }
        sub_definition.AddMember("command_splitted", splitted_cmd_json, alloc);

        rapidjson::Value& components = sub_definition["components"];;
        rapidjson::Value cmd_int_ids(rapidjson::kArrayType);
        std::string cmd_str = "";
        int comp_size = static_cast<int>(comp_ids.size());
        int non_id_comp = 0;
        for (int i = 0; i < static_cast<int>(cmd_ids.size()); i++) {
            cmd_str += splitted_cmd[i];
            std::string id = cmd_ids[i];
            int j;
            if (id == CMD_TOKEN_PERCENT) {
                j = CMD_ID_PERCENT;
                cmd_str += "%";
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
                            || comp_ids[non_id_comp] != ""
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
                cmd_str += "__comp" + std::to_string(j) + "__";
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
                if (id.GetInt() == j) { found = true; break; }
            if (!found) {
                result.ok = false;
                result.msg = "[\"components\"][" + std::to_string(j)
                             + "] is unused in the command; " + cmd_str;
                if (comp_ids[j] != "")
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
        sub_definition.AddMember("command_str", cmd_str, alloc);
        if (sub_definition.HasMember("command_ids"))
            sub_definition.RemoveMember("command_ids");
        sub_definition.AddMember("command_ids", cmd_int_ids, alloc);
    }

    // don't use map. it will make exe larger.
    int ComptypeToInt(const char* comptype) {
        if (strcmp(comptype, "static_text") == 0)
            return COMP_STATIC_TEXT;
        else if (strcmp(comptype, "file") == 0)
            return COMP_FILE;
        else if (strcmp(comptype, "folder") == 0)
            return COMP_FOLDER;
        else if (strcmp(comptype, "dir") == 0)
            return COMP_FOLDER;
        else if (strcmp(comptype, "choice") == 0)
            return COMP_COMBO;
        else if (strcmp(comptype, "combo") == 0)
            return COMP_COMBO;
        else if (strcmp(comptype, "radio") == 0)
            return COMP_RADIO;
        else if (strcmp(comptype, "check") == 0)
            return COMP_CHECK;
        else if (strcmp(comptype, "check_array") == 0)
            return COMP_CHECK_ARRAY;
        else if (strcmp(comptype, "checks") == 0)
            return COMP_CHECK_ARRAY;
        else if (strcmp(comptype, "text") == 0)
            return COMP_TEXT;
        else if (strcmp(comptype, "text_box") == 0)
            return COMP_TEXT;
        else if (strcmp(comptype, "int") == 0)
            return COMP_INT;
        else if (strcmp(comptype, "integer") == 0)
            return COMP_INT;
        else if (strcmp(comptype, "float") == 0)
            return COMP_FLOAT;
        return COMP_UNKNOWN;
    }

    // validate one of definitions (["gui"][i]) and store parsed info
    void CheckSubDefinition(JsonResult& result, rapidjson::Value& sub_definition,
                            rapidjson::Document::AllocatorType& alloc) {
        // check is_string
        CheckJsonType(result, sub_definition, "label", JsonType::STRING);
        CheckJsonType(result, sub_definition, "button", JsonType::STRING, "", CAN_SKIP);
        CorrectKey(sub_definition, "window_title", "window_name", alloc);
        CorrectKey(sub_definition, "title", "window_name", alloc);
        CheckJsonType(result, sub_definition, "window_name", JsonType::STRING, "", CAN_SKIP);

        CheckJsonType(result, sub_definition, "check_exit_code", JsonType::BOOLEAN, "", CAN_SKIP);
        CheckJsonType(result, sub_definition, "exit_success", JsonType::INTEGER, "", CAN_SKIP);
        CheckJsonType(result, sub_definition, "show_last_line", JsonType::BOOLEAN, "", CAN_SKIP);
        CheckJsonType(result, sub_definition,
                      "show_success_dialog", JsonType::BOOLEAN, "", CAN_SKIP);

        CorrectKey(sub_definition, "component", "components", alloc);
        CorrectKey(sub_definition, "component_array", "components", alloc);
        CheckJsonType(result, sub_definition, "components", JsonType::JSON_ARRAY);

        if (!result.ok) return;

        // check components
        std::vector<std::string> comp_ids;
        for (rapidjson::Value& c : sub_definition["components"].GetArray()) {
            // check if type and label exist
            CheckJsonType(result, c, "label", JsonType::STRING, "components");
            if (!result.ok) return;
            std::string label = c["label"].GetString();

            // convert ["type"] from string to enum.
            CheckJsonType(result, c, "type", JsonType::STRING, label);
            if (!result.ok) return;
            std::string type_str = c["type"].GetString();
            int type = ComptypeToInt(type_str.c_str());
            if (c.HasMember("type_int"))
                c.RemoveMember("type_int");
            c.AddMember("type_int", type, alloc);
            CorrectKey(c, "item", "items", alloc);
            CorrectKey(c, "item_array", "items", alloc);
            switch (type) {
                case COMP_FILE:
                    CheckJsonType(result, c, "extension", JsonType::STRING, label, CAN_SKIP);
                    /* Falls through. */
                case COMP_FOLDER:
                    CheckJsonType(result, c, "button", JsonType::STRING, label, CAN_SKIP);
                    /* Falls through. */
                case COMP_TEXT:
                    CheckJsonType(result, c, "default", JsonType::STRING, label, CAN_SKIP);
                    break;
                case COMP_COMBO:
                case COMP_RADIO:
                    CheckJsonType(result, c, "items", JsonType::JSON_ARRAY, label);
                    if (!result.ok) return;
                    for (rapidjson::Value& i : c["items"].GetArray()) {
                        CheckJsonType(result, i, "label", JsonType::STRING, "items");
                        CheckJsonType(result, i, "value", JsonType::STRING, "items", CAN_SKIP);
                    }
                    CheckJsonType(result, c, "default", JsonType::INTEGER, label, CAN_SKIP);
                    break;
                case COMP_CHECK:
                    CheckJsonType(result, c, "value", JsonType::STRING, label, CAN_SKIP);
                    CheckJsonType(result, c, "default", JsonType::BOOLEAN, label, CAN_SKIP);
                    break;
                case COMP_CHECK_ARRAY:
                    CheckJsonType(result, c, "items", JsonType::JSON_ARRAY, label);
                    if (!result.ok) return;
                    for (rapidjson::Value& i : c["items"].GetArray()) {
                        CheckJsonType(result, i, "label", JsonType::STRING, "items");
                        CheckJsonType(result, i, "value", JsonType::STRING, "items", CAN_SKIP);
                        CheckJsonType(result, i, "default", JsonType::BOOLEAN, "items", CAN_SKIP);
                        CheckJsonType(result, i, "tooltip", JsonType::STRING, "items", CAN_SKIP);
                    }
                    break;
                case COMP_INT:
                case COMP_FLOAT:
                    JsonType jtype;
                    if (type == COMP_INT) {
                        jtype = JsonType::INTEGER;
                    } else {
                        jtype = JsonType::FLOAT;
                        CheckJsonType(result, c, "digits", JsonType::INTEGER, label, CAN_SKIP);
                        if (!result.ok) return;
                        if (c.HasMember("digits") && c["digits"].GetInt() < 0) {
                            result.ok = false;
                            result.msg = GetLabel(label, "digits")
                                         + " should be a non-negative integer.";
                        }
                    }
                    CheckJsonType(result, c, "default", jtype, label, CAN_SKIP);
                    CheckJsonType(result, c, "min", jtype, label, CAN_SKIP);
                    CheckJsonType(result, c, "max", jtype, label, CAN_SKIP);
                    CheckJsonType(result, c, "inc", jtype, label, CAN_SKIP);
                    CheckJsonType(result, c, "wrap", JsonType::BOOLEAN, label, CAN_SKIP);
                    break;
                case COMP_UNKNOWN:
                    result.ok = false;
                    result.msg = "Unknown component type: " + type_str;
                    break;
            }
            if (!result.ok) return;

            CorrectKey(c, "add_quote", "add_quotes", alloc);
            CheckJsonType(result, c, "add_quotes", JsonType::BOOLEAN, label, CAN_SKIP);
            CorrectKey(c, "empty_message", "placeholder", alloc);
            CheckJsonType(result, c, "placeholder", JsonType::STRING, label, CAN_SKIP);
            CheckJsonType(result, c, "id", JsonType::STRING, label, CAN_SKIP);
            CheckJsonType(result, c, "tooltip", JsonType::STRING, label, CAN_SKIP);

            bool ignore = false;
            CorrectKey(c, "platform", "platforms", alloc);
            CorrectKey(c, "platform_array", "platforms", alloc);
            CheckJsonType(result, c, "platforms", JsonType::STRING_ARRAY, label, CAN_SKIP);
            if (!result.ok) return;
            if (c.HasMember("platforms")) {
                ignore = true;
                for (rapidjson::Value& v : c["platforms"].GetArray()) {
                    if (v.GetString() == std::string(tuw_constants::OS)) {
                        ignore = false;
                        break;
                    }
                }
            }

            std::string id = "";
            if (c.HasMember("id")) {
                id = c["id"].GetString();
                if (id == "") {
                    result.ok = false;
                    result.msg = GetLabel(label, "id")
                                 + " should NOT be an empty string.";
                } else if (id[0] == "_"[0]) {
                    result.ok = false;
                    result.msg = GetLabel(label, "id")
                                 + " should NOT start with '_'.";
                }
            }
            if (!result.ok) return;

            if (ignore) {
                comp_ids.push_back("");
                c["type_int"].SetInt(COMP_EMPTY);
            } else {
                comp_ids.push_back(id);
            }
        }
        CheckIndexDuplication(result, comp_ids);
        if (!result.ok) return;

        // Overwrite ["command"] with ["command_'os'"] if exists.
        std::string command_os_key = std::string("command_") + tuw_constants::OS;
        if (sub_definition.HasMember(command_os_key)) {
            CheckJsonType(result, sub_definition, command_os_key, JsonType::STRING);
            if (!result.ok) return;
            std::string command_os = sub_definition[command_os_key].GetString();
            if (sub_definition.HasMember("command"))
                sub_definition.RemoveMember("command");
            sub_definition.AddMember("command", command_os, alloc);
        }

        // check sub_definition["command"] and convert it to more useful format.
        CheckJsonType(result, sub_definition, "command", JsonType::STRING);
        if (!result.ok) return;
        CompileCommand(result, sub_definition, comp_ids, alloc);
    }

    // vX.Y.Z -> 10000*X + 100 * Y + Z
    static int VersionStringToInt(JsonResult& result, const std::string& string) {
        std::vector<std::string> version_strings =
            SplitString(string, { '.' });
        int digit = 10000;
        int version_int = 0;
        for (const std::string& str : version_strings) {
            if (str.length() == 0 || str.length() > 2) {
                result.ok = false;
                result.msg = "Can NOT convert '" + string + "' to int.";
                return 0;
            }
            if (str.length() == 1) {
                version_int += digit * (str[0] - 48);
            } else {  // length() == 2
                version_int += digit * (str[0] - 48) * 10;
                version_int += digit * (str[1] - 48);
            }
            if (digit == 1) { break; }
            digit /= 100;
        }
        return version_int;
    }

    void CheckVersion(JsonResult& result, rapidjson::Document& definition) {
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
            std::string required = definition["minimum_required"].GetString();
            if (!result.ok) return;
            int required_int = VersionStringToInt(result, required);
            if (tuw_constants::VERSION_INT < required_int) {
                result.ok = false;
                result.msg = "Version " + required + " is required.";
            }
        }
    }

    void CheckDefinition(JsonResult& result, rapidjson::Document& definition) {
        CheckJsonType(result, definition, "gui", JsonType::JSON_ARRAY);
        if (!result.ok) return;
        if (definition["gui"].Size() == 0) {
            result.ok = false;
            result.msg = "The size of [\"gui\"] should NOT be zero.";
        }

        for (rapidjson::Value& sub_d : definition["gui"].GetArray()) {
            if (!result.ok) return;
            CheckSubDefinition(result, sub_d, definition.GetAllocator());
        }
    }

    void CheckHelpURLs(JsonResult& result, const rapidjson::Document& definition) {
        if (!definition.HasMember("help")) return;
        CheckJsonType(result, definition, "help", JsonType::JSON_ARRAY);
        if (!result.ok) return;
        for (const rapidjson::Value& h : definition["help"].GetArray()) {
            CheckJsonType(result, h, "type", JsonType::STRING);
            CheckJsonType(result, h, "label", JsonType::STRING);
            if (!result.ok) return;
            std::string type = h["type"].GetString();
            if (type == "url") {
                CheckJsonType(result, h, "url", JsonType::STRING);
            } else if (type == "file") {
                CheckJsonType(result, h, "path", JsonType::STRING);
            } else {
                result.ok = false;
                result.msg = "Unsupported help type: " + type;
                return;
            }
        }
    }
}  // namespace json_utils
