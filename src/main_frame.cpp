#include "main_frame.h"
#include "rapidjson/error/en.h"
#include "exe_container.h"
#include "env_utils.h"
#include "exec.h"
#include "string_utils.h"
#include "tuw_constants.h"

// Main window
MainFrame::MainFrame(const rapidjson::Document& definition, const rapidjson::Document& config) {
    PrintFmt("%s v%s by %s\n", tuw_constants::TOOL_NAME,
              tuw_constants::VERSION, tuw_constants::AUTHOR);

    m_box = NULL;
#ifdef __linux__
    m_log_entry = NULL;
#endif
    std::string exe_path = env_utils::GetExecutablePath();

    m_definition.CopyFrom(definition, m_definition.GetAllocator());
    m_config.CopyFrom(config, m_config.GetAllocator());
    json_utils::JsonResult result = { true };
    if (!m_definition.IsObject() || m_definition.ObjectEmpty()) {
        bool exists_external_json = env_utils::FileExists("gui_definition.json");
        ExeContainer exe;

        result = exe.Read(exe_path);
        if (result.ok) {
            if (exe.HasJson()) {
                PrintFmt("[LoadDefinition] Found JSON in the executable.\n");
                exe.GetJson(m_definition);
                if (exists_external_json) {
                    const char* msg =
                        "WARNING: Using embedded JSON. gui_definition.json was ignored.\n";
                    PrintFmt("[LoadDefinition] %s", msg);
                    // ShowSuccessDialog(msg, "Warning");
                }
            } else {
                PrintFmt("[LoadDefinition] Embedded JSON not found.\n");
                result = { false };
            }
        } else {
            PrintFmt("[LoadDefinition] ERROR: %s\n", result.msg.c_str());
        }

        if (!result.ok) {
            if (exists_external_json) {
                PrintFmt("[LoadDefinition] Loaded gui_definition.json\n");
                result = json_utils::LoadJson("gui_definition.json", m_definition);
                if (!result.ok)
                    m_definition.SetObject();
            } else {
                result = { false, "gui_definition.json not found." };
            }
        }
    }

    if (!config.IsObject() || config.ObjectEmpty()) {
        json_utils::JsonResult cfg_result =
            json_utils::LoadJson("gui_config.json", m_config);
        if (!cfg_result.ok) {
            m_config.SetObject();
        }
    }

    if (result.ok)
        result = CheckDefinition(m_definition);

    if (!result.ok)
        json_utils::GetDefaultDefinition(m_definition);

    int definition_id = 0;
    if (m_config.HasMember("_mode") && m_config["_mode"].IsInt()) {
        int mode = m_config["_mode"].GetInt();
        if (mode < m_definition["gui"].Size())
            definition_id = mode;
    }

    CreateMenu();
    CreateFrame();

    if (!result.ok) {
        uiMainStep(1);
        JsonLoadFailed(result.msg, m_definition);
    }

    UpdatePanel(definition_id);
    Fit();
}

static int OnClosing(uiWindow *w, void *data) {
    uiQuit();
    return 1;
}

static int OnShouldQuit(void *data) {
    uiWindow *mainwin = uiWindow(data);
    uiControlDestroy(uiControl(mainwin));
    return 1;
}

void MainFrame::CreateFrame() {
    m_mainwin = uiNewWindow(tuw_constants::TOOL_NAME, 400, 1, 1);
    uiWindowOnClosing(m_mainwin, OnClosing, NULL);
    uiOnShouldQuit(OnShouldQuit, m_mainwin);
    uiControlShow(uiControl(m_mainwin));
}

struct MenuData {
    MenuData(MainFrame* mf, int id):
        main_frame{mf}, menu_id{id} {}
    MainFrame* main_frame;
    int menu_id;
};

static void OnUpdatePanel(uiMenuItem *item, uiWindow *w, void *data) {
    MenuData* menu_data = static_cast<MenuData*>(data);
    menu_data->main_frame->UpdatePanel(menu_data->menu_id);
    menu_data->main_frame->Fit();
}

static void OnOpenURL(uiMenuItem *item, uiWindow *w, void *data) {
    MenuData* menu_data = static_cast<MenuData*>(data);
    menu_data->main_frame->OpenURL(menu_data->menu_id);
}

void MainFrame::CreateMenu() {
    uiMenuItem* item;
    uiMenu* menu = uiNewMenu("Menu");
    if (m_definition["gui"].Size() > 1) {
        for (int i = 0; i < m_definition["gui"].Size(); i++) {
            item = uiMenuAppendItem(menu, m_definition["gui"][i]["label"].GetString());
            uiMenuItemOnClicked(item, OnUpdatePanel, new MenuData(this, i));
        }
    }
    item = uiMenuAppendQuitItem(menu);

    // put help urls to menu bar
    if (m_definition.HasMember("help") && m_definition["help"].Size() > 0) {
        menu = uiNewMenu("Help");

        for (int i = 0; i < m_definition["help"].Size(); i++) {
            item = uiMenuAppendItem(menu, m_definition["help"][i]["label"].GetString());
            uiMenuItemOnClicked(item, OnOpenURL, new MenuData(this, i));
        }
    }
}

