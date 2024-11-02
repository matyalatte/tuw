#pragma once
#include "rapidjson/document.h"
#include "json_utils.h"
#include "string_utils.h"

class ExeContainer {
 private:
    tuwString m_exe_path;
    uint32_t m_exe_size;
    rapidjson::Document m_json;

 public:
    ExeContainer(): m_exe_path(""),
                    m_exe_size(0),
                    m_json() {
        m_json.SetObject();
    }
    json_utils::JsonResult Read(const tuwString& exe_path);
    json_utils::JsonResult Write(const tuwString& exe_path);
    bool HasJson() { return m_json.IsObject() && !m_json.ObjectEmpty(); }
    void GetJson(rapidjson::Document& json) { json.CopyFrom(m_json, json.GetAllocator()); }
    void SetJson(rapidjson::Document& json) { m_json.CopyFrom(json, m_json.GetAllocator()); }
    void RemoveJson() {
        rapidjson::Document doc;
        doc.SetObject();
        SetJson(doc);
    }
};
