#include "MainFrame.h"

const char* VERSION = "0.1.3";

//Console window for unix
#ifdef __linux__
LogFrame::LogFrame(wxString exepath) : wxFrame(nullptr, wxID_ANY, exepath, \
    wxDefaultPosition, wxSize(600, 400), \
    wxSYSTEM_MENU | \
    wxRESIZE_BORDER | \
    wxMINIMIZE_BOX | \
    wxMAXIMIZE_BOX | \
    wxCAPTION | \
    wxCLIP_CHILDREN) {
    
    logBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    logBox->SetBackgroundColour(*wxBLACK);
    logBox->SetForegroundColour(*wxWHITE);
    wxFont font = logBox->GetFont();
    font.SetPointSize(font.GetPointSize() + 1);
    logBox->SetFont(font);
    logRedirector = new wxStreamToTextRedirector(logBox);
    Centre();
    wxPoint pos = GetPosition();
    SetPosition(wxPoint(pos.x-300, pos.y));
    Show();
}

LogFrame::~LogFrame() {};

void LogFrame::OnClose(wxCloseEvent& event)
{
    Destroy();
}
#endif

//Main window
MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Simple Command Runner")
{
#ifndef _WIN32 
    wxStandardPaths& path = wxStandardPaths::Get();
    path.UseAppInfo(wxStandardPaths::AppInfo_None);
    wxString strExe = path.GetExecutablePath();
    wxSetWorkingDirectory(wxPathOnly(strExe));
#endif
#ifdef __linux__
    logFrame = new LogFrame(strExe);
#endif
    std::cout << "Simple Command Runner v" << VERSION << " by matyalatte" << std::endl;
    
    //get gui definition
    LoadDefinition();
    config = jsonUtils::loadJson("gui_config.json");

    //make menu bar
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* menuFile = new wxMenu;

    if (definition["gui"].size() > 1) {
        for (int i = 0; i < definition["gui"].size(); i++) {
            menuFile->Append(wxID_HIGHEST + i + 1, wxString::FromUTF8(definition["gui"][i]["label"]));
            menuFile->Bind(wxEVT_MENU, &MainFrame::UpdateFrame, this, wxID_HIGHEST + i + 1);
        }
    }

    menuFile->Append(wxID_EXIT, "Quit");
    menuBar->Append(menuFile, "Menu");

    //put help urls to menu bar
    if (definition.contains("help")) {
        wxMenu* menuHelp = new wxMenu;

        for (int i = 0; i < definition["help"].size(); i++) {
            menuHelp->Append(wxID_HIGHEST + i + 1 + definition["gui"].size(), wxString::FromUTF8(definition["help"][i]["label"]));
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
    components = std::vector<Component*>();
    int y = UpdatePanel(mainPanel);
    runButton = new wxButton(mainPanel, wxID_EXECUTE, wxString::FromUTF8(sub_definition["button"]), wxPoint(143, y), wxSize(105, 25));

    Connect(wxID_EXECUTE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::RunCommand));

    mainPanel->Show();

    Layout();
    Centre();
    SetSize(wxSize(405, y + 105));
    SetWindowStyleFlag(wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
}


//read gui_definition.json
void MainFrame::LoadDefinition() {
    std::ifstream istream("gui_definition.json");
    std::string msg;
    definition = jsonUtils::loadJson("gui_definition.json");

    if (definition == nlohmann::json({})) {
        msg = "Fialed to load gui_definition.json (Can't read)";
        std::cout << "[LoadDefinition] " << msg << std::endl;
        ShowErrorDialog(msg);
        sub_definition = jsonUtils::default_definition();
        definition = { { "gui", {sub_definition}} };
        return;
    }

    //check help urls
    if (definition.contains("help")) {
        try {
            jsonUtils::checkHelpURLs(definition);
        }
        catch(std::exception& e) {
            msg = "Fialed to load help URLs (" + std::string(e.what()) + ")";
            std::cout << "[LoadDefinition] " << msg << std::endl;
            definition.erase("help");
        }
    }

    //check format
    if (!definition.contains("gui") || !definition["gui"].is_array()) {
        msg = "Fialed to load gui_definition.json ('gui' array not found.)";
        std::cout << "[LoadDefinition] " << msg << std::endl;
        ShowErrorDialog(msg);
        sub_definition = jsonUtils::default_definition();
        definition["gui"] = { { sub_definition } };
        return;
    }

    //check panel definitions
    for (nlohmann::json& sub_d : definition["gui"]) {
        try {
            jsonUtils::checkSubDefinition(sub_d);
        }
        catch (std::exception& e) {
            msg = "Fialed to load gui_definition.json (" + std::string(e.what()) + ")";
            std::cout << "[LoadDefinition] " << msg << std::endl;
            ShowErrorDialog(msg);
            sub_definition = jsonUtils::default_definition();
            definition["gui"] = {{ sub_definition }};
            return;
        }
    }
    sub_definition = definition["gui"][0];
    
    std::cout << "[LoadDefinition] Loaded gui_definition.json" << std::endl;
}

void MainFrame::UpdateConfig() {
    for (Component* c: components){
        config[c->GetLabel()] = c->GetConfig();
    }
}

void MainFrame::SaveConfig() {
    bool saved = jsonUtils::saveJson(config, "gui_config.json");
    if (saved) {
        std::cout << "[SaveConfig] Saved gui_config.json" << std::endl;
    }
    else {
        std::cout << "[SaveConfig] Failed to write gui_config.json" << std::endl;
    }
}

void MainFrame::ShowErrorDialog(wxString msg) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, "Error", wxICON_ERROR | wxOK | wxCENTRE);
    dialog->ShowModal();
    dialog->Destroy();
}