void MainFrame::OpenURL(int id) {
    rapidjson::Value& help = m_definition["help"].GetArray()[id];
    std::string type = help["type"].GetString();
    std::string url = "";
    std::string tag;

    if (type == "url") {
        url = help["url"].GetString();
        tag = "[OpenURL] ";

        int pos = url.find("://");
        if (pos != std::string::npos) {
            std::string scheme = url.substr(0, pos);
            // scheme should be http or https
            if (scheme == "file") {
                std::string msg = "Use 'file' type for a path, not 'url' type. (" + url + ")";
                PrintFmt("%sError: %s\n", tag.c_str(), msg.c_str());
                ShowErrorDialog(msg);
                return;
            } else if (scheme != "https" && scheme != "http") {
                std::string msg = "Unsupported scheme detected. "
                                  "It should be http or https. (" + scheme + ")";
                PrintFmt("%sError: %s\n", tag.c_str(), msg.c_str());
                ShowErrorDialog(msg);
                return;
            }
        } else {
            url = "https://" + url;
        }

    } else if (type == "file") {
        url = env_utils::GetFullPath(help["path"].GetString());
        tag = "[OpenFile] ";
        if (!env_utils::FileExists(url)) {
            std::string msg = "File does not exist. (" + url + ")";
            PrintFmt("%sError: %s\n", tag.c_str(), msg.c_str());
            ShowErrorDialog(msg);
            return;
        }
    }

    PrintFmt("%s%s\n", tag.c_str(), url.c_str());

    if (type == "file") {
        url = "file:" + url;
    }

    ExecuteResult result = LaunchDefaultApp(url);
    if (result.exit_code != 0) {
        std::string msg = "Failed to open a " + type + " by an unexpected error.";
        PrintFmt("%sError: %s\n", tag.c_str(), msg.c_str());
        ShowErrorDialog(msg.c_str());
    }
}

static void OnClicked(uiButton *sender, void *data) {
    MainFrame* main_frame = static_cast<MainFrame*>(data);
    main_frame->SaveConfig();
    main_frame->RunCommand();
}

void MainFrame::UpdatePanel(int definition_id) {
    m_definition_id = definition_id;
    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];
    const char* label = sub_definition["label"].GetString();
    PrintFmt("[UpdatePanel] Lable: %s\n", label);
    const char* cmd_str = sub_definition["command_str"].GetString();
    PrintFmt("[UpdatePanel] Command: %s\n", cmd_str);
    const char* window_name = json_utils::GetString(sub_definition,
                                                    "window_name", tuw_constants::TOOL_NAME);
    uiWindowSetTitle(m_mainwin, window_name);

#ifdef __linux__
    if (m_log_entry == NULL) {
        m_log_entry = uiNewMultilineEntry();
        uiMultilineEntrySetReadOnly(m_log_entry, 1);
        SetLogEntry(m_log_entry);
    } else {
        uiBoxDelete(m_box, uiBoxNumChildren(m_box) - 1);
    }
#endif

    uiBox* old_box = m_box;
    m_box = uiNewVerticalBox();
    uiBoxSetPadded(m_box, 1);

    // Delete old components
    for (Component* comp : m_components) {
        delete comp;
    }
    m_components.clear();
    m_components.shrink_to_fit();

    // Put new components
    Component* new_comp = nullptr;
    if (sub_definition["components"].Size() > 0) {
        for (rapidjson::Value& c : sub_definition["components"].GetArray()) {
            uiBox* priv_box = uiNewVerticalBox();
            new_comp = Component::PutComponent(priv_box, c);
            if (new_comp == nullptr) {
                ShowErrorDialog("Unknown component type detected. This is unexpected.");
            } else {
                new_comp->SetConfig(m_config);
                m_components.push_back(new_comp);
            }
        uiBoxAppend(m_box, uiControl(priv_box), 0);
        }
    }

    // put a button
    const char* button = json_utils::GetString(sub_definition, "button", "Run");
    m_run_button = uiNewButton(button);
    uiButtonOnClicked(m_run_button, OnClicked, this);
    uiBoxAppend(m_box, uiControl(m_run_button), 0);

#ifdef __linux__
    // Todo: Make the log frame larger.
    uiBoxAppend(m_box, uiControl(m_log_entry), 1);
