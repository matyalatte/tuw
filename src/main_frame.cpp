#include "main_frame.h"

const char* VERSION = "0.2.1";

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
    this->m_definition = definition;
    this->m_config = config;
    CreateFrame();
}

void MainFrame::CreateFrame() {
#ifdef __linux__
    m_log_frame = new LogFrame(m_exe_path);
    m_ostream = m_log_frame;
#else
    m_ostream = &std::cout;
#endif
    *m_ostream << "Simple Command Runner v" << VERSION << " by matyalatte" << std::endl;

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
        wxMenu* menu_help = new wxMenu;

        for (int i = 0; i < m_definition["help"].size(); i++) {
            menu_help->Append(wxID_HIGHEST + i + 1 + m_definition["gui"].size(),
                wxString::FromUTF8(m_definition["help"][i]["label"]));
            menu_help->Bind(wxEVT_MENU,
                &MainFrame::OpenURL, this, wxID_HIGHEST + i + 1 + m_definition["gui"].size());
        }
        menu_bar->Append(menu_help, "Help");
    }
    SetMenuBar(menu_bar);

    // set events
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Close(true); }, wxID_EXIT);
    Connect(wxID_EXECUTE, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(MainFrame::ClickButton));

    // put components
    m_components = std::vector<Component*>();
    UpdatePanel();
    Fit();

#ifdef __linux__
    // Idk why, but the sizer will ignore the last component on Ubuntu
    int button_height;
    m_run_button->GetSize(nullptr, &button_height);
    SetSize(GetSize() + wxSize(0, button_height));
#endif

    SetWindowStyleFlag(wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
}

void MainFrame::JsonLoadFailed(std::string msg) {
    *m_ostream << "[LoadDefinition] " << msg << std::endl;
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
            *m_ostream << "[LoadDefinition] " << msg << std::endl;
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

    *m_ostream << "[LoadDefinition] Loaded gui_definition.json" << std::endl;
}

void MainFrame::UpdateConfig() {
    for (Component *c : m_components) {
        nlohmann::json comp_config = c->GetConfig();
        if (comp_config.is_null()) continue;
        m_config[c->GetID()] = comp_config;
    }
}

void MainFrame::SaveConfig() {
    UpdateConfig();
    bool saved = json_utils::SaveJson(m_config, "gui_config.json");
    if (saved) {
        *m_ostream << "[SaveConfig] Saved gui_config.json" << std::endl;
    } else {
        *m_ostream << "[SaveConfig] Failed to write gui_config.json" << std::endl;
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

// Make command string
wxString MainFrame::GetCommand() {
    std::vector<std::string> cmd_ary = m_sub_definition["command"];
    std::vector<std::string> cmd_ids = m_sub_definition["command_ids"];
    std::vector<std::string> comp_ids = m_sub_definition["component_ids"];

    std::vector<wxString> comp_strings = std::vector<wxString>(m_components.size());
    for (int i = 0; i < m_components.size(); i++) {
        comp_strings[i] = m_components[i]->GetString();
    }

    wxString cmd = wxString::FromUTF8(cmd_ary[0]);
    std::string id;
    int comp_size = comp_ids.size();
    int j;
    int non_id_comp = 0;
    for (int i = 0; i < cmd_ids.size(); i++) {
        id = cmd_ids[i];
        if (id == ""){
            j = comp_size;
        }
        else {
            for (j = 0; j < comp_size; j++) {
                if (id == comp_ids[j]) {
                    break;
                }
            }
        }
        if (j >= comp_size) {
            while ((!m_components[non_id_comp]->HasString() || comp_ids[non_id_comp] != "") && non_id_comp < comp_size) {
                non_id_comp++;
            }
            j = non_id_comp;
            non_id_comp++;
        }
        if (j < comp_size) {
            cmd += comp_strings[j];
        }
        if (i + 1 < cmd_ary.size()) {
            cmd += wxString::FromUTF8(cmd_ary[i + 1]);
        }
    }
    return cmd;
}

std::array<std::string, 2> MainFrame::RunCommand() {
    wxString cmd = GetCommand();
    wxString text = m_run_button->GetLabel();
    m_run_button->SetLabel("Processing...");

    *m_ostream << "[RunCommand] Command: " << cmd << std::endl;
#ifdef _WIN32
    cmd = "cmd.exe /c " + cmd;
#endif
    std::array<std::string, 2> msg = Exec(*m_ostream, cmd);
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
        *m_ostream << "[RunCommand] Failed to execute commands." << std::endl;
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
    *m_ostream << "[OpenURL] " << url << std::endl;
    bool success = wxLaunchDefaultBrowser(url);
    if (!success) {
        *m_ostream << "[OpenURL] Failed to open URL by an unexpected error." << std::endl;
    }
}

void MainFrame::UpdateFrame(wxCommandEvent& event) {
    m_sub_definition = m_definition["gui"][event.GetId() - 1 - wxID_HIGHEST];

    UpdateConfig();

    wxPanel* unused_panel = m_panel;
    UpdatePanel();

    unused_panel->Destroy();
    Fit();

    Refresh();
}

// put components
void MainFrame::UpdatePanel() {
    std::string str = "Simple Command Runner";
    str = m_sub_definition["label"];
    *m_ostream << "[UpdatePanel] " << str.c_str() << std::endl;
    if (m_sub_definition.contains("window_name")) {
        SetLabel(wxString::FromUTF8(m_sub_definition["window_name"]));
    } else {
        SetLabel("Simple Command Runner");
    }

    if (m_sub_definition["components"].size() == 0) {
        m_sub_definition["components"] = std::vector<nlohmann::json>();
    }
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* comp_sizer = new wxBoxSizer(wxVERTICAL);
    comp_sizer->SetMinSize(wxSize(200, 25));
    m_panel = new wxPanel(this);

    std::vector<nlohmann::json> comp = m_sub_definition["components"];
    m_components.clear();
    m_components.shrink_to_fit();
    Component* new_comp = nullptr;

    // put components
    for (nlohmann::json c : comp) {
        new_comp = Component::PutComponent(m_panel, comp_sizer, c);
        if (new_comp != nullptr) {
            std::string const id = new_comp->GetID();
            if (m_config.contains(id)) {
                new_comp->SetConfig(m_config[id]);
            }
            m_components.push_back(new_comp);
        }
    }

    // put a button
    wxString button;
    if (m_sub_definition.contains("button")) {
        button = wxString::FromUTF8(m_sub_definition["button"]);
    } else {
        button = "Run";
    }
    m_run_button = new wxButton(m_panel, wxID_EXECUTE, button);
    comp_sizer->Add(m_run_button, 0, wxFIXED_MINSIZE | wxALIGN_CENTER);

    main_sizer->Add(comp_sizer, 0, wxALIGN_CENTER | wxALL, 15);
    m_panel->SetSizerAndFit(main_sizer);
    m_panel->Show();
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
