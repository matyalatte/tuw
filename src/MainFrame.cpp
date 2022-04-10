#pragma once
#include "MainFrame.h"
#include "Exec.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <array>

//Drop target for path picker
template <typename T>
DropFilePath<T>::DropFilePath(T* frame) : wxFileDropTarget() {
    this->frame = frame;
}

template <typename T>
DropFilePath<T>::~DropFilePath() {
}

template <typename T>
bool DropFilePath<T>::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
    std::cout << filenames[0].c_str();
    this->frame->SetPath(filenames[0]);
    return 1;
}

//Main window
MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Simple Command Runner")
{
    //get gui definition
    this->readDefinition();
    this->SetLabel(this->definition["name"]);

    //make menu bar
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* menuFile = new wxMenu;
    /*
    for (int i = 0; i < this->names.size(); i++) {
        menuFile->Append(wxID_HIGHEST + i + 1, this->names[i]);
        menuFile->Bind(wxEVT_MENU, &MainFrame::UpdateFrame, this, wxID_HIGHEST + i + 1);
    }*/
    menuFile->Append(wxID_EXIT, "Quit");
    menuBar->Append(menuFile, "Menu");
    SetMenuBar(menuBar);

    //set close event
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Close(true); }, wxID_EXIT);
    
    //put components
    wxPanel* panel = new wxPanel(this);
    int y = this->UpdatePanel(panel);

    //run button
    wxButton* button = new wxButton(panel, wxID_EXECUTE, this->definition["button"], wxPoint(165+this->hasChoice*130, y), wxSize(75, 25));
    Connect(wxID_EXECUTE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::RunCommand));
    
    panel->Show();

    Layout();
    Centre();
    this->SetSize(wxSize(405, y+105));
    this->SetWindowStyleFlag(wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
}

bool hasKey(nlohmann::json json, std::string key) {
    auto subjectIdIter = json.find(key);
    return subjectIdIter != json.end();
}

//get default definition of gui
nlohmann::json default_definition(std::string msg) {
    std::cout << msg << std::endl;
    nlohmann::json def =
        {
            {"name", "Simple Command Runner"},
            {"command", {"cmd.exe /c dir"} },
            {"button", "run 'dir'"},
            {"components",{}}
        };
    return def;
}

//read gui_definition.json
void MainFrame::readDefinition() {
    std::ifstream istream("gui_definition.json");

    if (!istream) {
        this->definition = default_definition("Fialed to read gui_definition.json");
        return;
    }

    //read json file
    istream >> this->definition;

    //check format
    if (hasKey(this->definition, "gui")) {
        this->definition = this->definition["gui"][0];
    }
    else {
        this->definition = default_definition("Json format error ('gui' not found)");
        return;
    }
    std::vector<std::string> keys = { "name", "button", "command", "components" };
    for (std::string key : keys) {
        if (!hasKey(this->definition, key)) {
            this->definition = default_definition("Json format error ('" + key + "' not found)");
            return;
        }
    }
    if (this->definition["command"].size() < this->definition["components"].size()) {
        this->definition = default_definition("Json format error (size of 'command' should be larger than size of 'components')");
        return;
    }
}

//run command
void MainFrame::RunCommand(wxCommandEvent& event) {

    //make command string
    std::vector<std::string> cmd_ary = this->definition["command"];
    wxString cmd = "";
    int i = 0;
    if (this->definition["components"].size() > i && this->definition["components"][i]["type"] == "file"){
        wxString file = this->filePicker->GetPath();
        cmd += cmd_ary[i] + file;
        i += 1;
    }
    if (this->definition["components"].size() > i && this->definition["components"][i]["type"] == "file"){
        wxString file2 = this->filePicker->GetPath();
        cmd += cmd_ary[i] + file2;
        i += 1;
    }
    if (this->definition["components"].size() > i && this->definition["components"][i]["type"] == "folder") {
        wxString folder = this->folderPicker->GetPath();
        cmd += cmd_ary[i] + folder;
        i += 1;
    }
    if (this->definition["components"].size() > i && this->definition["components"][i]["type"] == "choice") {
        wxString item = this->choice->GetStringSelection();
        cmd += cmd_ary[i] + item;
        i += 1;
    }
    if (cmd_ary.size() > i) {
        cmd += cmd_ary[i];
    }

    //run command
    std::cout << cmd << std::endl;
    std::string err_msg = exec(cmd);

    if (err_msg == "__null__") {
        return;
    }

    //show result
    wxMessageDialog* dialog;
    if (err_msg != "") {//if error
        dialog = new wxMessageDialog(this, err_msg, "Error", wxICON_ERROR | wxOK | wxCENTRE);
    }
    else {//if success
        dialog = new wxMessageDialog(this, "Success!", "Success");
    }
    dialog->ShowModal();
    delete dialog;
}

