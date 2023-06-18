#pragma once
#include "wx/wx.h"
#include "wx/file.h"
#include "wx/buffer.h"
#include <nlohmann/json.hpp>

class ExeContainer {
 private:
    wxString m_exe_path;
    wxUint32 m_exe_size;
    nlohmann::json m_json;

 public:
    ExeContainer(): m_exe_path(""),
                    m_exe_size(0),
                    m_json(nlohmann::json::array({})) {}
    void Read(const wxString& exe_path);
    void Write(const wxString& exe_path);
    bool HasJson() { return !m_json.empty(); }
    nlohmann::json GetJson() { return m_json; }
    void SetJson(nlohmann::json json) { m_json = json; }
    void RemoveJson() { SetJson(nlohmann::json::array({})); }
};
