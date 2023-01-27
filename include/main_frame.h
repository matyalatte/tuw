#pragma once
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "wx/wx.h"
#include "wx/filepicker.h"
#include "component.h"
#include "exec.h"
#include "json_utils.h"

#ifndef _WIN32
#include <wx/stdpaths.h>
#endif

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

#ifndef _WIN32
    // Unix systems need to get current dir to read json files.
    wxString m_exe_path;
    void CalcExePath();
#endif

    std::vector<Component*> m_components;
    wxPanel* m_panel;
    wxButton* m_run_button;

    void CreateFrame();
    void CheckDefinition();
    void UpdatePanel();
    void UpdateConfig();
    void ShowErrorDialog(wxString msg);
    void ShowSuccessDialog(wxString msg);
    void JsonLoadFailed(std::string msg);

 public:
    MainFrame();
    explicit MainFrame(nlohmann::json definition, nlohmann::json config = nlohmann::json({}));

    void OnClose(wxCloseEvent& event);
    void OpenURL(wxCommandEvent& event);
    void UpdateFrame(wxCommandEvent& event);
    void ClickButton(wxCommandEvent& event);
    wxString GetCommand();
    std::array<std::string, 2> RunCommand();
    nlohmann::json GetDefinition();
    void SaveConfig();
};
