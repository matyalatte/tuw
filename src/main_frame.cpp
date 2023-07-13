#include "main_frame.h"

#ifdef __linux__
#define PRINT_FMT(fmt, ...) *m_ostream << wxString::Format(fmt, __VA_ARGS__)
#define PRINT(str) *m_ostream << str
#else
#define PRINT_FMT(fmt, ...) wxPrintf(fmt, __VA_ARGS__)
#define PRINT(str) wxPrintf(str)
#endif

// Main window
MainFrame::MainFrame(const rapidjson::Document& definition, const rapidjson::Document& config)
    : wxFrame(nullptr, wxID_ANY, scr_constants::TOOL_NAME,
              wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX) {
    // Use the executable directory as the working dir.
    wxString exe_path = stdpath::GetExecutablePath(wxTheApp->argv[0]);
    wxSetWorkingDirectory(wxPathOnly(exe_path));

#ifdef __linux__
    // Make console window for Linux
    m_log_frame = new LogFrame(exe_path);
    m_ostream = m_log_frame;
#endif

    PRINT_FMT("%s v%s by %s\n", scr_constants::TOOL_NAME,
              scr_constants::VERSION, scr_constants::AUTHOR);

    m_definition.CopyFrom(definition, m_definition.GetAllocator());
    m_config.CopyFrom(config, m_config.GetAllocator());

    if (!m_definition.IsObject() || m_definition.ObjectEmpty()) {
        bool exists_external_json = wxFileExists("gui_definition.json");
        ExeContainer exe;
        wxResult result = exe.Read(exe_path);
        if (result.ok) {
            if (exe.HasJson()) {
                PRINT("[LoadDefinition] Found JSON in the executable.\n");
                exe.GetJson(m_definition);
                if (exists_external_json) {
                    wxString msg =
                        "WARNING: Using embedded JSON. gui_definition.json was ignored.\n";
                    PRINT_FMT("[LoadDefinition] %s", msg);
                    ShowSuccessDialog(msg, "Warning");
                }
            } else {
                PRINT("[LoadDefinition] Embedded JSON not found.\n");
                result = { false };
            }
        } else {
            PRINT_FMT("[LoadDefinition] ERROR: %s\n", result.msg);
        }

        if (!result.ok) {
            if (exists_external_json) {
                PRINT("[LoadDefinition] Loaded gui_definition.json\n");
                LoadJson("gui_definition.json", m_definition, true);
            } else {
                JsonLoadFailed("gui_definition.json not found.", m_definition);
            }
        }
    }

    if (!config.IsObject() || config.ObjectEmpty()) {
        LoadJson("gui_config.json", m_config, false);
    }

    CheckDefinition(m_definition);
    m_definition_id = 0;
    if (m_config.HasMember("_mode") && m_config["_mode"].IsInt()) {
        int mode = m_config["_mode"].GetInt();
        if (mode < m_definition["gui"].Size())
            m_definition_id = mode;
    }
    CreateFrame();
}

void MainFrame::LoadJson(const std::string& file, rapidjson::Document& json, bool is_definition) {
    json_utils::JsonResult result = json_utils::LoadJson(file, json);

    if (!result.ok) {
        json.SetObject();
        if (is_definition)
            JsonLoadFailed(wxString::FromUTF8(result.msg.c_str()), json);
    }
}

void MainFrame::CreateFrame() {
    // make menu bar
    wxMenuBar* menu_bar = new wxMenuBar;
    wxMenu* menu_file = new wxMenu;

    if (m_definition["gui"].Size() > 1) {
        for (int i = 0; i < m_definition["gui"].Size(); i++) {
            menu_file->Append(wxID_HIGHEST + i + 1,
                wxString::FromUTF8(m_definition["gui"][i]["label"].GetString()));
            menu_file->Bind(wxEVT_MENU,
                &MainFrame::UpdateFrame, this, wxID_HIGHEST + i + 1);
        }
    }

    menu_file->Append(wxID_EXIT, "Quit");
    menu_bar->Append(menu_file, "Menu");

    // put help urls to menu bar
    if (m_definition.HasMember("help") && m_definition["help"].Size() > 0) {
        wxMenu* menu_help = new wxMenu;

        for (int i = 0; i < m_definition["help"].Size(); i++) {
            menu_help->Append(wxID_HIGHEST + i + 1 + m_definition["gui"].Size(),
                wxString::FromUTF8(m_definition["help"][i]["label"].GetString()));
            menu_help->Bind(wxEVT_MENU,
                &MainFrame::OpenURL, this, wxID_HIGHEST + i + 1 + m_definition["gui"].Size());
        }
        menu_bar->Append(menu_help, "Help");
    }
    SetMenuBar(menu_bar);

    // set events
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    Bind(wxEVT_MENU, &MainFrame::OnCommandClose, this, wxID_EXIT);
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
}

