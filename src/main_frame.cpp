#include "main_frame.h"

// Main window
MainFrame::MainFrame(nlohmann::json definition, nlohmann::json config)
    : wxFrame(nullptr, wxID_ANY, scr_constants::TOOL_NAME) {
    SetUp();
    if (definition.empty()) {
        definition = LoadJson("gui_definition.json", true);
    }
    if (config.empty()) {
        config = LoadJson("gui_config.json", false);
    }
    CheckDefinition(definition);
    this->m_definition = definition;
    this->m_sub_definition = definition["gui"][0];
    this->m_config = config;
    CreateFrame();
}

void MainFrame::SetUp() {
    // Use the executable directory as the working dir.
    wxStandardPaths& path = wxStandardPaths::Get();
    path.UseAppInfo(wxStandardPaths::AppInfo_None);
    wxString exe_path = path.GetExecutablePath();
    wxSetWorkingDirectory(wxPathOnly(exe_path));

#ifdef __linux__
    m_log_frame = new LogFrame(exe_path);
    m_ostream = m_log_frame;
#else
    m_ostream = &std::cout;
#endif
    *m_ostream << scr_constants::TOOL_NAME << " v" << scr_constants::VERSION;
    *m_ostream << " by " << scr_constants::AUTHOR << std::endl;
}

nlohmann::json MainFrame::LoadJson(const std::string& file, bool is_definition) {
    nlohmann::json json = nlohmann::json({});
    try {
        json = json_utils::LoadJson(file);
    }
    catch (nlohmann::json::exception& e) {
        if (is_definition) JsonLoadFailed(e.what(), json);
    }
    catch (std::exception& e) {
        if (is_definition) JsonLoadFailed(e.what(), json);
    }
    return json;
}

void MainFrame::CreateFrame() {
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

void MainFrame::JsonLoadFailed(const std::string& msg, nlohmann::json& definition) {
    wxString wxmsg = wxString::FromUTF8(msg);
    *m_ostream << "[LoadDefinition] Error: " << wxmsg << std::endl;
    ShowErrorDialog(wxmsg);
    nlohmann::json sub_definition = json_utils::GetDefaultDefinition();
    definition["gui"] = nlohmann::json::array({ sub_definition });
}

// read gui_definition.json
void MainFrame::CheckDefinition(nlohmann::json& definition) {
    std::string msg;

    // Check tool version
    try {
        json_utils::CheckVersion(definition);
        std::string key = "recommended";
        if (definition.contains(key)) {
            std::string version = definition[key];
            if (definition["not_" + key]) {
                msg = "Version " + version + " is " + key + ".";
                *m_ostream << "[LoadDefinition] Warning: " << msg << std::endl;
            }
        }
    }
    catch(std::exception& e) {
        JsonLoadFailed(std::string(e.what()), definition);
        return;
    }

    // check help urls
    if (definition.contains("help")) {
        try {
            json_utils::CheckHelpURLs(definition);
        }
        catch(std::exception& e) {
            msg = "Fialed to load help URLs (" + std::string(e.what()) + ")";
            wxString wxmsg = wxString::FromUTF8(msg);
            *m_ostream << "[LoadDefinition] Error: " << wxmsg << std::endl;
            ShowErrorDialog(wxmsg);
            definition.erase("help");
        }
    }

    // check panel definitions
    try {
        json_utils::CheckDefinition(definition);
    }
    catch (std::exception& e) {
        msg = "Fialed to load gui_definition.json (" + std::string(e.what()) + ")";
        JsonLoadFailed(msg, definition);
        return;
    }

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
        *m_ostream << "[SaveConfig] Error: Failed to write gui_config.json" << std::endl;
    }
}

void MainFrame::ShowErrorDialog(const wxString& msg) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, "Error", wxICON_ERROR | wxOK | wxCENTRE);
    dialog->ShowModal();
    dialog->Destroy();
}

void MainFrame::ShowSuccessDialog(const wxString& msg) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, "Success");
    dialog->ShowModal();
    dialog->Destroy();
}

constexpr char* CMD_ID_PERCENT = "_";
constexpr char* CMD_ID_CURRENT_DIR = "__CWD__";

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
    int comp_size = comp_ids.size();
    int j;
    int non_id_comp = 0;
    for (int i = 0; i < cmd_ids.size(); i++) {
        std::string id = cmd_ids[i];
        j = -1;
        if (id == "") {
            j = comp_size;
        } else if (id == CMD_ID_PERCENT) {
            cmd += "%";
        } else if (id == CMD_ID_CURRENT_DIR) {
            cmd += wxGetCwd();
        } else {
            for (j = 0; j < comp_size; j++) {
                if (id == comp_ids[j]) {
                    break;
                }
            }
        }
        if (j >= comp_size) {
            while (non_id_comp < comp_size &&
                  (!m_components[non_id_comp]->HasString() || comp_ids[non_id_comp] != "")) {
                non_id_comp++;
            }
            j = non_id_comp;
            if (non_id_comp >= comp_size) {
                *m_ostream << "[RunCommand] Warning: "
                           << "The command requires more components for arguments."
                           << std::endl;
            }
            non_id_comp++;
        }
        if (j >= 0 && j < comp_size) {
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
    size_t id = event.GetId() - 1 - wxID_HIGHEST - m_definition["gui"].size();
    nlohmann::json help = m_definition["help"][id];
    std::string type = help["type"];
    wxString url;
    std::string tag;
    if (type == "url") {
        url = wxString::FromUTF8(help["url"]);
        tag = "[OpenURL] ";
    } else if (type == "file") {
        url = wxString::FromUTF8(help["path"]);
        tag = "[OpenFile] ";
    } else {
        ShowErrorDialog("Unsupported help type: " + type);
        return;
    }
    *m_ostream << tag << url << std::endl;
    if (type == "file") {
        url = "file:" + url;
    }
    bool success = wxLaunchDefaultBrowser(url);
    if (!success) {
        *m_ostream << tag << "Failed to open " << type << " by an unexpected error." << std::endl;
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

void MainFrame::UpdatePanel() {
    wxString label = wxString::FromUTF8(m_sub_definition["label"]);
    *m_ostream << "[UpdatePanel] " << label << std::endl;
    wxString window_name = wxString::FromUTF8(
        m_sub_definition.value("window_name", "Simple Command Runner"));
    SetLabel(window_name);

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* comp_sizer = new wxBoxSizer(wxVERTICAL);
    comp_sizer->SetMinSize(wxSize(200, 25));
    m_panel = new wxPanel(this);

    // put components
    std::vector<nlohmann::json> comp = m_sub_definition["components"];
    m_components.clear();
    m_components.shrink_to_fit();
    Component* new_comp = nullptr;
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
    wxString button = wxString::FromUTF8(m_sub_definition.value("button", "Run"));
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