MainFrame::~MainFrame()
{
}

void MainFrame::UpdateFrame(wxCommandEvent& event)
{
    //std::string str = this->names[event.GetId()-1- wxID_HIGHEST];
    //std::cout << str.c_str();
    //this->SetLabel(str);
}

//put components
int MainFrame::UpdatePanel(wxPanel* panel)
{
    this->hasChoice = 0;

    //file picker
    int i = 0;
    int y = 10;
    if (this->definition["components"].size() > i && this->definition["components"][i]["type"] == "file") {
        wxStaticText* text = new wxStaticText(panel, wxID_ANY, this->definition["components"][i]["label"], wxPoint(20, y));
        this->filePicker = new wxFilePickerCtrl(panel, wxID_ANY, "", "", this->definition["components"][i]["extension"], wxPoint(20, y + 15), wxSize(350, 25));
        this->filePicker->GetTextCtrl()->SetDropTarget(new DropFilePath<wxFilePickerCtrl>(filePicker));
        this->filePicker->DragAcceptFiles(true);
        i += 1;
        y += 50;
    }

    //file picker2
    if (this->definition["components"].size() > i && this->definition["components"][i]["type"]=="file") {
        wxStaticText* text2 = new wxStaticText(panel, wxID_ANY, this->definition["components"][i]["label"], wxPoint(20, y));
        this->filePicker2 = new wxFilePickerCtrl(panel, wxID_ANY, "", "", this->definition["components"][i]["extension"], wxPoint(20, y + 15), wxSize(350, 25));
        this->filePicker2->GetTextCtrl()->SetDropTarget(new DropFilePath<wxFilePickerCtrl>(filePicker2));
        this->filePicker2->DragAcceptFiles(true);
        i += 1;
        y += 50;
    }

    //folder picker
    if (this->definition["components"].size() > i && this->definition["components"][i]["type"] == "folder") {
        wxStaticText* text3 = new wxStaticText(panel, wxID_ANY, this->definition["components"][i]["label"], wxPoint(20, y));
        this->folderPicker = new wxDirPickerCtrl(panel, wxID_ANY, "", "", wxPoint(20, y + 15), wxSize(350, 25));
        this->folderPicker->GetTextCtrl()->SetDropTarget(new DropFilePath<wxDirPickerCtrl>(folderPicker));
        this->folderPicker->DragAcceptFiles(true);
        i += 1;
        y += 50;
    }
    
    //choice
    if (this->definition["components"].size() > i && this->definition["components"][i]["type"] == "choice") {
        wxArrayString wxitems;
        std::vector<std::string> items = this->definition["components"][i]["items"];
        std::for_each(items.begin(), items.end(), [&](std::string i) {
            wxitems.Add(i);
            });
        wxStaticText* text4 = new wxStaticText(panel, wxID_ANY, this->definition["components"][i]["label"], wxPoint(20, y));
        this->choice = new wxChoice(panel, wxID_ANY, wxPoint(20, y + 20), wxSize(95, 25), wxitems);
        this->choice->SetSelection(this->definition["components"][i]["default"]);

        this->hasChoice = 1;
    }
    y += 20;
    return y;
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    Destroy();
}