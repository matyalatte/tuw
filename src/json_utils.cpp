#include "json_utils.h"

const int VERSION_INT = 300;

namespace json_utils {
    nlohmann::json LoadJson(std::string file) {
        std::ifstream istream(file);
        nlohmann::json json;
        if (!istream) {
            return nlohmann::json({});
        }
        try {
            istream >> json;
            istream.close();
        }
        catch (...) {
            json = nlohmann::json({});
        }
        return json;
    }

    bool SaveJson(nlohmann::json& json, std::string file) {
        std::ofstream ostream(file);

        if (!ostream) {
            return false;
        }
        ostream << std::setw(4) << json << std::endl;
        ostream.close();
        return true;
    }

    std::string GetLabel(const std::string& label, const std::string& key) {
        // Todo: std::string to wchar for windows
        std::string msg = "['" + key + "']";
        if (label != "") {
            msg = "['" + label + "']" + msg;
        }
        return msg;
    }

    void CheckContain(const nlohmann::json& j,
                      const std::string& key, const std::string& label = "") {
        if (!j.contains(key)) {
            std::string msg = GetLabel(label, key) + " not found.";
            throw std::runtime_error(msg);
        }
    }

    enum class JsonType {
        STRING,
        INTEGER,
        BOOLEAN,
        JSON_ARRAY,
        STR_ARRAY,
        INT_ARRAY,
        BOOL_ARRAY
    };

    void Raise(std::string msg) {
        throw std::runtime_error(msg);
    }

    bool IsArray(const nlohmann::json& j, const std::string& key, const JsonType& type) {
        if (!j[key].is_array()) { return false; }
        std::function<bool(const nlohmann::json&)> lmd;
        switch (type) {
        case JsonType::JSON_ARRAY:
            lmd = [](const nlohmann::json& el){ return el.is_object(); };
            break;
        case JsonType::STR_ARRAY:
            lmd = [](const nlohmann::json& el){ return el.is_string(); };
            break;
        case JsonType::INT_ARRAY:
            lmd = [](const nlohmann::json& el){ return el.is_number_integer(); };
            break;
        case JsonType::BOOL_ARRAY:
            lmd = [](const nlohmann::json& el){ return el.is_boolean(); };
            break;
        default:
            return false;
        }
        return std::all_of(j[key].begin(), j[key].end(), lmd);
    }

    const bool CAN_SKIP = true;

