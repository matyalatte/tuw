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

#ifdef __linux__
class LogFrame : public wxFrame {
 private:
    wxTextCtrl* m_log_box;
    wxStreamToTextRedirector* m_log_redirector;
 public:
    explicit LogFrame(wxString exepath);
    virtual ~LogFrame() {}
    void OnClose(wxCloseEvent& event);
};
#endif

// Main window
class MainFrame : public wxFrame {
 private:
    nlohmann::json m_definition;
    nlohmann::json m_sub_definition;
    nlohmann::json m_config;
#ifdef __linux__
    LogFrame* m_log_frame;
#endif
#ifndef _WIN32
    wxString m_exe_path;
    void CalcExePath();
#endif
    std::vector<Component*> m_components;
    wxPanel* m_main_panel;
    wxButton* m_run_button;

    void CreateFrame();
    void CheckDefinition();
    int UpdatePanel(wxPanel* panel);
    void UpdateConfig();
    void ShowErrorDialog(wxString msg);
    void ShowSuccessDialog(wxString msg);
    void JsonLoadFailed(std::string msg);

 public:
    MainFrame();
    explicit MainFrame(nlohmann::json definition, nlohmann::json config = nlohmann::json({}));
    virtual ~MainFrame() {}

    void OnClose(wxCloseEvent& event);
    void OpenURL(wxCommandEvent& event);
    void UpdateFrame(wxCommandEvent& event);
    void ClickButton(wxCommandEvent& event);
    std::array<std::string, 2> RunCommand();
    nlohmann::json GetDefinition();
    void SaveConfig();
};
