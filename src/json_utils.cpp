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
        NUMBER,
        BOOLEAN,
        ARRAY
    };

    void Raise(std::string msg) {
        throw std::runtime_error(msg);
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
        case JsonType::NUMBER:
            valid = j[key].is_number();
            type_name = "an int";
            break;
        case JsonType::BOOLEAN:
            valid = j[key].is_boolean();
            type_name = "a boolean";
            break;
        case JsonType::ARRAY:
            valid = j[key].is_array();
            type_name = "an array";
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
                {"components", nlohmann::json::array({})}
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
        CheckJsonType(c, "items", JsonType::ARRAY, label);
        if (!c.contains("values") && c.contains("value")) {
            c["values"] = c["value"];
        }
        std::string key = "values";
        CheckJsonType(c, key, JsonType::ARRAY, label, CAN_SKIP);
        if (c.contains(key)) {
            if (c[key].size() != c["items"].size()) {
                Raise(GetLabel(label, key) + " and " +
                    GetLabel(label, "items") + " should have the same size.");
            }
        }
    }

    std::vector<std::string> split(const std::string& s, const char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        bool store = true;
        while (std::getline(tokenStream, token, delimiter)) {
            if (store) {
                tokens.push_back(token);
                store = false;
            } else {
                store = true;
            }
        }
        return tokens;
    }

    const std::string COMMAND = "command";
    const std::string COMPONENTS = "components";

    void CheckSubDefinition(nlohmann::json& sub_definition) {
        // check is_string
        CheckJsonType(sub_definition, "label", JsonType::STRING);
        CheckJsonType(sub_definition, "button", JsonType::STRING, "", CAN_SKIP);
        CheckJsonType(sub_definition, "window_name", JsonType::STRING, "", CAN_SKIP);

        CheckContain(sub_definition, COMMAND, "");
        if (sub_definition[COMMAND].is_string()) {
            sub_definition[COMMAND] = split(sub_definition[COMMAND], '%');
        }
        CheckJsonType(sub_definition, COMMAND, JsonType::ARRAY);

        // check is_boolean
        CheckJsonType(sub_definition, "show_last_line", JsonType::BOOLEAN, "", CAN_SKIP);

        // check is_array
        CheckJsonType(sub_definition, COMPONENTS, JsonType::ARRAY);

        // check components
        std::string label;
        std::string type;
        for (nlohmann::json& c : sub_definition[COMPONENTS]) {
            // check if type and label exist
            CheckJsonType(c, "type", JsonType::STRING, COMPONENTS);
            CheckJsonType(c, "label", JsonType::STRING, COMPONENTS);
            label = c["label"];
            type = c["type"];

            if (type == "file") {
                CheckJsonType(c, "extention", JsonType::STRING, label, CAN_SKIP);
            } else if (type == "choice") {
                CheckItemsValues(c);
                CheckJsonType(c, "default", JsonType::NUMBER, label, CAN_SKIP);
            } else if (type == "check") {
                CheckJsonType(c, "value", JsonType::STRING, label, CAN_SKIP);
                CheckJsonType(c, "default", JsonType::BOOLEAN, label, CAN_SKIP);
            } else if (type == "checks" || type == "check_array") {
                CheckItemsValues(c);
                std::string key = "default";
                CheckJsonType(c, key, JsonType::ARRAY, label, true);
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
        }
    }

    void CheckDefinition(nlohmann::json& definition) {
        CheckJsonType(definition, "gui", JsonType::ARRAY);
        for (nlohmann::json& sub_d : definition["gui"]) {
            CheckSubDefinition(sub_d);
        }
    }

    void CheckHelpURLs(const nlohmann::json& definition) {
        CheckJsonType(definition, "help", JsonType::ARRAY);
        std::vector<std::string> keys = { "type", "label", "url" };
        for (nlohmann::json h : definition["help"]) {
            for (std::string key : keys) {
                CheckJsonType(h, key, JsonType::STRING);
            }
        }
    }
}  // namespace json_utils
