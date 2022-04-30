#pragma once
#include "MainFrame.h"
#include "Exec.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <array>

const char* VERSION = "0.0.3";

//Main window
MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Simple Command Runner")
{
    std::cout << "Simple Command Runner v" << VERSION << " by matyalatte" << std::endl;
    
    //get gui definition
    LoadDefinition();
    LoadConfig();

    //make menu bar
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* menuFile = new wxMenu;
    
    if (definition["gui"].size() > 1) {
        for (int i = 0; i < definition["gui"].size(); i++) {
            menuFile->Append(wxID_HIGHEST + i + 1, definition["gui"][i]["label"]);
            menuFile->Bind(wxEVT_MENU, &MainFrame::UpdateFrame, this, wxID_HIGHEST + i + 1);
        }
    }
    menuFile->Append(wxID_EXIT, "Quit");
    menuBar->Append(menuFile, "Menu");
    
    //put help urls to menu bar
    if (hasKey(definition, "help")) {
        wxMenu* menuHelp = new wxMenu;

        for (int i = 0; i < definition["help"].size(); i++) {
            menuHelp->Append(wxID_HIGHEST + i + 1 + definition["gui"].size(), definition["help"][i]["label"]);
            menuHelp->Bind(wxEVT_MENU, &MainFrame::OpenURL, this, wxID_HIGHEST + i + 1 + definition["gui"].size());
        }
        menuBar->Append(menuHelp, "Help");
    }
    SetMenuBar(menuBar);

    //set close event
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Close(true); }, wxID_EXIT);
    
    //put components
    mainPanel = new wxPanel(this);
    components = std::vector<Component>();
    int y = UpdatePanel(mainPanel);

    //run button
    wxButton* button = new wxButton(mainPanel, wxID_EXECUTE, sub_definition["button"], wxPoint(143, y), wxSize(105, 25));
    Connect(wxID_EXECUTE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::RunCommand));
    
    mainPanel->Show();

    Layout();
    Centre();
    SetSize(wxSize(405, y+105));
    SetWindowStyleFlag(wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
}

//get default definition of gui
nlohmann::json default_definition() {
    nlohmann::json def =
        {
#ifdef _WIN32
            {"label", "Default GUI"},
            {"command", {"dir"} },
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


std::string checkSubDefinition(nlohmann::json sub_definition) {
    std::vector<std::string> keys = { "label", "button", "command", "components" };
    for (std::string key : keys) {
        if (!hasKey(sub_definition, key)) {
            return "'" + key + "' not found";
        }
    }
    keys = {"type", "label"};
    for (nlohmann::json c : sub_definition["components"]) {
        for (std::string key : keys) {
            if (!hasKey(c, key)) {
                return "components['" + key + "'] not found";
            }
        }
    }
    return "__null__";
}

//read gui_definition.json
void MainFrame::LoadDefinition() {
    std::ifstream istream("gui_definition.json");
    std::string msg;

    if (!istream) {
        msg = "Fialed to load gui_definition.json";
        std::cout << "[LoadDefinition] " << msg << std::endl;
        ShowErrorDialog(msg);
        sub_definition = default_definition();
        definition = { { "gui", {sub_definition}} };
        return;
    }

    //read json file
    try {
        istream >> definition;
        istream.close();
    }
    catch (...) {
        msg = "Fialed to load gui_definition.json";
        std::cout << "[LoadDefinition] " << msg << std::endl;
        ShowErrorDialog(msg);
        sub_definition = default_definition();
        definition = { { "gui", {sub_definition}} };
        return;
    }
    

    //check format
    if (hasKey(definition, "gui")) {
        sub_definition = definition["gui"][0];
    }
    else {
        msg = "Json format error('gui' not found)";
        std::cout << "[LoadDefinition] " << msg << std::endl;
        ShowErrorDialog(msg);
        sub_definition = default_definition();
        return;
    }

    msg = checkSubDefinition(sub_definition);
    if (msg!="__null__") {
        msg = "Json format error("+ msg +")";
        std::cout << "[LoadDefinition] " << msg << std::endl;
        ShowErrorDialog(msg);
        sub_definition = default_definition();
        return;
    }
    std::cout << "[LoadDefinition] Loaded gui_definition.json" << std::endl;
}



void MainFrame::UpdateConfig() {
    for (Component c: components){
        config[c.GetLabel()] = c.GetConfig();
    }
}
void MainFrame::LoadConfig() {
    std::ifstream istream("gui_config.json");
    if (!istream) {
        std::cout << "[LoadConfig] Fialed to load gui_config.json" << std::endl;
        config = {};
    }
    else {
        try {
            istream >> config;
            istream.close();
            std::cout << "[LoadConfig] Loaded gui_config.json" << std::endl;
        }
        catch(...) {
            std::cout << "[LoadConfig] Fialed to load gui_config.json" << std::endl;
            config = {};
        }
    }

}

void MainFrame::SaveConfig() {
    UpdateConfig();
    std::ofstream ostream("gui_config.json");

    if (!ostream) {
        std::cout << "[SaveConfig] Fialed to write gui_config.json" << std::endl;
    }
    else {
        std::cout << "[SaveConfig] Saved gui_config.json" << std::endl;
        ostream << std::setw(4) << config << std::endl;
        ostream.close();
    }
}

void MainFrame::ShowErrorDialog(std::string msg) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, "Error", wxICON_ERROR | wxOK | wxCENTRE);
    dialog->ShowModal();
    dialog->Destroy();
}

