#pragma once
#include <vector>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "wx/wx.h"
#include "wx/filepicker.h"
#include "wx/stdpaths.h"
#include "wx/stream.h"
#include "wx/app.h"
#include "component.h"
#include "exec.h"
#include "json_utils.h"
#include "scr_constants.h"
#include "exe_container.h"
#include "wx_utils.h"

// Main window
class MainFrame : public wxFrame {
 private:
    rapidjson::Document m_definition;
    int m_definition_id;
    rapidjson::Document m_config;

#ifdef __linux__
    // Linux needs a window to show outputs
    LogFrame* m_log_frame;
    LogFrame* m_ostream;
#endif

    std::vector<Component*> m_components;
    wxPanel* m_panel;
    wxButton* m_run_button;

    void SetUp();
    void CreateFrame();
    void CheckDefinition(rapidjson::Document& definition);
    void UpdatePanel();
    void UpdateConfig();
    void ShowErrorDialog(const wxString& msg, const wxString& title = "Error");
    void ShowSuccessDialog(const wxString& msg, const wxString& title = "Success");
    void LoadJson(const std::string& file, rapidjson::Document& json, bool is_definition = false);
    void JsonLoadFailed(const wxString& msg, rapidjson::Document& definition);

 public:
    MainFrame(const rapidjson::Document& definition = rapidjson::Document(rapidjson::kObjectType),
              const rapidjson::Document& config = rapidjson::Document(rapidjson::kObjectType));

    void OnClose(wxCloseEvent& event);
    void OnCommandClose(wxCommandEvent& event) { Close(true); }
    void OpenURL(wxCommandEvent& event);
    void UpdateFrame(wxCommandEvent& event);
    void ClickButton(wxCommandEvent& event);
    wxString GetCommand();
    wxResult RunCommand(wxString& last_line);
    void GetDefinition(rapidjson::Document& json);
    void SaveConfig();
};
