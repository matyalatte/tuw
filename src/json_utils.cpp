#include "json_utils.h"

namespace json_utils {
    nlohmann::json LoadJson(const std::string& file) {
        std::ifstream istream(file);
        nlohmann::json json;
        if (!istream) {
            std::string msg = "Failed to open " + file;
            throw std::runtime_error(msg);
        }
        istream >> json;
        istream.close();
        return json;
    }

    bool SaveJson(nlohmann::json& json, const std::string& file) {
        std::ofstream ostream(file);

        if (!ostream) {
            return false;
        }
        ostream << std::setw(4) << json << std::endl;
        ostream.close();
        return true;
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
        BOOL_ARRAY,
        INT_ARRAY,
        STR_ARRAY,
        JSON_ARRAY,
        MAX
    };

    static bool IsArray(const nlohmann::json& j, const std::string& key, const JsonType& type) {
        if (!j[key].is_array()) { return false; }
        std::function<bool(const nlohmann::json&)> lmd;
        switch (type) {
        case JsonType::BOOL_ARRAY:
            lmd = [](const nlohmann::json& el){ return el.is_boolean(); };
            break;
        case JsonType::INT_ARRAY:
            lmd = [](const nlohmann::json& el){ return el.is_number_integer(); };
            break;
        case JsonType::STR_ARRAY:
            lmd = [](const nlohmann::json& el){ return el.is_string(); };
            break;
        case JsonType::JSON_ARRAY:
            lmd = [](const nlohmann::json& el){ return el.is_object(); };
            break;
        default:
            return false;
        }
        return std::all_of(j[key].begin(), j[key].end(), lmd);
    }

    static const bool CAN_SKIP = true;

