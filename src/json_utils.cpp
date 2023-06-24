#include "json_utils.h"

namespace json_utils {
    void LoadJson(const std::string& file, rapidjson::Document& json) {
        std::ifstream ifs(file);
        if (!ifs) {
            std::string msg = "Failed to open " + file;
            throw std::runtime_error(msg);
        }
        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::ParseResult ok = json.ParseStream(isw);
        if (!ok) {
            std::string msg("Failed to parse JSON: ");
            msg += std::string(rapidjson::GetParseError_En(ok.Code()))
                   + " (offset: " + std::to_string(ok.Offset()) + ")";
            throw std::runtime_error(msg);
        }
        if (!json.IsObject())
            json.SetObject();
    }

    bool SaveJson(rapidjson::Document& json, const std::string& file) {
        std::ofstream ofs(file);
        if (!ofs)
            return false;
        rapidjson::OStreamWrapper osw(ofs);
        rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
        json.Accept(writer);
        return true;
    }

    std::string JsonToString(rapidjson::Document& json) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
        return std::string(buffer.GetString());
    }

    std::string GetString(const rapidjson::Value& json, const char* key, const char* def) {
        if (json.HasMember(key))
            return json[key].GetString();
        return std::string(def);
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

    static void CheckJsonType(const rapidjson::Value& j, const std::string& key,
        const JsonType& type, const std::string& label = "", const bool& canSkip = false) {
        if (!j.HasMember(key)) {
            if (canSkip) return;
            std::string msg = GetLabel(label, key) + " not found.";
            throw std::runtime_error(msg);
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
            throw std::runtime_error(GetLabel(label, key) + " should be " + type_name + ".");
        }
    }

    // get default definition of gui
    void GetDefaultDefinition(rapidjson::Document& definition) {
        std::string def_str = "{\"gui\":[{"
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

    static void CheckIndexDuplication(const std::vector<std::string>& component_ids) {
        int size = component_ids.size();
        for (int i = 0; i < size - 1; i++) {
            std::string str = component_ids[i];
            if (str == "") { continue; }
            for (int j = i + 1; j < size; j++) {
                if (str == component_ids[j]) {
                    std::string msg = GetLabel("components", "id")
                                      + " should not be duplicated in a gui definition. ("
                                      + str + ")";
                    throw std::runtime_error(msg);
                }
            }
        }
    }

    // split command by "%" symbol, and calculate which component should be inserted there.
    static void CompileCommand(rapidjson::Value& sub_definition,
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
        int comp_size = comp_ids.size();
        int non_id_comp = 0;
        for (int i = 0; i < cmd_ids.size(); i++) {
            cmd_str += splitted_cmd[i];
            std::string id = cmd_ids[i];
            int j;
            if (id == CMD_TOKEN_PERCENT) {
                j = CMD_ID_PERCENT;
                cmd_str += "%";
            } else if (id == CMD_TOKEN_CURRENT_DIR) {
                j = CMD_ID_CURRENT_DIR;
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
                std::string msg = "[\"commponents\"][" + std::to_string(j)
                                  + "] is unused in the command; " + cmd_str;
                if (comp_ids[j] != "")
                    msg = "The ID of " + msg;
                throw std::runtime_error(msg);
            }
        }
        if (non_id_comp > comp_size) {
            throw std::runtime_error(
                "The command requires more components for arguments; " + cmd_str);
        }
        if (sub_definition.HasMember("command_str"))
            sub_definition.RemoveMember("command_str");
        sub_definition.AddMember("command_str", cmd_str, alloc);
        if (sub_definition.HasMember("command_ids"))
            sub_definition.RemoveMember("command_ids");
        sub_definition.AddMember("command_ids", cmd_int_ids, alloc);
    }

    static const matya::map_as_vec<int> COMPTYPE_TO_INT = {
        {"static_text", COMP_STATIC_TEXT},
        {"file", COMP_FILE},
        {"folder", COMP_FOLDER},
        {"dir", COMP_FOLDER},
        {"choice", COMP_CHOICE},
        {"combo", COMP_CHOICE},
        {"check", COMP_CHECK},
        {"check_array", COMP_CHECK_ARRAY},
        {"checks", COMP_CHECK_ARRAY},
        {"text", COMP_TEXT},
        {"text_box", COMP_TEXT},
        {"int", COMP_INT},
        {"integer", COMP_INT},
        {"float", COMP_FLOAT},
    };

    // validate one of definitions (["gui"][i]) and store parsed info
    void CheckSubDefinition(rapidjson::Value& sub_definition,
                            rapidjson::Document::AllocatorType& alloc) {
        // check is_string
        CheckJsonType(sub_definition, "label", JsonType::STRING);
        CheckJsonType(sub_definition, "button", JsonType::STRING, "", CAN_SKIP);
        CorrectKey(sub_definition, "window_title", "window_name", alloc);
        CorrectKey(sub_definition, "title", "window_name", alloc);
        CheckJsonType(sub_definition, "window_name", JsonType::STRING, "", CAN_SKIP);

        CheckJsonType(sub_definition, "check_exit_code", JsonType::BOOLEAN, "", CAN_SKIP);
        CheckJsonType(sub_definition, "exit_success", JsonType::INTEGER, "", CAN_SKIP);
        CheckJsonType(sub_definition, "show_last_line", JsonType::BOOLEAN, "", CAN_SKIP);

        CorrectKey(sub_definition, "component", "components", alloc);
        CorrectKey(sub_definition, "component_array", "components", alloc);
        CheckJsonType(sub_definition, "components", JsonType::JSON_ARRAY);

        // check components
        std::vector<std::string> comp_ids;
        for (rapidjson::Value& c : sub_definition["components"].GetArray()) {
            // check if type and label exist
            CheckJsonType(c, "label", JsonType::STRING, "components");
            std::string label = c["label"].GetString();

            // convert ["type"] from string to enum.
            CheckJsonType(c, "type", JsonType::STRING, label);
            std::string type_str = c["type"].GetString();
            int type = COMPTYPE_TO_INT.get(type_str, COMP_UNKNOWN);
            if (c.HasMember("type_int"))
                c.RemoveMember("type_int");
            c.AddMember("type_int", type, alloc);
            CorrectKey(c, "item", "items", alloc);
            CorrectKey(c, "item_array", "items", alloc);
            switch (type) {
                case COMP_FILE:
                    CheckJsonType(c, "extention", JsonType::STRING, label, CAN_SKIP);
                case COMP_FOLDER:
                case COMP_TEXT:
                    CheckJsonType(c, "default", JsonType::STRING, label, CAN_SKIP);
                    break;
                case COMP_CHOICE:
                    CheckJsonType(c, "items", JsonType::JSON_ARRAY, label);
                    for (rapidjson::Value& i : c["items"].GetArray()) {
                        CheckJsonType(i, "label", JsonType::STRING, "items");
                        CheckJsonType(i, "value", JsonType::STRING, "items", CAN_SKIP);
                    }
                    break;
                case COMP_CHECK:
                    CheckJsonType(c, "value", JsonType::STRING, label, CAN_SKIP);
                    CheckJsonType(c, "default", JsonType::BOOLEAN, label, CAN_SKIP);
                    break;
                case COMP_CHECK_ARRAY:
                    CheckJsonType(c, "items", JsonType::JSON_ARRAY, label);
                    for (rapidjson::Value& i : c["items"].GetArray()) {
                        CheckJsonType(i, "label", JsonType::STRING, "items");
                        CheckJsonType(i, "value", JsonType::STRING, "items", CAN_SKIP);
                        CheckJsonType(i, "default", JsonType::BOOLEAN, "items", CAN_SKIP);
                        CheckJsonType(i, "tooltip", JsonType::STRING, "items", CAN_SKIP);
                    }
                    break;
                case COMP_INT:
                case COMP_FLOAT:
                    JsonType jtype;
                    if (type == COMP_INT) {
                        jtype = JsonType::INTEGER;
                    } else {
                        jtype = JsonType::FLOAT;
                        CheckJsonType(c, "digits", JsonType::INTEGER, label, CAN_SKIP);
                        if (c.HasMember("digits") && c["digits"].GetInt() < 0) {
                            throw std::runtime_error(GetLabel(label, "digits")
                                                     + " should be a non-negative integer.");
                        }
                    }
                    CheckJsonType(c, "default", jtype, label, CAN_SKIP);
                    CheckJsonType(c, "min", jtype, label, CAN_SKIP);
                    CheckJsonType(c, "max", jtype, label, CAN_SKIP);
                    CheckJsonType(c, "inc", jtype, label, CAN_SKIP);
                    CheckJsonType(c, "wrap", JsonType::BOOLEAN, label, CAN_SKIP);
                    break;
                case COMP_UNKNOWN:
                    throw std::runtime_error("Unknown component type: " + type_str);
                    break;
            }

            CorrectKey(c, "add_quote", "add_quotes", alloc);
            CheckJsonType(c, "add_quotes", JsonType::BOOLEAN, label, CAN_SKIP);
            CorrectKey(c, "placeholder", "empty_message", alloc);
            CheckJsonType(c, "empty_message", JsonType::STRING, label, CAN_SKIP);
            CheckJsonType(c, "id", JsonType::STRING, label, CAN_SKIP);
            CheckJsonType(c, "tooltip", JsonType::STRING, label, CAN_SKIP);

            bool ignore = false;
            CorrectKey(c, "platform", "platforms", alloc);
            CorrectKey(c, "platform_array", "platforms", alloc);
            CheckJsonType(c, "platforms", JsonType::STRING_ARRAY, label, CAN_SKIP);
            if (c.HasMember("platforms")) {
                ignore = true;
                for (rapidjson::Value& v : c["platforms"].GetArray()) {
                    if (v.GetString() == std::string(scr_constants::OS)) {
                        ignore = false;
                        break;
                    }
                }
            }

            std::string id = "";
            if (c.HasMember("id")) {
                id = c["id"].GetString();
                if (id == "") {
                    throw std::runtime_error(GetLabel(label, "id")
                                             + " should NOT be an empty string.");
                } else if (id[0] == "_"[0]) {
                    throw std::runtime_error(GetLabel(label, "id")
                                             + " should NOT start with '_'.");
                }
            }

            if (ignore) {
                comp_ids.push_back("");
                c["type_int"].SetInt(COMP_EMPTY);
            } else {
                comp_ids.push_back(id);
            }
        }
        CheckIndexDuplication(comp_ids);

        // Overwrite ["command"] with ["command_'os'"] if exists.
        std::string command_os_key = std::string("command_") + scr_constants::OS;
        if (sub_definition.HasMember(command_os_key)) {
            CheckJsonType(sub_definition, command_os_key, JsonType::STRING);
            std::string command_os = sub_definition[command_os_key].GetString();
            if (sub_definition.HasMember("command"))
                sub_definition.RemoveMember("command");
            sub_definition.AddMember("command", command_os, alloc);
        }

        // check sub_definition["command"] and convert it to more useful format.
        CheckJsonType(sub_definition, "command", JsonType::STRING);
        CompileCommand(sub_definition, comp_ids, alloc);
    }

    // vX.Y.Z -> 10000*X + 100 * Y + Z
    static int VersionStringToInt(const std::string& string) {
        try {
            std::vector<std::string> version_strings = SplitString(string, {"."});
            int digit = 10000;
            int version_int = 0;
            for (std::string str : version_strings) {
                version_int += digit * std::stoi(str);
                if (digit == 1) { break; }
                digit /= 100;
            }
            return version_int;
        }
        catch(std::exception& e) {
            throw std::runtime_error("Can NOT convert '" + string + "' to int.");
        }
        return 0;
    }

    void CheckVersion(rapidjson::Document& definition) {
        CorrectKey(definition, "recommended_version", "recommended", definition.GetAllocator());
        if (definition.HasMember("recommended")) {
            CheckJsonType(definition, "recommended", JsonType::STRING);
            int recom_int = VersionStringToInt(definition["recommended"].GetString());
            CheckJsonType(definition, "not_recommended", JsonType::BOOLEAN, "", CAN_SKIP);
            if (definition.HasMember("not_recommended")) definition.RemoveMember("not_recommended");
            definition.AddMember("not_recommended",
                                 scr_constants::VERSION_INT != recom_int,
                                 definition.GetAllocator());
        }
        CorrectKey(definition, "minimum_required_version",
                   "minimum_required", definition.GetAllocator());
        if (definition.HasMember("minimum_required")) {
            CheckJsonType(definition, "minimum_required", JsonType::STRING);
            std::string required = definition["minimum_required"].GetString();
            int required_int = VersionStringToInt(required);
            if (scr_constants::VERSION_INT < required_int) {
                std::string msg = "Version " + required + " is required.";
                throw std::runtime_error(msg);
            }
        }
    }

    void CheckDefinition(rapidjson::Document& definition) {
        CheckJsonType(definition, "gui", JsonType::JSON_ARRAY);
        if (definition["gui"].Size() == 0)
            throw std::runtime_error("The size of [\"gui\"] should NOT be zero.");
        for (rapidjson::Value& sub_d : definition["gui"].GetArray()) {
            CheckSubDefinition(sub_d, definition.GetAllocator());
        }
    }

    void CheckHelpURLs(const rapidjson::Document& definition) {
        CheckJsonType(definition, "help", JsonType::JSON_ARRAY);
        for (const rapidjson::Value& h : definition["help"].GetArray()) {
            CheckJsonType(h, "type", JsonType::STRING);
            CheckJsonType(h, "label", JsonType::STRING);
            std::string type = h["type"].GetString();
            if (type == "url") {
                CheckJsonType(h, "url", JsonType::STRING);
            } else if (type == "file") {
                CheckJsonType(h, "path", JsonType::STRING);
            } else {
                throw std::runtime_error("Unsupported help type: " + type);
            }
        }
    }
}  // namespace json_utils