    void CheckJsonType(const nlohmann::json& j, const std::string& key,
        const JsonType& type, const std::string& label = "", const bool& canSkip = false) {
        if (canSkip && !j.contains(key)) {
            return;
        }
        CheckContain(j, key, label);

        bool valid = false;
        std::string type_name;
        switch (type) {
        case JsonType::STRING:
            valid = j[key].is_string();
            type_name = "a string";
            break;
        case JsonType::INTEGER:
            valid = j[key].is_number_integer();
            type_name = "an int";
            break;
        case JsonType::BOOLEAN:
            valid = j[key].is_boolean();
            type_name = "a boolean";
            break;
        case JsonType::JSON_ARRAY:
            valid = IsArray(j, key, type);
            type_name = "an array of json objects";
            break;
        case JsonType::STR_ARRAY:
            valid = IsArray(j, key, type);
            type_name = "an array of strings";
            break;
        case JsonType::INT_ARRAY:
            valid = IsArray(j, key, type);
            type_name = "an array of integers";
            break;
        case JsonType::BOOL_ARRAY:
            valid = IsArray(j, key, type);
            type_name = "an array of booleans";
            break;
        default:
            break;
        }
        if (!valid) {
            Raise(GetLabel(label, key) + " should be " + type_name + ".");
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

    void CorrectKey(nlohmann::json& j, std::string false_key, std::string true_key) {
        if (!j.contains(true_key) && j.contains(false_key)) {
            j[true_key] = j[false_key];
            j.erase(false_key);
        }
    }

    void KeyToSingular(nlohmann::json& c, std::string singular) {
        std::vector<std::string> extends = {"s", "_array"};
        std::string plural;
        for (std::string ext : extends) {
            plural = singular + ext;
            CorrectKey(c, plural, singular);
        }
    }

    void CheckArraySize(nlohmann::json& c, std::string key) {
        if (c.contains(key) && (c[key].size() != c["item"].size())) {
            std::string label = c["label"];
            Raise(GetLabel(label, key) + " and " +
                GetLabel(label, "item") + " should have the same size.");
        }
    }

    void CheckItemsValues(nlohmann::json& c) {
        std::string label = c["label"];
        KeyToSingular(c, "item");
        CheckJsonType(c, "item", JsonType::STR_ARRAY, label);
        KeyToSingular(c, "value");
        CheckJsonType(c, "value", JsonType::STR_ARRAY, label, CAN_SKIP);
        CheckArraySize(c, "value");
    }

    std::vector<std::string> SplitString(const std::string& s, const std::string& delimiter) {
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

    const std::string COMMAND = "command";
    const std::string COMPONENTS = "components";

    void CheckIndexDuplication(const std::vector<std::string>& component_ids) {
        int size = component_ids.size();
        std::string str;
        for (int i = 0; i < size - 1; i++) {
            str = component_ids[i];
            if (str == "") { continue; }
            for (int j = i + 1; j < size; j++) {
                if (str == component_ids[j]) {
                    Raise(GetLabel(COMPONENTS, "id")
                          + " should not be duplicated in a gui definition. (" + str + ")");
                }
            }
        }
    }

    void CheckCommand(nlohmann::json& sub_definition) {
        CheckContain(sub_definition, COMMAND, "");
        CheckJsonType(sub_definition, "command_ids", JsonType::STR_ARRAY, "", CAN_SKIP);
        if (sub_definition[COMMAND].is_string()) {
            sub_definition[COMMAND] = SplitString(sub_definition[COMMAND], { '%' });
        } else {
            CheckJsonType(sub_definition, COMMAND, JsonType::STR_ARRAY);
        }

        std::vector<std::string> cmd = sub_definition[COMMAND];
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
        sub_definition[COMMAND] = splitted_cmd;
    }

    void CorrectValue(nlohmann::json& j, std::string key,
                      std::string false_value, std::string true_value) {
        if (j[key] == false_value) {
            j[key] = true_value;
        }
    }

    void CheckSubDefinition(nlohmann::json& sub_definition) {
        // check is_string
        CheckJsonType(sub_definition, "label", JsonType::STRING);
        CheckJsonType(sub_definition, "button", JsonType::STRING, "", CAN_SKIP);
        CorrectKey(sub_definition, "window_title", "window_name");
        CorrectKey(sub_definition, "title", "window_name");
        CheckJsonType(sub_definition, "window_name", JsonType::STRING, "", CAN_SKIP);

        // check sub_definition["command"]
        CheckCommand(sub_definition);

        CheckJsonType(sub_definition, "show_last_line", JsonType::BOOLEAN, "", CAN_SKIP);

        CorrectKey(sub_definition, "component", COMPONENTS);
        CorrectKey(sub_definition, "component_array", COMPONENTS);
        CheckJsonType(sub_definition, COMPONENTS, JsonType::JSON_ARRAY);

        // check components
        std::string label;
        std::string type;
        std::vector<std::string> comp_ids;
        for (nlohmann::json& c : sub_definition[COMPONENTS]) {
            // check if type and label exist
            CheckJsonType(c, "label", JsonType::STRING, COMPONENTS);
            label = c["label"];
            CheckJsonType(c, "type", JsonType::STRING, label);
            CorrectValue(c, "type", "dir", "folder");
            CorrectValue(c, "type", "combo", "choice");
            CorrectValue(c, "type", "checks", "check_array");
            CorrectValue(c, "type", "text_box", "text");
            KeyToSingular(c, "default");
            type = c["type"];

            if (type == "file") {
                CheckJsonType(c, "extention", JsonType::STRING, label, CAN_SKIP);
            } else if (type == "choice") {
                CheckItemsValues(c);
                CheckJsonType(c, "default", JsonType::INTEGER, label, CAN_SKIP);
            } else if (type == "check") {
                CheckJsonType(c, "value", JsonType::STRING, label, CAN_SKIP);
                CheckJsonType(c, "default", JsonType::BOOLEAN, label, CAN_SKIP);
            } else if (type == "check_array") {
                CheckItemsValues(c);
                CheckJsonType(c, "default", JsonType::BOOL_ARRAY, label, CAN_SKIP);
                CheckArraySize(c, "default");
                KeyToSingular(c, "tooltip");
                CheckJsonType(c, "tooltip", JsonType::STR_ARRAY, label, CAN_SKIP);
                CheckArraySize(c, "tooltip");
            }
            if (type == "text" || type == "file" || type == "folder") {
                CheckJsonType(c, "default", JsonType::STRING, label, CAN_SKIP);
            }

            CorrectKey(c, "add_quote", "add_quotes");
            CheckJsonType(c, "add_quotes", JsonType::BOOLEAN, label, CAN_SKIP);
            CorrectKey(c, "placeholder", "empty_message");
            CheckJsonType(c, "empty_message", JsonType::STRING, label, CAN_SKIP);
            CheckJsonType(c, "id", JsonType::STRING, label, CAN_SKIP);
            if (type != "check_array") {
                CheckJsonType(c, "tooltip", JsonType::STRING, label, CAN_SKIP);
            }


            if (c.contains("id")) {
                comp_ids.push_back(c["id"]);
            } else {
                comp_ids.push_back("");
            }
        }
        CheckJsonType(sub_definition, "component_ids", JsonType::STR_ARRAY, "", CAN_SKIP);
        CheckIndexDuplication(comp_ids);
        sub_definition["component_ids"] = comp_ids;
    }

    // vX.Y.Z -> 10000*X + 100 * Y + Z
    int VersionStringToInt(std::string string) {
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
            Raise("Can NOT convert '" + string + "' to int.");
        }
    }

    void CheckVersion(nlohmann::json& definition) {
        std::string k = "recommended";
        CorrectKey(definition, k + "_version", k);
        if (definition.contains(k)) {
            CheckJsonType(definition, k, JsonType::STRING);
            int recom_int = VersionStringToInt(definition[k]);
            CheckJsonType(definition, "not_" + k, JsonType::BOOLEAN, "", CAN_SKIP);
            definition["not_" + k] = VERSION_INT != recom_int;
        }
        k = "minimum_required";
        CorrectKey(definition, k + "_version", k);
        if (definition.contains(k)) {
            CheckJsonType(definition, k, JsonType::STRING);
            std::string required = definition[k];
            int required_int = VersionStringToInt(required);
            if (VERSION_INT < required_int) {
                std::string msg = "Version " + required + " is required.";
                Raise(msg);
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
            if (h["type"] == "url") {
                CheckJsonType(h, "url", JsonType::STRING);
            } else if (h["type"] == "file") {
                CheckJsonType(h, "path", JsonType::STRING);
            }
        }
    }
}  // namespace json_utils
