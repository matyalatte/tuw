#include "MainFrame.h"

const char* VERSION = "0.1.0";

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
    LoadConfig();

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
    if (hasKey(definition, "help")) {
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
    components = std::vector<Component>();
    int y = UpdatePanel(mainPanel);
    runButton = new wxButton(mainPanel, wxID_EXECUTE, wxString::FromUTF8(sub_definition["button"]), wxPoint(143, y), wxSize(105, 25));

    Connect(wxID_EXECUTE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::RunCommand));

    mainPanel->Show();

    Layout();
    Centre();
    SetSize(wxSize(405, y + 105));
    SetWindowStyleFlag(wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
}

//get default definition of gui
nlohmann::json default_definition() {
    nlohmann::json def =
    {
            {"label", "Default GUI"},
#ifdef _WIN32
            {"command", "dir" },
            {"button", "run 'dir'"},
#else
            {"command", "ls" },
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
    //check if keys exist
    std::vector<std::string> keys = { "label", "button", "command", "components" };
    for (std::string key : keys) {
        if (!hasKey(sub_definition, key)) {
            return "'" + key + "' not found.";
        }
    }

    //check is_string
    keys = { "label", "button", "command"};
    if (hasKey(sub_definition, "window_name")) {
        keys.push_back("window_name");
    }
    for (std::string key : keys) {
        if (!sub_definition[key].is_string()) {
            return "'" + key + "' should be a string.";
        }
    }

    //check is_boolean
    if (hasKey(sub_definition, "show_last_line") && !sub_definition["show_last_line"].is_boolean()) {
        return "'show_last_line' should be a boolean.";
    }
    
    //check is_array
    keys = { "components" };
    for (std::string key : keys) {
        if (!sub_definition[key].is_array()) {
            return "'" + key + "' should be an array.";
        }
    }

    //check components
    keys = { "type", "label" };
    std::vector<std::string> subkeys = {};
    std::string label;
    for (nlohmann::json c : sub_definition["components"]) {
        //check if type and label exist
        for (std::string key : keys) {
            if (!hasKey(c, key)) {
                return "components['" + key + "'] not found.";
            }
            if (!c[key].is_string()) {
                return "components['" + key + "'] should be a string.";
            }
        }
        label = c["label"];
        if (c["type"]=="file"){
            if (hasKey(c, "extention") && !c["extension"].is_string()) {
                return label + "['extention'] should be a string.";
            }
        }
        else if(c["type"] == "choice") {
            subkeys = { "items", "values" };
            for (std::string key : subkeys) {
                if (hasKey(c, key) && !c[key].is_array()) {
                    return label + "['" + key +"'] should be an array.";
                }
            }
            subkeys = { "width", "default" };
            for (std::string key : subkeys) {
                if (hasKey(c, key) && !c[key].is_number()) {
                    return label + "['" + key + "'] should be an int.";
                }
            }
        }
        else if (c["type"] == "check") {
            if (hasKey(c, "value") && !c["value"].is_string()) {
                return label + "['value'] should be a string.";
            }
        }
        else if (c["type"] == "checks") {
            if (!hasKey(c, "items")) {
                return label + "['items'] not found.";
            }
            if (!c["items"].is_array()) {
                return label + "['items'] should be an array.";
            }
            if (hasKey(c, "values")) {
                if (!c["values"].is_array()) {
                    return label + "['values'] should be an array.";
                }
                if (c["values"].size()!=c["items"].size()) {
                    return label + "['values'] and " + label + "['items'] should have the same size.";
                }
            }
        }
        if (hasKey(c, "add_quotes") && !c["add_quotes"].is_boolean()) {
            return label + "['add_quotes'] should be a boolean.";
        }
    }
    return "__null__";
}

std::string checkHelpURLs(nlohmann::json definition) {
    if (!definition["help"].is_array()) {
        return "'help' should be an array.";
    }
    std::vector<std::string> keys = { "type", "label", "url" };
    for (nlohmann::json h : definition["help"]) {
        for (std::string key : keys) {
            if (!hasKey(h, key)) {
                return "'" + key + "' not found.";
            }
            if (!h[key].is_string()) {
                return "'" + key + "' should be a string.";
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
        msg = "Fialed to load gui_definition.json (Not found)";
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
        msg = "Fialed to load gui_definition.json (Can not read)";
        std::cout << "[LoadDefinition] " << msg << std::endl;
        ShowErrorDialog(msg);
        sub_definition = default_definition();
        definition = { { "gui", {sub_definition}} };
        return;
    }
    

    //check format
    if (hasKey(definition, "gui") && definition["gui"].is_array()) {
        sub_definition = definition["gui"][0];
    }
    else {
        msg = "Fialed to load gui_definition.json ('gui' array not found.)";
        std::cout << "[LoadDefinition] " << msg << std::endl;
        ShowErrorDialog(msg);
        sub_definition = default_definition();
        definition = { { "gui", {sub_definition}} };
        return;
    }

    //check help urls
    if (hasKey(definition, "help")) {
        msg = checkHelpURLs(definition);
        if (msg != "__null__") {
            msg = "Fialed to load help URLs (" + msg + ")";
            std::cout << "[LoadDefinition] " << msg << std::endl;
            ShowErrorDialog(msg);
            definition.erase("help");
            return;
        }
    }


    //check panel definitions
    msg = checkSubDefinition(sub_definition);
    if (msg!="__null__") {
        msg = "Fialed to load gui_definition.json ("+ msg +")";
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



std::vector<std::string> split(const std::string& s, const char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    bool store = true;
    while (std::getline(tokenStream, token, delimiter))
    {
        if (store) {
            tokens.push_back(token);
            store = false;
        }
        else {
            store = true;
        }
    }
    return tokens;
}

//run command
void MainFrame::RunCommand(wxCommandEvent& event) {
    //save config
    SaveConfig();

    //make command string
    std::vector<std::string> cmd_ary = split(sub_definition["command"], '%');
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
    wxString text = runButton->GetLabel();
    runButton->SetLabel("Processing...");
    //run command
    std::cout << "[RunCommand] Command: " << cmd << std::endl;
#ifdef _WIN32
    cmd = "cmd.exe /c " + cmd;
#endif
    std::vector<wxString> msg = exec(cmd);
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
    wxString url = wxString::FromUTF8(definition["help"][event.GetId() - 1 - wxID_HIGHEST - definition["gui"].size()]["url"]);
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
    if (hasKey(sub_definition, "window_name")) {
        SetLabel(wxString::FromUTF8(sub_definition["window_name"]));
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
#ifdef __linux__
    logFrame->Destroy();
#endif
    Destroy();
}