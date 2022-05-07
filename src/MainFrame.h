#pragma once
#include <wx/wx.h>
#include <wx/filepicker.h>
#include <nlohmann/json.hpp>
#include "Component.h"
#include "Exec.h"
#include <fstream>
#include <iostream>

#ifndef _WIN32
#include <wx/stdpaths.h>
#endif

#ifdef __linux__
class LogFrame : public wxFrame {
private:
    wxTextCtrl* logBox;
    wxStreamToTextRedirector* logRedirector;
public:
    LogFrame(wxString exepath);
    virtual ~LogFrame();
    void OnClose(wxCloseEvent& event);
};
#endif

//Main window
class MainFrame : public wxFrame
{
private:
    nlohmann::json definition;
    nlohmann::json sub_definition;
    nlohmann::json config;
#ifdef __linux__
    LogFrame* logFrame;
#endif
    std::vector<Component> components;
    wxPanel* mainPanel;
    wxButton* runButton;

    void LoadDefinition();
    int UpdatePanel(wxPanel* panel);
    void LoadConfig();
    void UpdateConfig();
    void SaveConfig();
    void ShowErrorDialog(wxString msg);
    void ShowSuccessDialog(wxString msg);

public:
    MainFrame();
    virtual ~MainFrame();

    void OnClose(wxCloseEvent& event);
    void OpenURL(wxCommandEvent& event);
    void UpdateFrame(wxCommandEvent& event);
    void RunCommand(wxCommandEvent& event);
};