#endif

    uiWindowSetChild(m_mainwin, uiControl(m_box));
    uiWindowSetMargined(m_mainwin, 1);

    if (old_box != NULL) {
        uiControlDestroy(uiControl(old_box));
    }
}

void MainFrame::Fit() {
    // Fit the window size to the new components.
    int width;
    int height;
    uiWindowContentSize(m_mainwin, &width, &height);
    uiWindowSetContentSize(m_mainwin, width, 1);
#ifdef __linux__
    // Expand the log entry.
    uiWindowContentSize(m_mainwin, &width, &height);
    uiWindowSetContentSize(m_mainwin, width, height + 100);
#endif
}

// Make command string
std::string MainFrame::GetCommand() {
    std::vector<std::string> cmd_ary;
    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];
    for (rapidjson::Value& c : sub_definition["command_splitted"].GetArray())
        cmd_ary.push_back(c.GetString());
    std::vector<int> cmd_ids;
    for (rapidjson::Value& c : sub_definition["command_ids"].GetArray())
        cmd_ids.push_back(c.GetInt());

    std::vector<std::string> comp_strings = std::vector<std::string>(m_components.size());
    for (int i = 0; i < m_components.size(); i++) {
        comp_strings[i] = m_components[i]->GetString();
    }

    std::string cmd = cmd_ary[0];
    for (int i = 0; i < cmd_ids.size(); i++) {
        int id = cmd_ids[i];
        if (id == CMD_ID_PERCENT) {
            cmd += "%";
        } else if (id == CMD_ID_CURRENT_DIR) {
            cmd += env_utils::GetCwd();
        } else if (id == CMD_ID_HOME_DIR) {
            cmd += env_utils::GetHome();
        } else {
            cmd += comp_strings[id];
        }
        if (i + 1 < cmd_ary.size()) {
            cmd += cmd_ary[i + 1];
        }
    }
    return cmd;
}

void MainFrame::RunCommand() {
    char* text = uiButtonText(m_run_button);
    uiButtonSetText(m_run_button, "processing...");

    std::string cmd = GetCommand();
    PrintFmt("[RunCommand] Command: %s\n", cmd.c_str());
    ExecuteResult result = Execute(cmd);
    uiButtonSetText(m_run_button, text);

    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];
    bool check_exit_code = json_utils::GetBool(sub_definition, "check_exit_code", false);
    int exit_success = json_utils::GetInt(sub_definition, "exit_success", 0);
    bool show_last_line = json_utils::GetBool(sub_definition, "show_last_line", false);

    if (result.err_msg != "") {
        PrintFmt("[RunCommand] Error: %s\n", result.err_msg.c_str());
        ShowErrorDialog(result.err_msg);
        return;
    }

    if (check_exit_code && result.exit_code != exit_success) {
        std::string err_msg;
        if (show_last_line)
            err_msg = result.last_line;
        else
            err_msg = "Invalid exit code (" + std::to_string(result.exit_code) + ")";
        PrintFmt("[RunCommand] Error: %s\n", err_msg.c_str());
        ShowErrorDialog(err_msg);
        return;
    }

    if (show_last_line && result.last_line != "") {
        ShowSuccessDialog(result.last_line);
    } else {
        ShowSuccessDialog("Success!");
    }
}

// read gui_definition.json
json_utils::JsonResult MainFrame::CheckDefinition(rapidjson::Document& definition) {
    json_utils::JsonResult result = { true };
    json_utils::CheckVersion(result, definition);
    if (!result.ok) return result;

    if (definition.HasMember("recommended")) {
        if (definition["not_recommended"].GetBool()) {
            PrintFmt("[LoadDefinition] Warning: Version %s is recommended.\n",
                        definition["recommended"].GetString());
        }
    }

    json_utils::CheckDefinition(result, definition);
    if (!result.ok) return result;

    json_utils::CheckHelpURLs(result, definition);
    return result;
}

void MainFrame::JsonLoadFailed(const std::string& msg, rapidjson::Document& definition) {
    PrintFmt("[LoadDefinition] Error: %s\n", msg.c_str());
    ShowErrorDialog(msg);
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
        PrintFmt("[SaveConfig] Saved gui_config.json\n");
    } else {
        PrintFmt("[SaveConfig] Error: %s\n", result.msg.c_str());
    }
}

void MainFrame::ShowSuccessDialog(const std::string& msg, const std::string& title) {
    uiMsgBox(m_mainwin, title.c_str(), msg.c_str());
}

void MainFrame::ShowErrorDialog(const std::string& msg, const std::string& title) {
    uiMsgBoxError(m_mainwin, title.c_str(), msg.c_str());
}

void MainFrame::GetDefinition(rapidjson::Document& json) {
    json.CopyFrom(m_definition, json.GetAllocator());
}
