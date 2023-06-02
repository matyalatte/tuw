#pragma once
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "wx/wx.h"
#include "wx/filepicker.h"
#include "wx/stdpaths.h"
#include "component.h"
#include "exec.h"
#include "json_utils.h"
#include "scr_constants.h"


// Main window
class MainFrame : public wxFrame {
 private:
    nlohmann::json m_definition;
    nlohmann::json m_sub_definition;
    nlohmann::json m_config;

#ifdef __linux__
    // Linux needs a window to show outputs
    LogFrame* m_log_frame;
    LogFrame* m_ostream;
#else
    std::ostream* m_ostream;
#endif

    std::vector<Component*> m_components;
    wxPanel* m_panel;
    wxButton* m_run_button;

    void SetUp();
    void CreateFrame();
    void CheckDefinition(nlohmann::json& definition);
    void UpdatePanel();
    void UpdateConfig();
    void ShowErrorDialog(const wxString& msg);
    void ShowSuccessDialog(const wxString& msg);
    nlohmann::json LoadJson(const std::string& file, bool is_definition = false);
    void JsonLoadFailed(const std::string& msg, nlohmann::json& definition);

 public:
    MainFrame(nlohmann::json definition = nlohmann::json({}),
              nlohmann::json config = nlohmann::json({}));

    void OnClose(wxCloseEvent& event);
    void OpenURL(wxCommandEvent& event);
    void UpdateFrame(wxCommandEvent& event);
    void ClickButton(wxCommandEvent& event);
    wxString GetCommand();
    std::string RunCommand();
    nlohmann::json GetDefinition();
    void SaveConfig();
};
