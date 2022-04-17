#pragma once
#include <wx/wx.h>
#include <wx/filepicker.h>
#pragma once
#include <nlohmann/json.hpp>
#include "Component.h"



//Main window
class MainFrame : public wxFrame
{
private:
    nlohmann::json definition;
    nlohmann::json sub_definition;
    std::vector<Component> components;
    wxPanel* mainPanel;

    void ReadDefinition();
    int UpdatePanel(wxPanel* panel);

public:
    //std::array<std::string, 2> names = { "Frame1", "Frame2" };
    //wxFilePickerCtrl* filePicker;
    //wxFilePickerCtrl* filePicker2;
    //wxDirPickerCtrl* folderPicker;
    //wxChoice* choice;
    MainFrame();
    virtual ~MainFrame();

    void OnClose(wxCloseEvent& event);
    void OpenURL(wxCommandEvent& event);
    void UpdateFrame(wxCommandEvent& event);
    void RunCommand(wxCommandEvent& event);
};
