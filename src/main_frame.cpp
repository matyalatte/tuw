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
    PrintFmt(tuw_constants::LOGO);

    m_grid = NULL;
    m_menu_item = NULL;
    std::string exe_path = envuStr(envuGetExecutablePath());

    m_definition.CopyFrom(definition, m_definition.GetAllocator());
    m_config.CopyFrom(config, m_config.GetAllocator());
    bool ignore_external_json = false;
    json_utils::JsonResult result = JSON_RESULT_OK;
    if (!m_definition.IsObject() || m_definition.ObjectEmpty()) {
        bool exists_external_json = envuFileExists("gui_definition.json");
        ExeContainer exe;

        result = exe.Read(exe_path);
        if (result.ok) {
            if (exe.HasJson()) {
                PrintFmt("[LoadDefinition] Found JSON in the executable.\n");
                exe.GetJson(m_definition);
                ignore_external_json = exists_external_json;
            } else {
                PrintFmt("[LoadDefinition] Embedded JSON not found.\n");
                result = { false, "" };
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

    unsigned definition_id = 0;
    if (m_config.HasMember("_mode") && m_config["_mode"].IsInt()) {
        unsigned mode = m_config["_mode"].GetInt();
        if (mode < m_definition["gui"].Size())
            definition_id = mode;
    }

    CreateMenu();
    CreateFrame();
#ifdef __TUW_UNIX__
    uiMainStep(1);  // Need uiMainStep before using uiMsgBox
#endif

    if (ignore_external_json) {
        const char* msg =
            "WARNING: Using embedded JSON. gui_definition.json was ignored.\n";
        PrintFmt("[LoadDefinition] %s", msg);
        ShowSuccessDialog(msg, "Warning");
    }

    if (!result.ok)
        JsonLoadFailed(result.msg);

    UpdatePanel(definition_id);
    Fit();
}

static int OnClosing(uiWindow *w, void *data) {
    uiQuit();
    UNUSED(w);
    UNUSED(data);
    return 1;
}

static int OnShouldQuit(void *data) {
    uiWindow *mainwin = uiWindow(data);
    uiControlDestroy(uiControl(mainwin));
    return 1;
}

void MainFrame::CreateFrame() {
    m_mainwin = uiNewWindow(tuw_constants::TOOL_NAME, 200, 1, 1);
#ifdef __APPLE__
    // Move the default position from bottom left to top left.
    uiWindowSetPosition(m_mainwin, 0, 0);
#endif
    uiWindowOnClosing(m_mainwin, OnClosing, NULL);
    uiOnShouldQuit(OnShouldQuit, m_mainwin);
    uiControlShow(uiControl(m_mainwin));
    uiWindowSetMargined(m_mainwin, 1);

#ifdef __TUW_UNIX__
    // Console window for linux
    char *exe_path = envuGetExecutablePath();
    m_logwin = uiNewWindow(exe_path, 600, 400, 0);
    envuFree(exe_path);
    uiWindowOnClosing(m_logwin, OnClosing, NULL);
    uiMultilineEntry* log_entry = uiNewMultilineEntry();

    /*
    If your monospace font doesn't work,
    you should make a config file to change the default font.
    ```
    <!-- ~/.config/fontconfig/fonts.conf -->
    <match target="pattern">
        <test name="family" qual="any">
            <string>monospace</string>
        </test>
        <edit binding="strong" mode="prepend" name="family">
            <string>Source Code Pro</string>
        </edit>
    </match>
    ```
    */
    uiUnixMultilineEntrySetMonospace(log_entry, 1);
    uiMultilineEntrySetReadOnly(log_entry, 1);
    SetLogEntry(log_entry);
    uiBox* log_box = uiNewVerticalBox();
    uiBoxAppend(log_box, uiControl(log_entry), 1);
    uiWindowSetChild(m_logwin, uiControl(log_box));
    uiControlShow(uiControl(m_logwin));
#endif
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
    UNUSED(item);
    UNUSED(w);
}

static void OnOpenURL(uiMenuItem *item, uiWindow *w, void *data) {
    MenuData* menu_data = static_cast<MenuData*>(data);
    menu_data->main_frame->OpenURL(menu_data->menu_id);
    UNUSED(item);
    UNUSED(w);
}

void MainFrame::CreateMenu() {
    uiMenuItem* item;
    uiMenu* menu = NULL;

#ifdef __APPLE__
    // No need the menu for the quit item on macOS.
    if (m_definition["gui"].Size() > 1) {
        menu = uiNewMenu("Menu");
#else
    menu = uiNewMenu("Menu");
    if (m_definition["gui"].Size() > 1) {
#endif  // __APPLE__
        for (unsigned i = 0; i < m_definition["gui"].Size(); i++) {
            item = uiMenuAppendItem(menu, m_definition["gui"][i]["label"].GetString());
            uiMenuItemOnClicked(item, OnUpdatePanel, new MenuData(this, i));
        }
    }
    item = uiMenuAppendQuitItem(menu);

    // put help urls to menu bar
    if (m_definition.HasMember("help") && m_definition["help"].Size() > 0) {
        menu = uiNewMenu("Help");

        for (unsigned i = 0; i < m_definition["help"].Size(); i++) {
            item = uiMenuAppendItem(menu, m_definition["help"][i]["label"].GetString());
            uiMenuItemOnClicked(item, OnOpenURL, new MenuData(this, i));
        }
    }
    menu = uiNewMenu("Debug");
    m_menu_item = uiMenuAppendCheckItem(menu, "Safe Mode");
}

static bool IsValidURL(const std::string &url) {
    for (const char c : { ' ', ';', '|', '&', '\r', '\n' }) {
        if (url.find(c) != std::string::npos)
            return false;
    }
    return true;
}

void MainFrame::OpenURL(int id) {
    rapidjson::Value& help = m_definition["help"].GetArray()[id];
    std::string type = help["type"].GetString();
    std::string url = "";
    std::string tag;

    if (type == "url") {
        url = help["url"].GetString();
        tag = "[OpenURL] ";

        size_t pos = url.find("://");
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
        char *url_cstr = envuGetRealPath(help["path"].GetString());
        int exists = envuFileExists(url_cstr);
        url = envuStr(url_cstr);
        tag = "[OpenFile] ";

        if (!exists) {
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

    if (!IsValidURL(url)) {
        std::string msg = "URL should NOT contains ' ', ';', '|', '&', '\\r', nor '\\n'.\n"
                          "URL: " + url;
        PrintFmt("%sError: %s\n", tag.c_str(), msg.c_str());
        ShowErrorDialog(msg.c_str());
        return;
    }

    if (IsSafeMode()) {
        std::string msg = "The URL was not opened because the safe mode is enabled.\n"
                          "You can disable it from the menu bar (Debug > Safe Mode.)\n"
                          "\n"
                          "URL: " + url;
        ShowSuccessDialog(msg, "Safe Mode");
    } else {
        ExecuteResult result = LaunchDefaultApp(url);
        if (result.exit_code != 0) {
            std::string msg = "Failed to open a " + type + " by an unexpected error.";
            PrintFmt("%sError: %s\n", tag.c_str(), msg.c_str());
            ShowErrorDialog(msg.c_str());
        }
    }
}

static void OnClicked(uiButton *sender, void *data) {
    MainFrame* main_frame = static_cast<MainFrame*>(data);

    if (!main_frame->Validate())
        return;
    main_frame->SaveConfig();
    main_frame->RunCommand();
    UNUSED(sender);
}

void MainFrame::UpdatePanel(unsigned definition_id) {
    m_definition_id = definition_id;
    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];
    const char* label = sub_definition["label"].GetString();
    PrintFmt("[UpdatePanel] Label: %s\n", label);
    const char* cmd_str = sub_definition["command_str"].GetString();
    PrintFmt("[UpdatePanel] Command: %s\n", cmd_str);
    const char* window_name = json_utils::GetString(sub_definition,
                                                    "window_name", tuw_constants::TOOL_NAME);
    uiWindowSetTitle(m_mainwin, window_name);

    uiGrid* old_grid = m_grid;
    m_grid = uiNewGrid();
    uiGridSetSpacing(m_grid, tuw_constants::GRID_MAIN_SPACE, tuw_constants::GRID_MAIN_SPACE);
    uiBox* main_box = uiNewVerticalBox();
    uiBoxSetSpacing(main_box, tuw_constants::BOX_MAIN_SPACE);

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
            uiBoxSetSpacing(priv_box, tuw_constants::BOX_SUB_SPACE);
            new_comp = Component::PutComponent(priv_box, c);
            new_comp->SetConfig(m_config);
            m_components.push_back(new_comp);
            uiBoxAppend(main_box, uiControl(priv_box), 0);
        }
    }

    uiGridAppend(m_grid, uiControl(main_box), 0, 0, 1, 1, 1, uiAlignFill, 1, uiAlignFill);

    // put a button
    const char* button = json_utils::GetString(sub_definition, "button", "Run");
    m_run_button = uiNewButton(button);
    uiButtonOnClicked(m_run_button, OnClicked, this);
    uiButtonSetMinSize(m_run_button, tuw_constants::BTN_WIDTH, tuw_constants::BTN_HEIGHT);
    uiGridAppend(m_grid, uiControl(m_run_button), 0, 1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);

    uiWindowSetChild(m_mainwin, uiControl(m_grid));

    if (old_grid != NULL) {
        uiControlDestroy(uiControl(old_grid));
    }
}

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void MainFrame::Fit(bool keep_width) {
    int width = 200;
    if (keep_width) {
        int height;
        uiWindowContentSize(m_mainwin, &width, &height);
        width = MAX(width, 200);
    }
    for (Component* c : m_components) {
        // Widen the window if a component has text box.
        if (c->IsWide()) {
            width = MAX(width, 400);
            break;
        }
    }
    // Fit the window size to the new components.
    uiWindowSetContentSize(m_mainwin, width, 1);
}

// Do validation for each component
bool MainFrame::Validate() {
    bool validate = true;
    bool redraw_flag = false;
    std::string val_first_err;
    for (Component* comp : m_components) {
        if (!comp->Validate(&redraw_flag)) {
            std::string val_err = comp->GetValidationError();
            if (validate)
                val_first_err = val_err;
            validate = false;
            PrintFmt("[RunCommand] Error: %s\n", val_err.c_str());
        }
    }

    if (redraw_flag) {
        Fit(true);
    #ifdef _WIN32
        uiWindowsWindowRedraw(m_mainwin);
    #endif
    }

    if (!validate)
        ShowErrorDialog(val_first_err);

    return validate;
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
    for (size_t i = 0; i < m_components.size(); i++) {
        comp_strings[i] = m_components[i]->GetString();
    }

    std::string cmd = cmd_ary[0];
    for (size_t i = 0; i < cmd_ids.size(); i++) {
        int id = cmd_ids[i];
        if (id == CMD_ID_PERCENT) {
            cmd += "%";
        } else if (id == CMD_ID_CURRENT_DIR) {
            cmd += envuStr(envuGetCwd());
        } else if (id == CMD_ID_HOME_DIR) {
            cmd += envuStr(envuGetHome());
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
    std::string cmd = GetCommand();
    PrintFmt("[RunCommand] Command: %s\n", cmd.c_str());

    if (IsSafeMode()) {
        std::string msg = "The command was not executed because the safe mode is enabled.\n"
                          "You can disable it from the menu bar (Debug > Safe Mode.)\n"
                          "\n"
                          "Command: " + cmd;
        ShowSuccessDialog(msg, "Safe Mode");
        return;
    }

    char* text = uiButtonText(m_run_button);
    uiButtonSetText(m_run_button, "Processing...");
#ifdef __APPLE__
    uiMainStep(1);
#elif defined(__TUW_UNIX__)
    uiUnixWaitEvents();
#endif
    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];

    std::string codepage = json_utils::GetString(sub_definition, "codepage", "");
    bool use_utf8_on_windows = codepage == "utf8" || codepage == "utf-8";

    ExecuteResult result = Execute(cmd, use_utf8_on_windows);
    uiButtonSetText(m_run_button, text);

    bool check_exit_code = json_utils::GetBool(sub_definition, "check_exit_code", false);
    int exit_success = json_utils::GetInt(sub_definition, "exit_success", 0);
    bool show_last_line = json_utils::GetBool(sub_definition, "show_last_line", false);
    bool show_success_dialog = json_utils::GetBool(sub_definition, "show_success_dialog", true);

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

    if (!show_success_dialog) {
        PrintFmt("[RunCommand] Done\n");
        return;
    }

    if (show_last_line && result.last_line != "") {
        ShowSuccessDialog(result.last_line);
        return;
    }

    ShowSuccessDialog("Success!");
}

// read gui_definition.json
json_utils::JsonResult MainFrame::CheckDefinition(rapidjson::Document& definition) {
    json_utils::JsonResult result = JSON_RESULT_OK;
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

void MainFrame::JsonLoadFailed(const std::string& msg) {
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

bool g_no_dialog = false;

void MainFrame::ShowSuccessDialog(const std::string& msg, const std::string& title) {
    if (g_no_dialog) return;
    uiMsgBox(m_mainwin, title.c_str(), msg.c_str());
}

void MainFrame::ShowErrorDialog(const std::string& msg, const std::string& title) {
    if (g_no_dialog) return;
    uiMsgBoxError(m_mainwin, title.c_str(), msg.c_str());
}

void MainFrameDisableDialog() {
    g_no_dialog = true;
}

void MainFrame::GetDefinition(rapidjson::Document& json) {
    json.CopyFrom(m_definition, json.GetAllocator());
}
