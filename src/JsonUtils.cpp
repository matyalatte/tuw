#include "JsonUtils.h"

namespace jsonUtils {
    nlohmann::json loadJson(std::string file) {
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
            json = {};
        }
        return json;
    }

    bool saveJson(nlohmann::json json, std::string file) {
        std::ofstream ostream(file);

        if (!ostream) {
            return false;
        }
        ostream << std::setw(4) << json << std::endl;
        ostream.close();
        return true;
    }

    bool hasKey(nlohmann::json json, std::string key) {
        auto subjectIdIter = json.find(key);
        return subjectIdIter != json.end();
    }

    //get default definition of gui
    nlohmann::json default_definition() {
        nlohmann::json def =
        {
                {"label", "Default GUI"},
    #ifdef _WIN32
                {"command", {"dir"} },
                {"button", "run 'dir'"},
    #else
                {"command", {"ls"} },
                {"button", "run 'ls'"},
    #endif
                {"components",{}}
        };
        return def;
    }

    std::string checkItemsValues(nlohmann::json& c) {
        std::string label = c["label"];
        if (!hasKey(c, "items")) {
            if (hasKey(c, "item")) {
                c["items"] = c["item"];
            }
            else {
                return label + "['items'] not found.";
            }
        }
        if (!c["items"].is_array()) {
            return label + "['items'] should be an array.";
        }
        if (!hasKey(c, "values") && hasKey(c, "value")) {
            c["values"] = c["value"];
        }
        if (hasKey(c, "values")) {
            if (!c["values"].is_array()) {
                return label + "['values'] should be an array.";
            }
            if (c["values"].size() != c["items"].size()) {
                return label + "['values'] and " + label + "['items'] should have the same size.";
            }
        }
        return "__null__";
    }
    
    std::vector<std::string> split(const std::string& s, const char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        bool store = true;
        while (std::getline(tokenStream, token, delimiter))
        {
            if (store) {
                tokens.push_back(token);
                store = false;
            }
            else {
                store = true;
            }
        }
        return tokens;
    }

    std::string checkSubDefinition(nlohmann::json& sub_definition) {
        //check if keys exist
        std::vector<std::string> keys = { "label", "button", "command", "components" };
        for (std::string key : keys) {
            if (!hasKey(sub_definition, key)) {
                return "'" + key + "' not found.";
            }
        }

        //check is_string
        keys = { "label", "button"};
        if (hasKey(sub_definition, "window_name")) {
            keys.push_back("window_name");
        }
        for (std::string key : keys) {
            if (!sub_definition[key].is_string()) {
                return "'" + key + "' should be a string.";
            }
        }

        if (sub_definition["command"].is_string()) {
            sub_definition["command"] = split(sub_definition["command"], '%');
        }
        if (!sub_definition["command"].is_array()) {
            return "'command' should be a string or an string array.";
        }

        //check is_boolean
        if (hasKey(sub_definition, "show_last_line") && !sub_definition["show_last_line"].is_boolean()) {
            return "'show_last_line' should be a boolean.";
        }

        //check is_array
        keys = { "components" };
        for (std::string key : keys) {
            if (!sub_definition[key].is_array()) {
                return "'" + key + "' should be an array.";
            }
        }

        //check components
        keys = { "type", "label" };
        std::vector<std::string> subkeys = {};
        std::string label;
        std::string msg;
        for (nlohmann::json& c : sub_definition["components"]) {
            //check if type and label exist
            for (std::string key : keys) {
                if (!hasKey(c, key)) {
                    return "components['" + key + "'] not found.";
                }
                if (!c[key].is_string()) {
                    return "components['" + key + "'] should be a string.";
                }
            }
            label = c["label"];
            if (c["type"] == "file") {
                if (hasKey(c, "extention") && !c["extension"].is_string()) {
                    return label + "['extention'] should be a string.";
                }
            }
            else if (c["type"] == "choice") {
                
                msg = checkItemsValues(c);
                if (msg != "__null__") {
                    return msg;
                }
                subkeys = { "width", "default" };
                for (std::string key : subkeys) {
                    if (hasKey(c, key) && !c[key].is_number()) {
                        return label + "['" + key + "'] should be an int.";
                    }
                }
            }
            else if (c["type"] == "check") {
                if (hasKey(c, "value") && !c["value"].is_string()) {
                    return label + "['value'] should be a string.";
                }
            }
            else if (c["type"] == "checks" || c["type"] == "check_array") {
                c["type"] = "check_array";
                msg = checkItemsValues(c);
                if (msg != "__null__") {
                    return msg;
                }
            }
            else if (c["type"] == "text_box") {
                c["type"] = "text";
            }
            if (hasKey(c, "add_quotes") && !c["add_quotes"].is_boolean()) {
                return label + "['add_quotes'] should be a boolean.";
            }
        }
        return "__null__";
    }

    std::string checkHelpURLs(nlohmann::json& definition) {
        if (!definition["help"].is_array()) {
            return "'help' should be an array.";
        }
        std::vector<std::string> keys = { "type", "label", "url" };
        for (nlohmann::json h : definition["help"]) {
            for (std::string key : keys) {
                if (!hasKey(h, key)) {
                    return "'" + key + "' not found.";
                }
                if (!h[key].is_string()) {
                    return "'" + key + "' should be a string.";
                }
            }
        }
        return "__null__";
    }

}
