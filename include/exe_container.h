#pragma once
#include "json.h"
#include "json_utils.h"
#include "string_utils.h"

class ExeContainer {
 private:
    noex::string m_exe_path;
    uint32_t m_exe_size;
    tuwjson::Value m_json;

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
        return m_json.IsObject() && !m_json.IsEmptyObject();
    }

    void GetJson(tuwjson::Value& json) noexcept {
        json.CopyFrom(m_json);
    }

    void SetJson(tuwjson::Value& json) noexcept {
        m_json.CopyFrom(json);
    }

    void RemoveJson() noexcept {
        tuwjson::Value doc;
        doc.SetObject();
        SetJson(doc);
    }
};