void MainFrame::ShowSuccessDialog(std::string msg) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, "Success");
    dialog->ShowModal();
    dialog->Destroy();
}

//run command
void MainFrame::RunCommand(wxCommandEvent& event) {
    //save config
    SaveConfig();


    //make command string
    std::vector<std::string> cmd_ary = sub_definition["command"];

    wxString cmd = "";
    int i = 0;
    for (Component c : components) {
        if (c.HasString()) {
            if (cmd_ary.size() <= i) {
                std::cout << "[RunCommand]: Json format error (Can not make command)" << std::endl;
                ShowErrorDialog("Json format error (Can not make command)");
                return;
            }
            cmd += cmd_ary[i] + c.GetString();
            i += 1;
        }
    }

    while (cmd_ary.size() > i) {
        cmd += cmd_ary[i];
        i += 1;
    }

    //run command
    std::cout << "[RunCommand] Command: " << cmd << std::endl;
#ifdef _WIN32
    cmd = "cmd.exe /c " + cmd;
#endif
    std::vector<std::string> msg = exec(cmd);

    if (msg[0] == "__null__") {
        std::cout << "[RunCommand] Execution failed. " << std::endl;
        return;
    }

    //show result
    if (msg[1] != "") {//if error
        ShowErrorDialog(msg[1]);
    }
    else {//if success
        if (hasKey(sub_definition, "show_last_line") && sub_definition["show_last_line"]!=0 && msg[0]!="") {
            ShowSuccessDialog(msg[0]);
        }
        else {
            ShowSuccessDialog("Success!");
        }
    }
}

MainFrame::~MainFrame(){}

void MainFrame::OpenURL(wxCommandEvent& event) {
    std::string url = definition["help"][event.GetId() - 1 - wxID_HIGHEST - definition["gui"].size()]["url"];
    std::cout << "[OpenURL] " << url << std::endl;
    bool res = wxLaunchDefaultBrowser(url);
}

void MainFrame::UpdateFrame(wxCommandEvent& event)
{
    sub_definition = definition["gui"][event.GetId() - 1 - wxID_HIGHEST];
    std::string msg = checkSubDefinition(sub_definition);
    if (msg != "__null__") {
        msg = "Json format error(" + msg + ")";
        std::cout << "[UpdateFrame] " << msg << std::endl;
        ShowErrorDialog(msg);
        sub_definition = default_definition();
        return;
    }

    UpdateConfig();
    
    wxPanel* newPanel = new wxPanel(this);
    int y = UpdatePanel(newPanel);
    wxButton* button = new wxButton(newPanel, wxID_EXECUTE, sub_definition["button"], wxPoint(158, y), wxSize(75, 25));
    newPanel->Show();
    wxPanel* unused = mainPanel;
    mainPanel = newPanel;
    unused->Destroy();

    Layout();
    SetSize(wxSize(405, y + 105));
    Refresh();
}

//put components
int MainFrame::UpdatePanel(wxPanel* panel)
{
    std::string str = "Simple Command Runner";
    str = sub_definition["label"];
    std::cout << "[UpdatePanel] " << str.c_str() << std::endl;
    if (hasKey(sub_definition, "window_name")) {
        SetLabel(sub_definition["window_name"]);
    }
    else {
        SetLabel("Simple Command Runner");
    }

    //file picker
    int y = 10;
    if (sub_definition["components"].is_null()) {
        sub_definition["components"] = std::vector<nlohmann::json>();
        return y;
    }
    std::vector<nlohmann::json> comp = sub_definition["components"];
    components.clear();
    components.shrink_to_fit();
    Component* newComp = nullptr;

    //put components
    for (nlohmann::json c : comp) {
        newComp = Component::PutComponent(panel, c, y);
        if (newComp != nullptr) {
            y += newComp->GetHeight();
            if (hasKey(config, newComp->GetLabel())) {
                newComp->SetConfig(config[newComp->GetLabel()]);
            }
            components.push_back(*newComp);
        }
    }
    return y;
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    Destroy();
}