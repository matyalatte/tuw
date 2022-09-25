#include "main_frame.h"

const char* VERSION = "0.2.0";

#ifdef __linux__
// Console window for linux
LogFrame::LogFrame(wxString exepath) : wxFrame(nullptr, wxID_ANY, exepath,
    wxDefaultPosition, wxSize(600, 400),
    wxSYSTEM_MENU |
    wxRESIZE_BORDER |
    wxMINIMIZE_BOX |
    wxMAXIMIZE_BOX |
    wxCAPTION |
    wxCLIP_CHILDREN) {

    m_log_box = new wxTextCtrl(this, wxID_ANY,
        "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    m_log_box->SetBackgroundColour(*wxBLACK);
    m_log_box->SetForegroundColour(*wxWHITE);
    wxFont font = m_log_box->GetFont();
    font.SetPointSize(font.GetPointSize() + 1);
    m_log_box->SetFont(font);
    m_log_redirector = new wxStreamToTextRedirector(m_log_box);
    Centre();
    wxPoint pos = GetPosition();
    SetPosition(wxPoint(pos.x-300, pos.y));
    Show();
}

void LogFrame::OnClose(wxCloseEvent& event) {
    Destroy();
}
#endif

#ifndef _WIN32
void MainFrame::CalcExePath() {
    wxStandardPaths& path = wxStandardPaths::Get();
    path.UseAppInfo(wxStandardPaths::AppInfo_None);
    m_exe_path = path.GetExecutablePath();
    wxSetWorkingDirectory(wxPathOnly(m_exe_path));
}
#endif

// Main window
MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Simple Command Runner") {
#ifndef _WIN32
    CalcExePath();
#endif
    m_definition = json_utils::LoadJson("gui_definition.json");
    m_config = json_utils::LoadJson("gui_config.json");
    CreateFrame();
}

MainFrame::MainFrame(nlohmann::json definition, nlohmann::json config)
    : wxFrame(nullptr, wxID_ANY, "Simple Command Runner") {
#ifndef _WIN32
    CalcExePath();
#endif
    if (m_definition == nullptr) {
        m_definition = nlohmann::json({});
    }
    this->m_definition = definition;
    this->m_config = config;
    CreateFrame();
}

wxButton* GetRunButton(wxPanel* panel, nlohmann::json sub_definition, int y) {
    std::string button;
    if (sub_definition.contains("button")) {
        button = wxString::FromUTF8(sub_definition["button"]);
    } else {
        button = "Run";
    }
    return new wxButton(panel, wxID_EXECUTE, button, wxPoint(143, y), wxSize(105, 25));
}

void MainFrame::Align(int y) {
    Layout();
    Centre();
#ifdef __APPLE__
    // mac build should have a small window because it doesn't have the menu bar on the window.
    SetSize(wxSize(405, y + 65));
#else
    SetSize(wxSize(405, y + 105));
#endif
}

void MainFrame::CreateFrame() {
#ifdef __linux__
    m_log_frame = new LogFrame(m_exe_path);
#endif
    std::cout << "Simple Command Runner v" << VERSION << " by matyalatte" << std::endl;

    CheckDefinition();

    // make menu bar
    wxMenuBar* menu_bar = new wxMenuBar;
    wxMenu* menu_file = new wxMenu;

    if (m_definition["gui"].size() > 1) {
        for (int i = 0; i < m_definition["gui"].size(); i++) {
            menu_file->Append(wxID_HIGHEST + i + 1,
                wxString::FromUTF8(m_definition["gui"][i]["label"]));
            menu_file->Bind(wxEVT_MENU,
                &MainFrame::UpdateFrame, this, wxID_HIGHEST + i + 1);
        }
    }

    menu_file->Append(wxID_EXIT, "Quit");
    menu_bar->Append(menu_file, "Menu");

    // put help urls to menu bar
    if (m_definition.contains("help")) {
        wxMenu* menuHelp = new wxMenu;

        for (int i = 0; i < m_definition["help"].size(); i++) {
            menuHelp->Append(wxID_HIGHEST + i + 1 + m_definition["gui"].size(),
                wxString::FromUTF8(m_definition["help"][i]["label"]));
            menuHelp->Bind(wxEVT_MENU,
                &MainFrame::OpenURL, this, wxID_HIGHEST + i + 1 + m_definition["gui"].size());
        }
        menu_bar->Append(menuHelp, "Help");
    }
    SetMenuBar(menu_bar);

    // set close event
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Close(true); }, wxID_EXIT);

    // put components
    m_main_panel = new wxPanel(this);
    m_components = std::vector<Component*>();
    int y = UpdatePanel(m_main_panel);
    m_run_button = GetRunButton(m_main_panel, m_sub_definition, y);
    Connect(wxID_EXECUTE, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(MainFrame::ClickButton));

    m_main_panel->Show();

    Align(y);
    SetWindowStyleFlag(wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
}

void MainFrame::JsonLoadFailed(std::string msg) {
    std::cout << "[LoadDefinition] " << msg << std::endl;
    ShowErrorDialog(msg);
    m_sub_definition = json_utils::GetDefaultDefinition();
    m_definition["gui"] = nlohmann::json::array({ m_sub_definition });
}

