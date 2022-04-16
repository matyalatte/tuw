#pragma once
#include "MainFrame.h"
#include "Exec.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <array>


//Main window
MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Simple Command Runner")
{
    //get gui definition
    this->readDefinition();
    this->SetLabel(this->sub_definition["name"]);

    //make menu bar
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* menuFile = new wxMenu;
    
    for (int i = 0; i < this->definition["gui"].size(); i++) {
        menuFile->Append(wxID_HIGHEST + i + 1, this->definition["gui"][i]["name"]);
        menuFile->Bind(wxEVT_MENU, &MainFrame::UpdateFrame, this, wxID_HIGHEST + i + 1);
    }
    menuFile->Append(wxID_EXIT, "Quit");
    menuBar->Append(menuFile, "Menu");
    SetMenuBar(menuBar);

    //set close event
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Close(true); }, wxID_EXIT);
    
    //put components
    mainPanel = new wxPanel(this);
    int y = this->UpdatePanel(mainPanel);

    //run button
    wxButton* button = new wxButton(mainPanel, wxID_EXECUTE, this->sub_definition["button"], wxPoint(165+this->hasChoice*130, y), wxSize(75, 25));
    Connect(wxID_EXECUTE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::RunCommand));
    
    mainPanel->Show();

    Layout();
    Centre();
    this->SetSize(wxSize(405, y+105));
    this->SetWindowStyleFlag(wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
}

//get default definition of gui
nlohmann::json default_definition(std::string msg) {
    std::cout << msg << std::endl;
    nlohmann::json def =
        {
            {"name", "Simple Command Runner"},
#ifdef _WIN32
            {"command", {"cmd.exe /c dir"} },
            {"button", "run 'dir'"},
#else
            {"command", {"ls"} },
            {"button", "run 'ls'"},
#endif
            {"components",{}}
        };
    return def;
}

/*
bool hasKey(nlohmann::json json, std::string key) {
    auto subjectIdIter = json.find(key);
    return subjectIdIter != json.end();
}
*/
//read gui_definition.json
void MainFrame::readDefinition() {
    std::ifstream istream("gui_definition.json");

    if (!istream) {
        this->sub_definition = default_definition("[readDefinition] Fialed to read gui_definition.json");
        this->definition = { "gui", {this->sub_definition} };
        return;
    }

    //read json file
    istream >> this->definition;

    //check format
    if (hasKey(this->definition, "gui")) {
        this->sub_definition = this->definition["gui"][0];
    }
    else {
        this->sub_definition = default_definition("[readDefinition] Json format error ('gui' not found)");
        return;
    }
    std::vector<std::string> keys = { "name", "button", "command", "components" };
    for (std::string key : keys) {
        if (!hasKey(this->sub_definition, key)) {
            this->sub_definition = default_definition("[readDefinition] Json format error ('" + key + "' not found)");
            return;
        }
    }
}

//run command
void MainFrame::RunCommand(wxCommandEvent& event) {

    //make command string
    std::vector<std::string> cmd_ary = this->sub_definition["command"];
    wxString cmd = "";
    int i = 0;
    std::for_each(this->components.begin(), this->components.end(), [&](Component c) {
        if (c.HasString()) {
            cmd += cmd_ary[i] + c.GetString();
            i += 1;
        }
        });
    while (cmd_ary.size() > i) {
        cmd += cmd_ary[i];
        i += 1;
    }

    //run command
    std::cout << "[RunCommand] command: " << cmd << std::endl;
    std::string err_msg = exec(cmd);

    if (err_msg == "__null__") {
        std::cout << "[RunCommand] Execution failed. " << std::endl;
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

MainFrame::~MainFrame(){}

void MainFrame::UpdateFrame(wxCommandEvent& event)
{
    std::string str = definition["gui"][event.GetId() - 1 - wxID_HIGHEST]["name"];
    std::cout << str.c_str();
    SetLabel(str);
    sub_definition = definition["gui"][event.GetId() - 1 - wxID_HIGHEST];
    wxPanel* newPanel = new wxPanel(this);
    components = std::vector<Component>();
    int y = UpdatePanel(newPanel);
    wxButton* button = new wxButton(newPanel, wxID_EXECUTE, sub_definition["button"], wxPoint(165 + this->hasChoice * 130, y), wxSize(75, 25));
    newPanel->Show();
    delete mainPanel;
    mainPanel = newPanel;

    Layout();
    Centre();
    Refresh();
}

//put components
int MainFrame::UpdatePanel(wxPanel* panel)
{
    this->hasChoice = 0;
    //file picker
    int y = 10;

    if (sub_definition["components"]==nullptr) {
        sub_definition["components"] = std::vector<nlohmann::json>();
        return y;
    }
    std::vector<nlohmann::json> comp = sub_definition["components"];

    //put components
    std::for_each(comp.begin(), comp.end(), [&](nlohmann::json c) {
        y += Component::PutComponent(&this->components, panel, c, y);
        });
    return y;
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    Destroy();
}