void MainFrame::JsonLoadFailed(const wxString& msg, rapidjson::Document& definition) {
    PRINT_FMT("[LoadDefinition] Error: %s\n", msg);
    ShowErrorDialog(msg);
    json_utils::GetDefaultDefinition(definition);
}

// read gui_definition.json
void MainFrame::CheckDefinition(rapidjson::Document& definition) {
    json_utils::JsonResult result = { true };
    json_utils::CheckVersion(result, definition);
    if (!result.ok) {
        wxString msg = "Failed to load gui_definition.json ("
                       + wxString::FromUTF8(result.msg.c_str()) + ")";
        JsonLoadFailed(msg, definition);
        return;
    }
    if (definition.HasMember("recommended")) {
        if (definition["not_recommended"].GetBool()) {
            PRINT_FMT("[LoadDefinition] Warning: Version %s is recommended.\n",
                        definition["recommended"].GetString());
        }
    }

    json_utils::CheckDefinition(result, definition);
    if (!result.ok) {
        wxString msg = "Failed to load gui_definition.json ("
                       + wxString::FromUTF8(result.msg.c_str()) + ")";
        JsonLoadFailed(msg, definition);
        return;
    }

    json_utils::CheckHelpURLs(result, definition);
    if (!result.ok) {
        wxString msg = "Failed to load gui_definition.json ("
                       + wxString::FromUTF8(result.msg.c_str()) + ")";
        JsonLoadFailed(msg, definition);
        return;
    }
}

void MainFrame::UpdateConfig() {
    for (Component *c : m_components)
        c->GetConfig(m_config);
    if (m_config.HasMember("_mode"))
        m_config.RemoveMember("_mode");
    m_config.AddMember("_mode", m_definition_id, m_config.GetAllocator());
}

void MainFrame::SaveConfig() {
    UpdateConfig();
    json_utils::JsonResult result = json_utils::SaveJson(m_config, "gui_config.json");
    if (result.ok) {
        PRINT("[SaveConfig] Saved gui_config.json\n");
    } else {
        PRINT_FMT("[SaveConfig] Error: %s\n", result.msg.c_str());
    }
}

void MainFrame::ShowErrorDialog(const wxString& msg, const wxString& title) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, title, wxICON_ERROR | wxOK | wxCENTRE);
    dialog->ShowModal();
    dialog->Destroy();
}

void MainFrame::ShowSuccessDialog(const wxString& msg, const wxString& title) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, title);
    dialog->ShowModal();
    dialog->Destroy();
}

// Make command string
wxString MainFrame::GetCommand() {
    std::vector<std::string> cmd_ary;
    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];
    for (rapidjson::Value& c : sub_definition["command_splitted"].GetArray())
        cmd_ary.push_back(c.GetString());
    std::vector<int> cmd_ids;
    for (rapidjson::Value& c : sub_definition["command_ids"].GetArray())
        cmd_ids.push_back(c.GetInt());

    std::vector<wxString> comp_strings = std::vector<wxString>(m_components.size());
    for (int i = 0; i < m_components.size(); i++) {
        comp_strings[i] = m_components[i]->GetString();
    }

    wxString cmd = wxString::FromUTF8(cmd_ary[0].c_str());
    for (int i = 0; i < cmd_ids.size(); i++) {
        int id = cmd_ids[i];
        if (id == CMD_ID_PERCENT) {
            cmd += "%";
        } else if (id == CMD_ID_CURRENT_DIR) {
            cmd += wxGetCwd();
        } else {
            cmd += comp_strings[id];
        }
        if (i + 1 < cmd_ary.size()) {
            cmd += wxString::FromUTF8(cmd_ary[i + 1].c_str());
        }
    }
    return cmd;
}

wxResult MainFrame::RunCommand(wxString& last_line) {
    wxString cmd = GetCommand();

    PRINT_FMT("[RunCommand] Command: %s\n", cmd);
#ifdef _WIN32
    cmd = "cmd.exe /c " + cmd;
#endif
    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];
    bool check_exit_code = json_utils::GetBool(sub_definition, "check_exit_code", false);
    int exit_success = json_utils::GetInt(sub_definition, "exit_success", 0);
    bool show_last_line = json_utils::GetBool(sub_definition, "show_last_line", false);
#ifdef __linux__
    return Exec(*m_ostream, cmd,
#else
    return Exec(cmd,
#endif
                check_exit_code, exit_success,
                show_last_line, last_line);
}

// run command
void MainFrame::ClickButton(wxCommandEvent& event) {
    // save config
    SaveConfig();

    bool failed = false;
    wxString text = m_run_button->GetLabel();
    wxString last_line;

    m_run_button->SetLabel("Processing...");
    wxResult result = RunCommand(last_line);
    m_run_button->SetLabel(text);

    if (!result.ok) {
        PRINT_FMT("[RunCommand] Error: %s\n", result.msg);
        ShowErrorDialog(result.msg);
        return;
    }

    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];
    if (json_utils::GetBool(sub_definition, "show_last_line", false) && last_line != "") {
        ShowSuccessDialog(last_line);
    } else {
        ShowSuccessDialog("Success!");
    }
}