void MainFrame::ShowSuccessDialog(wxString msg) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, "Success");
    dialog->ShowModal();
    dialog->Destroy();
}

//run command
void MainFrame::RunCommand(wxCommandEvent& event) {
    //save config
    UpdateConfig();
    SaveConfig();

    //make command string
    std::vector<std::string> cmd_ary = sub_definition["command"];
    wxString cmd = "";
    int i = 0;
    for (Component* c : components) {
        if (c->HasString()) {
            if (cmd_ary.size() <= i) {
                std::cout << "[RunCommand]: Json format error (Can not make command)" << std::endl;
                ShowErrorDialog("Json format error (Can not make command)");
                return;
            }
            cmd += cmd_ary[i] + c->GetString();
            i += 1;
        }
    }

    while (cmd_ary.size() > i) {
        cmd += cmd_ary[i];
        i += 1;
    }
    wxString text = runButton->GetLabel();
    runButton->SetLabel("Processing...");
    //run command
    std::cout << "[RunCommand] Command: " << cmd << std::endl;
#ifdef _WIN32
    cmd = "cmd.exe /c " + cmd;
#endif
    std::vector<std::string> msg = exec(cmd);
    runButton->SetLabel(text);
    if (msg[0] == "__null__") {
        std::cout << "[RunCommand] Execution failed. " << std::endl;
        return;
    }

    //show result
    if (msg[1] != "") {//if error
        ShowErrorDialog(msg[1]);
    }
    else {//if success
        if (sub_definition.contains("show_last_line") && sub_definition["show_last_line"]!=0 && msg[0]!="") {
            ShowSuccessDialog(msg[0]);
        }
        else {
            ShowSuccessDialog("Success!");
        }
    }
}

MainFrame::~MainFrame(){
}

void MainFrame::OpenURL(wxCommandEvent& event) {
    wxString url = wxString::FromUTF8(definition["help"][event.GetId() - 1 - wxID_HIGHEST - definition["gui"].size()]["url"]);
    std::cout << "[OpenURL] " << url << std::endl;
    bool res = wxLaunchDefaultBrowser(url);
}

void MainFrame::UpdateFrame(wxCommandEvent& event)
{
    sub_definition = definition["gui"][event.GetId() - 1 - wxID_HIGHEST];

    UpdateConfig();
    
    wxPanel* newPanel = new wxPanel(this);
    int y = UpdatePanel(newPanel);
    wxButton* newRunButton = new wxButton(newPanel, wxID_EXECUTE, wxString::FromUTF8(sub_definition["button"]), wxPoint(143, y), wxSize(105, 25));

    newPanel->Show();
    wxPanel* unused = mainPanel;
    mainPanel = newPanel;
    runButton = newRunButton;
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
    if (sub_definition.contains("window_name")) {
        SetLabel(wxString::FromUTF8(sub_definition["window_name"]));
    }
    else {
        SetLabel("Simple Command Runner");
    }

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
            if (config.contains(newComp->GetLabel())) {
                newComp->SetConfig(config[newComp->GetLabel()]);
            }
            components.push_back(newComp);
        }
    }

    return y;
}

void MainFrame::OnClose(wxCloseEvent& event)
{
#ifdef __linux__
    logFrame->Destroy();
#endif
    Destroy();
}