    static void CheckJsonType(const nlohmann::json& j, const std::string& key,
        const JsonType& type, const std::string& label = "", const bool& canSkip = false) {
        if (canSkip && !j.contains(key)) {
            return;
        }
        if (!j.contains(key)) {
            std::string msg = GetLabel(label, key) + " not found.";
            throw std::runtime_error(msg);
        }
        bool valid = false;
        std::string type_name;
        switch (type) {
        case JsonType::BOOLEAN:
            valid = j[key].is_boolean();
            type_name = "a boolean";
            break;
        case JsonType::INTEGER:
            valid = j[key].is_number_integer();
            type_name = "an int";
            break;
        case JsonType::FLOAT:
            valid = j[key].is_number_float() || j[key].is_number_integer();
            type_name = "a float";
            break;
        case JsonType::STRING:
            valid = j[key].is_string();
            type_name = "a string";
            break;
        case JsonType::BOOL_ARRAY:
            valid = IsArray(j, key, type);
            type_name = "an array of booleans";
            break;
        case JsonType::INT_ARRAY:
            valid = IsArray(j, key, type);
            type_name = "an array of integers";
            break;
        case JsonType::STR_ARRAY:
            valid = IsArray(j, key, type);
            type_name = "an array of strings";
            break;
        case JsonType::JSON_ARRAY:
            valid = IsArray(j, key, type);
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
    nlohmann::json GetDefaultDefinition() {
        nlohmann::json def = {
                {"label", "Default GUI"},
    #ifdef _WIN32
                {"command", {"dir"} },
                {"button", "run 'dir'"},
    #else
                {"command", {"ls"} },
                {"button", "run 'ls'"},
    #endif
                {"command_ids", nlohmann::json::array({})},
                {"components", nlohmann::json::array({})},
                {"component_ids", nlohmann::json::array({})}
        };
        return def;
    }

    static void CorrectKey(nlohmann::json& j,
                           const std::string& false_key,
                           const std::string& true_key) {
        if (!j.contains(true_key) && j.contains(false_key)) {
            j[true_key] = j[false_key];
            j.erase(false_key);
        }
    }

    static void KeyToSingular(nlohmann::json& c, const std::string& singular) {
        std::vector<std::string> extends = {"s", "_array"};
        for (std::string ext : extends) {
            CorrectKey(c, singular + ext, singular);
        }
    }

    static void CheckArraySize(nlohmann::json& c, const std::string& key) {
        if (c.contains(key) && (c[key].size() != c["item"].size())) {
            std::string label = c["label"].get<std::string>();
            std::string msg = GetLabel(label, key) + " and " +
                              GetLabel(label, "item") + " should have the same size.";
            throw std::runtime_error(msg);
        }
    }

    static void CheckItemsValues(nlohmann::json& c) {
        std::string label = c["label"].get<std::string>();
        KeyToSingular(c, "item");
        CheckJsonType(c, "item", JsonType::STR_ARRAY, label);
        KeyToSingular(c, "value");
        CheckJsonType(c, "value", JsonType::STR_ARRAY, label, CAN_SKIP);
        CheckArraySize(c, "value");
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

    static void CheckCommand(nlohmann::json& sub_definition) {
        CheckJsonType(sub_definition, "command", JsonType::STRING);
        sub_definition["command"] = SplitString(sub_definition["command"].get<std::string>(),
                                                { '%' });

        std::vector<std::string> cmd = sub_definition["command"].get<std::vector<std::string>>();
        std::vector<std::string> cmd_ids = std::vector<std::string>(0);
        std::vector<std::string> splitted_cmd = std::vector<std::string>(0);
        bool store_ids = false;
        for (const std::string& token : cmd) {
            if (store_ids) {
                cmd_ids.push_back(token);
            } else {
                splitted_cmd.push_back(token);
            }
            store_ids = !store_ids;
        }
        sub_definition["command_ids"] = cmd_ids;
        sub_definition["command"] = splitted_cmd;
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

    void CheckSubDefinition(nlohmann::json& sub_definition) {
        // check is_string
        CheckJsonType(sub_definition, "label", JsonType::STRING);
        CheckJsonType(sub_definition, "button", JsonType::STRING, "", CAN_SKIP);
        CorrectKey(sub_definition, "window_title", "window_name");
        CorrectKey(sub_definition, "title", "window_name");
        CheckJsonType(sub_definition, "window_name", JsonType::STRING, "", CAN_SKIP);

        // check sub_definition["command"]
        CheckCommand(sub_definition);

        CheckJsonType(sub_definition, "check_exit_code", JsonType::BOOLEAN, "", CAN_SKIP);
        CheckJsonType(sub_definition, "exit_success", JsonType::INTEGER, "", CAN_SKIP);
        CheckJsonType(sub_definition, "show_last_line", JsonType::BOOLEAN, "", CAN_SKIP);

        CorrectKey(sub_definition, "component", "components");
        CorrectKey(sub_definition, "component_array", "components");
        CheckJsonType(sub_definition, "components", JsonType::JSON_ARRAY);

        // check components
        std::vector<std::string> comp_ids;
        for (nlohmann::json& c : sub_definition["components"]) {
            // check if type and label exist
            CheckJsonType(c, "label", JsonType::STRING, "components");
            std::string label = c["label"].get<std::string>();
            CheckJsonType(c, "type", JsonType::STRING, label);
            KeyToSingular(c, "default");
            std::string type_str = c["type"].get<std::string>();
            int type = COMPTYPE_TO_INT.get(type_str, COMP_UNKNOWN);
            c["type"] = type;

            switch (type) {
                case COMP_FILE:
                    CheckJsonType(c, "extention", JsonType::STRING, label, CAN_SKIP);
                case COMP_FOLDER:
                case COMP_TEXT:
                    CheckJsonType(c, "default", JsonType::STRING, label, CAN_SKIP);
                    break;
                case COMP_CHOICE:
                    CheckItemsValues(c);
                    CheckJsonType(c, "default", JsonType::INTEGER, label, CAN_SKIP);
                    break;
                case COMP_CHECK:
                    CheckJsonType(c, "value", JsonType::STRING, label, CAN_SKIP);
                    CheckJsonType(c, "default", JsonType::BOOLEAN, label, CAN_SKIP);
                    break;
                case COMP_CHECK_ARRAY:
                    CheckItemsValues(c);
                    CheckJsonType(c, "default", JsonType::BOOL_ARRAY, label, CAN_SKIP);
                    CheckArraySize(c, "default");
                    KeyToSingular(c, "tooltip");
                    CheckJsonType(c, "tooltip", JsonType::STR_ARRAY, label, CAN_SKIP);
                    CheckArraySize(c, "tooltip");
                    break;
                case COMP_INT:
                case COMP_FLOAT:
                    JsonType jtype;
                    if (type == COMP_INT) {
                        jtype = JsonType::INTEGER;
                    } else {
                        jtype = JsonType::FLOAT;
                        CheckJsonType(c, "digits", JsonType::INTEGER, label, CAN_SKIP);
                        if (c.contains("digits") && c["digits"] < 0) {
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

            CorrectKey(c, "add_quote", "add_quotes");
            CheckJsonType(c, "add_quotes", JsonType::BOOLEAN, label, CAN_SKIP);
            CorrectKey(c, "placeholder", "empty_message");
            CheckJsonType(c, "empty_message", JsonType::STRING, label, CAN_SKIP);
            CheckJsonType(c, "id", JsonType::STRING, label, CAN_SKIP);
            if (type != COMP_CHECK_ARRAY) {
                CheckJsonType(c, "tooltip", JsonType::STRING, label, CAN_SKIP);
            }

            if (c.contains("id")) {
                std::string id = c["id"].get<std::string>();
                if (id == "") {
                    throw std::runtime_error(GetLabel(label, "id")
                                             + " should NOT be an empty string.");
                } else if (id[0] == "_"[0]) {
                    throw std::runtime_error(GetLabel(label, "id")
                                             + " should NOT start with '_'.");
                }
                comp_ids.push_back(id);
            } else {
                comp_ids.push_back("");
            }
        }
        CheckIndexDuplication(comp_ids);
        sub_definition["component_ids"] = comp_ids;
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

    void CheckVersion(nlohmann::json& definition) {
        std::string k = "recommended";
        CorrectKey(definition, k + "_version", k);
        if (definition.contains(k)) {
            CheckJsonType(definition, k, JsonType::STRING);
            int recom_int = VersionStringToInt(definition[k].get<std::string>());
            CheckJsonType(definition, "not_" + k, JsonType::BOOLEAN, "", CAN_SKIP);
            definition["not_" + k] = scr_constants::VERSION_INT != recom_int;
        }
        k = "minimum_required";
        CorrectKey(definition, k + "_version", k);
        if (definition.contains(k)) {
            CheckJsonType(definition, k, JsonType::STRING);
            std::string required = definition[k].get<std::string>();
            int required_int = VersionStringToInt(required);
            if (scr_constants::VERSION_INT < required_int) {
                std::string msg = "Version " + required + " is required.";
                throw std::runtime_error(msg);
            }
        }
    }

    void CheckDefinition(nlohmann::json& definition) {
        CheckJsonType(definition, "gui", JsonType::JSON_ARRAY);
        for (nlohmann::json& sub_d : definition["gui"]) {
            CheckSubDefinition(sub_d);
        }
    }

    void CheckHelpURLs(const nlohmann::json& definition) {
        CheckJsonType(definition, "help", JsonType::JSON_ARRAY);
        for (nlohmann::json h : definition["help"]) {
            CheckJsonType(h, "type", JsonType::STRING);
            CheckJsonType(h, "label", JsonType::STRING);
            std::string type = h["type"].get<std::string>();
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