// read gui_definition.json
void MainFrame::CheckDefinition() {
    std::string msg;

    if (m_definition == nlohmann::json({})) {
        msg = "Fialed to load gui_definition.json (Can't read)";
        m_definition = nlohmann::json({});
        JsonLoadFailed(msg);
        return;
    }

    // check help urls
    if (m_definition.contains("help")) {
        try {
            json_utils::CheckHelpURLs(m_definition);
        }
        catch(std::exception& e) {
            msg = "Fialed to load help URLs (" + std::string(e.what()) + ")";
            std::cout << "[LoadDefinition] " << msg << std::endl;
            m_definition.erase("help");
        }
    }

    // check panel definitions
    try {
        json_utils::CheckDefinition(m_definition);
    }
    catch (std::exception& e) {
        msg = "Fialed to load gui_definition.json (" + std::string(e.what()) + ")";
        JsonLoadFailed(msg);
        return;
    }

    m_sub_definition = m_definition["gui"][0];

    std::cout << "[LoadDefinition] Loaded gui_definition.json" << std::endl;
}

void MainFrame::UpdateConfig() {
    for (Component *c : m_components) {
        nlohmann::json comp_config = c->GetConfig();
        if (comp_config.is_null()) continue;
        m_config[c->GetLabel()] = comp_config;
    }
}

void MainFrame::SaveConfig() {
    UpdateConfig();
    bool saved = json_utils::SaveJson(m_config, "gui_config.json");
    if (saved) {
        std::cout << "[SaveConfig] Saved gui_config.json" << std::endl;
    } else {
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

std::array<std::string, 2> MainFrame::RunCommand() {
    // make command string
    std::vector<std::string> cmd_ary = m_sub_definition["command"];
    wxString cmd = "";
    int i = 0;
    for (Component* c :  m_components) {
        if (c->HasString()) {
            if (cmd_ary.size() <= i) {
                std::cout << "[RunCommand]: Json format error (Can not make command)" << std::endl;
                return {"", "Json format error(Can not make command)"};
            }
            cmd += cmd_ary[i] + c->GetString();
            i += 1;
        }
    }

    while (cmd_ary.size() > i) {
        cmd += cmd_ary[i];
        i += 1;
    }

    wxString text = m_run_button->GetLabel();
    m_run_button->SetLabel("Processing...");
    // run command
    std::cout << "[RunCommand] Command: " << cmd << std::endl;
#ifdef _WIN32
    cmd = "cmd.exe /c " + cmd;
#endif
    std::array<std::string, 2> msg = Exec(cmd);
    m_run_button->SetLabel(text);
    return msg;
}

// run command
void MainFrame::ClickButton(wxCommandEvent& event) {
    // save config
    SaveConfig();

    std::array<std::string, 2> msg = RunCommand();

    // show result
    if (msg[1] != "") {  // if error
        std::cout << "[RunCommand] Failed to execute commands." << std::endl;
        ShowErrorDialog(msg[1]);
    } else {  // if success
        if (m_sub_definition.contains("show_last_line") &&
            m_sub_definition["show_last_line"] != 0 && msg[0] != "") {
            ShowSuccessDialog(msg[0]);
        } else {
            ShowSuccessDialog("Success!");
        }
    }
}

void MainFrame::OpenURL(wxCommandEvent& event) {
    wxString url = wxString::FromUTF8(m_definition["help"][event.GetId()
        - 1 - wxID_HIGHEST - m_definition["gui"].size()]["url"]);
    std::cout << "[OpenURL] " << url << std::endl;
    bool success = wxLaunchDefaultBrowser(url);
    if (!success) {
        std::cout << "[OpenURL] Failed to open URL by an unexpected error." << std::endl;
    }
}

void MainFrame::UpdateFrame(wxCommandEvent& event) {
    m_sub_definition = m_definition["gui"][event.GetId() - 1 - wxID_HIGHEST];

    UpdateConfig();

    wxPanel* new_panel = new wxPanel(this);
    int y = UpdatePanel(new_panel);
    wxButton* new_run_button = GetRunButton(new_panel, m_sub_definition, y);
    new_panel->Show();
    wxPanel* unused = m_main_panel;
    m_main_panel = new_panel;
    m_run_button = new_run_button;
    unused->Destroy();

    Align(y);
    Refresh();
}

// put components
int MainFrame::UpdatePanel(wxPanel* panel) {
    std::string str = "Simple Command Runner";
    str = m_sub_definition["label"];
    std::cout << "[UpdatePanel] " << str.c_str() << std::endl;
    if (m_sub_definition.contains("window_name")) {
        SetLabel(wxString::FromUTF8(m_sub_definition["window_name"]));
    } else {
        SetLabel("Simple Command Runner");
    }

    int y = 10;
    if (m_sub_definition["components"].size() == 0) {
        m_sub_definition["components"] = std::vector<nlohmann::json>();
        return y;
    }
    std::vector<nlohmann::json> comp = m_sub_definition["components"];
    m_components.clear();
    m_components.shrink_to_fit();
    Component* new_comp = nullptr;

    // put components
    for (nlohmann::json c : comp) {
        new_comp = Component::PutComponent(panel, c, y);
        if (new_comp != nullptr) {
            y += new_comp->GetHeight();
            if (m_config.contains(new_comp->GetLabel())) {
                new_comp->SetConfig(m_config[new_comp->GetLabel()]);
            }
            m_components.push_back(new_comp);
        }
    }

    return y;
}

void MainFrame::OnClose(wxCloseEvent& event) {
#ifdef __linux__
    m_log_frame->Destroy();
#endif
    Destroy();
}

nlohmann::json MainFrame::GetDefinition() {
    return m_definition;
}
