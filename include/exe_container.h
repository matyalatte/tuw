#pragma once
#include "rapidjson/document.h"
#include "json_utils.h"
#include "string_utils.h"

class ExeContainer {
 private:
    noex::string m_exe_path;
    uint32_t m_exe_size;
    rapidjson::Document m_json;

 public:
    ExeContainer(): m_exe_path(""),
                    m_exe_size(0),
                    m_json() {
        m_json.SetObject();
    }

    // Returns an empty string if succeed. An error message otherwise.
    noex::string Read(const noex::string& exe_path) noexcept;
    noex::string Write(const noex::string& exe_path) noexcept;

    bool HasJson() noexcept {
        return m_json.IsObject() && !m_json.ObjectEmpty();
    }

    void GetJson(rapidjson::Document& json) noexcept {
        json.CopyFrom(m_json, json.GetAllocator());
    }

    void SetJson(rapidjson::Document& json) noexcept {
        m_json.CopyFrom(json, m_json.GetAllocator());
    }

    void RemoveJson() noexcept {
        rapidjson::Document doc;
        doc.SetObject();
        SetJson(doc);
    }
};
