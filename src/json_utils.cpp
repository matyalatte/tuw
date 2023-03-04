#include "json_utils.h"

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

    bool SaveJson(nlohmann::json json, std::string file) {
        std::ofstream ostream(file);

        if (!ostream) {
            return false;
        }
        ostream << std::setw(4) << json << std::endl;
        ostream.close();
        return true;
    }

    std::string GetLabel(const std::string& label, const std::string& key) {
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

    void CheckItemsValues(nlohmann::json& c) {
        std::string label = c["label"];
        if (!c.contains("items")) {
            if (c.contains("item")) {
                c["items"] = c["item"];
            } else {
                Raise(GetLabel(label, "items") + " not found.");
            }
        }
        CheckJsonType(c, "items", JsonType::STR_ARRAY, label);
        if (!c.contains("values") && c.contains("value")) {
            c["values"] = c["value"];
        }
        std::string key = "values";
        CheckJsonType(c, key, JsonType::STR_ARRAY, label, CAN_SKIP);
        if (c.contains(key)) {
            if (c[key].size() != c["items"].size()) {
                Raise(GetLabel(label, key) + " and " +
                    GetLabel(label, "items") + " should have the same size.");
            }
        }
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
        while (s.length() > offset + 1) {
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

    void CheckSubDefinition(nlohmann::json& sub_definition) {
        // check is_string
        CheckJsonType(sub_definition, "label", JsonType::STRING);
        CheckJsonType(sub_definition, "button", JsonType::STRING, "", CAN_SKIP);
        CheckJsonType(sub_definition, "window_name", JsonType::STRING, "", CAN_SKIP);

        // check sub_definition["command"]
        CheckCommand(sub_definition);

        // check is_boolean
        CheckJsonType(sub_definition, "show_last_line", JsonType::BOOLEAN, "", CAN_SKIP);

        // check is_array
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
            type = c["type"];

            if (type == "file") {
                CheckJsonType(c, "extention", JsonType::STRING, label, CAN_SKIP);
            } else if (type == "choice") {
                CheckItemsValues(c);
                CheckJsonType(c, "default", JsonType::INTEGER, label, CAN_SKIP);
            } else if (type == "check") {
                CheckJsonType(c, "value", JsonType::STRING, label, CAN_SKIP);
                CheckJsonType(c, "default", JsonType::BOOLEAN, label, CAN_SKIP);
            } else if (type == "checks" || type == "check_array") {
                CheckItemsValues(c);
                std::string key = "default";
                CheckJsonType(c, key, JsonType::BOOL_ARRAY, label, true);
                if (c.contains(key) && c[key].size() != c["items"].size()) {
                    Raise(GetLabel(label, key) + " and " +
                        GetLabel(label, "items") + " should have the same size.");
                }
            }
            if (type == "text" || type == "text_box" || type == "file" || type == "folder") {
                CheckJsonType(c, "default", JsonType::STRING, label, CAN_SKIP);
            }
            CheckJsonType(c, "add_quotes", JsonType::BOOLEAN, label, CAN_SKIP);
            CheckJsonType(c, "empty_message", JsonType::STRING, label, CAN_SKIP);
            CheckJsonType(c, "id", JsonType::STRING, label, CAN_SKIP);
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

    void CheckDefinition(nlohmann::json& definition) {
        CheckJsonType(definition, "gui", JsonType::JSON_ARRAY);
        for (nlohmann::json& sub_d : definition["gui"]) {
            CheckSubDefinition(sub_d);
        }
    }

    void CheckHelpURLs(const nlohmann::json& definition) {
        CheckJsonType(definition, "help", JsonType::JSON_ARRAY);
        std::vector<std::string> keys = { "type", "label", "url" };
        for (nlohmann::json h : definition["help"]) {
            for (std::string key : keys) {
                CheckJsonType(h, key, JsonType::STRING);
            }
        }
    }
}  // namespace json_utils
