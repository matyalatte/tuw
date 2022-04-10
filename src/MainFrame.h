#pragma once
#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/dnd.h>
#include <nlohmann/json.hpp>

//Drop target for path picker
template <typename T>
class DropFilePath : public wxFileDropTarget
{
private:
    T* frame;
public:
    DropFilePath(T* frame);
    ~DropFilePath();
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override;
};

//Main window
class MainFrame : public wxFrame
{
private:
    nlohmann::json definition;
    int hasChoice;
    void readDefinition();
    int UpdatePanel(wxPanel* panel);

public:
    //std::array<std::string, 2> names = { "Frame1", "Frame2" };
    wxFilePickerCtrl* filePicker;
    wxFilePickerCtrl* filePicker2;
    wxDirPickerCtrl* folderPicker;
    wxChoice* choice;
    MainFrame();
    virtual ~MainFrame();

    void OnClose(wxCloseEvent& event);
    void UpdateFrame(wxCommandEvent& event);
    void RunCommand(wxCommandEvent& event);
};
