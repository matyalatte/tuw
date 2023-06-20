#pragma once
#include "wx/wx.h"
#include "wx/file.h"
#include "wx/buffer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"

class ExeContainer {
 private:
    wxString m_exe_path;
    wxUint32 m_exe_size;
    rapidjson::Document m_json;

 public:
    ExeContainer(): m_exe_path(""),
                    m_exe_size(0),
                    m_json() {}
    void Read(const wxString& exe_path);
    void Write(const wxString& exe_path);
    bool HasJson() { return m_json.Size() != 0; }
    void GetJson(rapidjson::Document& json) { json.CopyFrom(m_json, json.GetAllocator()); }
    void SetJson(rapidjson::Document& json) { m_json.CopyFrom(json, m_json.GetAllocator()); }
    void RemoveJson() {
        rapidjson::Document doc;
        doc.SetObject();
        SetJson(doc);
    }
};