void MainFrame::OpenURL(wxCommandEvent& event) {
    size_t id = event.GetId() - 1 - wxID_HIGHEST - m_definition["gui"].Size();
    rapidjson::Value& help = m_definition["help"].GetArray()[id];
    std::string type = help["type"].GetString();
    wxString url = "";
    wxString tag;

    if (type == "url") {
        url = wxString::FromUTF8(help["url"].GetString());
        tag = "[OpenURL] ";
        int pos = url.Find("://");
        if (pos !=wxNOT_FOUND) {
            wxString scheme = url.Left(pos);
            // scheme should be http or https
            if (scheme.IsSameAs("file", false)) {
                wxString msg = "Use 'file' type for a path, not 'url' type. (" + url + ")";
                PRINT_FMT("%sError: %s\n", tag, msg);
                ShowErrorDialog(msg);
                return;
            } else if (!scheme.IsSameAs("https", false) && !scheme.IsSameAs("http", false)) {
                wxString msg = "Unsupported scheme detected. "
                                    "It should be http or https. (" + scheme + ")";
                PRINT_FMT("%sError: %s\n", tag, msg);
                ShowErrorDialog(msg);
                return;
            }
        } else {
            url = "https://" + url;
        }
    } else if (type == "file") {
        url = wxString::FromUTF8(help["path"].GetString());
        tag = "[OpenFile] ";
        if (!wxFileExists(url) && !wxDirExists(url)) {
            wxString msg = "File does not exist. (" + url + ")";
            PRINT_FMT("%sError: %s\n", tag, msg);
            ShowErrorDialog(msg);
            return;
        }
    }

    PRINT_FMT("%s%s\n", tag.c_str(), url);
    if (type == "file") {
        url = "file:" + url;
    }
    bool success = wxLaunchDefaultBrowser(url);
    if (!success) {
        std::string msg = "Failed to open a " + type + " by an unexpected error.";
        PRINT_FMT("%sError: %s\n", tag, msg.c_str());
        ShowErrorDialog(msg.c_str());
    }
}

void MainFrame::UpdateFrame(wxCommandEvent& event) {
    m_definition_id = event.GetId() - 1 - wxID_HIGHEST;

    UpdateConfig();

    wxPanel* unused_panel = m_panel;
    UpdatePanel();

    unused_panel->Destroy();
    Fit();

    Refresh();
}

void MainFrame::UpdatePanel() {
    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];
    wxString label = wxString::FromUTF8(sub_definition["label"].GetString());
    PRINT_FMT("[UpdatePanel] Lable: %s\n", label);
    wxString cmd_str = wxString::FromUTF8(sub_definition["command_str"].GetString());
    PRINT_FMT("[UpdatePanel] Command: %s\n", cmd_str);
    wxString window_name = wxString::FromUTF8(
        json_utils::GetString(sub_definition, "window_name", "Simple Command Runner").c_str());
    SetLabel(window_name);

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* comp_sizer = new wxBoxSizer(wxVERTICAL);
    comp_sizer->SetMinSize(wxSize(200, 25));
    m_panel = new wxPanel(this);

    // put components
    m_components.clear();
    m_components.shrink_to_fit();
    Component* new_comp = nullptr;
    if (sub_definition["components"].Size() > 0) {
        for (rapidjson::Value& c : sub_definition["components"].GetArray()) {
            new_comp = Component::PutComponent(m_panel, comp_sizer, c);
            if (new_comp == nullptr) {
                ShowErrorDialog("Unknown component type detected. This is unexpected.");
            } else {
                new_comp->SetConfig(m_config);
                m_components.push_back(new_comp);
            }
        }
    }

    // put a button
    wxString button = wxString::FromUTF8(
        json_utils::GetString(sub_definition, "button", "Run").c_str());
    m_run_button = new wxButton(m_panel, wxID_EXECUTE, button);
    comp_sizer->Add(m_run_button, 0, wxFIXED_MINSIZE | wxALIGN_CENTER_HORIZONTAL);

    main_sizer->Add(comp_sizer, 0, wxALL, 15);
    m_panel->SetSizerAndFit(main_sizer);
    m_panel->Show();
}

void MainFrame::OnClose(wxCloseEvent& event) {
#ifdef __linux__
    m_log_frame->Destroy();
#endif
    Destroy();
}

void MainFrame::GetDefinition(rapidjson::Document& json) {
    json.CopyFrom(m_definition, json.GetAllocator());
}

#undef PRINT_FMT
#undef PRINT
