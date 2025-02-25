#include "main_frame.h"
#include "rapidjson/error/en.h"
#include "exe_container.h"
#include "env_utils.h"
#include "exec.h"
#include "string_utils.h"
#include "tuw_constants.h"

noex::string GetDefaultJsonPath() {
    noex::string def_name = "gui_definition.";
    for (const char* ext : { "jsonc", "tuw" }) {
        noex::string json_path = def_name + ext;
        if (envuFileExists(json_path.c_str()))
            return json_path;
    }
    return def_name + "json";
}

// Main window
void MainFrame::Initialize(const rapidjson::Document& definition,
                           const rapidjson::Document& config,
                           noex::string json_path) noexcept {
    PrintFmt("%s v%s by %s\n", tuw_constants::TOOL_NAME,
              tuw_constants::VERSION, tuw_constants::AUTHOR);
    PrintFmt(tuw_constants::LOGO);

    m_grid = NULL;
    m_menu_item = NULL;
    noex::string exe_path = envuStr(envuGetExecutablePath());

    m_definition.CopyFrom(definition, m_definition.GetAllocator());
    m_config.CopyFrom(config, m_config.GetAllocator());
    bool ignore_external_json = false;

    noex::string workdir;
    if (json_path.empty()) {
        workdir = envuStr(envuGetDirectory(exe_path.c_str()));
    } else {
        char* full_json_path = envuGetFullPath(json_path.c_str());
        if (full_json_path)
            json_path = full_json_path;
        workdir = envuStr(envuGetDirectory(full_json_path));
        envuFree(full_json_path);
    }

    if (!workdir.empty()) {
        // Set working directory
        int ret = envuSetCwd(workdir.c_str());
        if (ret != 0) {
            // Failed to set CWD
            PrintFmt("[LoadDefinition] Failed to set a path as CWD. (%s)\n", workdir.c_str());
        }
    }

    if (json_path.empty()) {
        // Find gui_definition.json
        json_path = GetDefaultJsonPath();
    }

    bool exists_external_json = envuFileExists(json_path.c_str());

    json_utils::JsonResult result = JSON_RESULT_OK;
    if (!m_definition.IsObject() || m_definition.ObjectEmpty()) {
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
                PrintFmt("[LoadDefinition] Loaded %s\n", json_path.c_str());
                result = json_utils::LoadJson(json_path, m_definition);
                if (!result.ok)
                    m_definition.SetObject();
            } else {
                result = { false, json_path + " not found." };
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

    unsigned definition_id = json_utils::GetInt(m_config, "_mode", 0);
    if (definition_id >= m_definition["gui"].Size())
        definition_id = 0;

    CreateMenu();
    CreateFrame();
#ifdef __TUW_UNIX__
    uiMainStep(1);  // Need uiMainStep before using uiMsgBox
#endif

    {
        // Show CWD
        char* cwd = envuGetCwd();
        PrintFmt("[LoadDefinition] CWD: %s\n", cwd);
        envuFree(cwd);
    }

    if (ignore_external_json) {
        noex::string msg = "WARNING: Using embedded JSON. " +
                            json_path + " was ignored.\n";
        PrintFmt("[LoadDefinition] %s", msg.c_str());
        ShowSuccessDialog(msg, "Warning");
    }

    if (!result.ok)
        JsonLoadFailed(result.msg);

    UpdatePanel(definition_id);
    Fit();
}

static int OnClosing(uiWindow *w, void *data) noexcept {
    uiQuit();
    UNUSED(w);
    UNUSED(data);
    return 1;
}

static int OnShouldQuit(void *data) noexcept {
    uiWindow *mainwin = uiWindow(data);
    uiControlDestroy(uiControl(mainwin));
    return 1;
}

#ifdef __TUW_UNIX__
static uiWindow* CreateLogWindowForGtk() {
    // Console window for linux
    char *exe_path = envuGetExecutablePath();
    uiWindow *log_win = uiNewWindow(exe_path, 600, 400, 0);
    envuFree(exe_path);
    uiWindowOnClosing(log_win, OnClosing, NULL);
    uiMultilineEntry* log_entry = uiNewMultilineEntry();
    uiMultilineEntrySetReadOnly(log_entry, 1);
    SetLogEntry(log_entry);
    uiBox* log_box = uiNewVerticalBox();
    uiBoxAppend(log_box, uiControl(log_entry), 1);
    uiWindowSetChild(log_win, uiControl(log_box));
    uiControlShow(uiControl(log_win));
    return log_win;
}
#endif

void MainFrame::CreateFrame() noexcept {
#ifdef __TUW_UNIX__
    m_logwin = CreateLogWindowForGtk();
#endif

    m_mainwin = uiNewWindow(tuw_constants::TOOL_NAME, 200, 1, 1);
#ifdef __APPLE__
    // Move the default position from bottom left to top left.
    uiWindowSetPosition(m_mainwin, 0, 0);
#endif
    uiWindowOnClosing(m_mainwin, OnClosing, NULL);
    uiOnShouldQuit(OnShouldQuit, m_mainwin);
    uiControlShow(uiControl(m_mainwin));
    uiWindowSetMargined(m_mainwin, 1);
}

static void OnUpdatePanel(uiMenuItem *item, uiWindow *w, void *data) noexcept {
    MenuData* menu_data = static_cast<MenuData*>(data);
    menu_data->main_frame->UpdatePanel(menu_data->menu_id);
    menu_data->main_frame->Fit();
    UNUSED(item);
    UNUSED(w);
}

static void OnOpenURL(uiMenuItem *item, uiWindow *w, void *data) noexcept {
    MenuData* menu_data = static_cast<MenuData*>(data);
    menu_data->main_frame->OpenURL(menu_data->menu_id);
    UNUSED(item);
    UNUSED(w);
}

void MainFrame::CreateMenu() noexcept {
    uiMenuItem* item;
    uiMenu* menu = NULL;

    // Note: We should reserve the buffer to prevent realloc,
    //       or MenuData* pointers can be broken after using push_back()
    size_t menu_item_count = m_definition["gui"].Size();
    if (m_definition.HasMember("help"))
        menu_item_count += m_definition["help"].Size();
    m_menu_data_vec.reserve(menu_item_count);

#ifdef __APPLE__
    // No need the menu for the quit item on macOS.
    if (m_definition["gui"].Size() > 1) {
        menu = uiNewMenu("Menu");
#else
    menu = uiNewMenu("Menu");
    if (m_definition["gui"].Size() > 1) {
#endif  // __APPLE__
        int i = 0;
        for (const rapidjson::Value& j : m_definition["gui"].GetArray()) {
            item = uiMenuAppendItem(menu, j["label"].GetString());
            m_menu_data_vec.push_back({ this, i });
            MenuData* m = &m_menu_data_vec.back();
            uiMenuItemOnClicked(item, OnUpdatePanel, m);
            i++;
        }
    }
    item = uiMenuAppendQuitItem(menu);

    // put help urls to menu bar
    if (m_definition.HasMember("help") && m_definition["help"].Size() > 0) {
        menu = uiNewMenu("Help");

        int i = 0;
        for (const rapidjson::Value& j : m_definition["help"].GetArray()) {
            item = uiMenuAppendItem(menu, j["label"].GetString());
            m_menu_data_vec.push_back({ this, i });
            MenuData* m = &m_menu_data_vec.back();
            uiMenuItemOnClicked(item, OnOpenURL, m);
            i++;
        }
    }
    menu = uiNewMenu("Debug");
    m_menu_item = uiMenuAppendCheckItem(menu, "Safe Mode");
}

static bool IsValidURL(const noex::string &url) noexcept {
    for (const char c : { ' ', ';', '|', '&', '\r', '\n' }) {
        if (url.find(c) != noex::string::npos)
            return false;
    }
    return true;
}

void MainFrame::OpenURL(int id) noexcept {
    rapidjson::Value& help = m_definition["help"].GetArray()[id];
    const char* type = help["type"].GetString();
    noex::string url;
    const char* tag = "";

    if (strcmp(type, "url") == 0) {
        url = help["url"].GetString();
        tag = "[OpenURL] ";

        size_t pos = url.find("://");
        if (pos != noex::string::npos) {
            noex::string scheme = url.substr(0, pos);
            // scheme should be http or https
            if (scheme == "file") {
                noex::string msg = "Use 'file' type for a path, not 'url' type. (" +
                                url + ")";
                PrintFmt("%sError: %s\n", tag, msg.c_str());
                ShowErrorDialog(msg);
                return;
            } else if (scheme != "https" && scheme != "http") {
                noex::string msg = "Unsupported scheme detected. "
                                "It should be http or https. (" + scheme + ")";
                PrintFmt("%sError: %s\n", tag, msg.c_str());
                ShowErrorDialog(msg);
                return;
            }
        } else {
            url = "https://" + url;
        }

    } else if (strcmp(type, "file") == 0) {
        char *url_cstr = envuGetRealPath(help["path"].GetString());
        int exists = envuFileExists(url_cstr);
        url = envuStr(url_cstr);
        tag = "[OpenFile] ";

        if (!exists) {
            noex::string msg = "File does not exist. (" + url + ")";
            PrintFmt("%sError: %s\n", tag, msg.c_str());
            ShowErrorDialog(msg);
            return;
        }
    }

    PrintFmt("%s%s\n", tag, url.c_str());

    if (strcmp(type, "file") == 0) {
        url = "file:" + url;
    }

    if (!IsValidURL(url)) {
        noex::string msg = "URL should NOT contains ' ', ';', '|', '&', '\\r', nor '\\n'.\n"
                          "URL: " + url;
        PrintFmt("%sError: %s\n", tag, msg.c_str());
        ShowErrorDialog(msg.c_str());
        return;
    }

    if (IsSafeMode()) {
        noex::string msg = "The URL was not opened because the safe mode is enabled.\n"
                           "You can disable it from the menu bar (Debug > Safe Mode.)\n"
                           "\n"
                           "URL: " + url;
        ShowSuccessDialog(msg, "Safe Mode");
    } else {
        if (noex::get_error_no() != noex::OK) {
            // Reject the URL as it might have an unexpected value.
            const char* msg =
                "The URL was not opened "
                "because a fatal error has occurred while editing strings or vectors. "
                "Please reboot the application.";
            PrintFmt("%sError: %s\n", tag, msg);
            ShowErrorDialog(msg);
        } else {
            ExecuteResult result = LaunchDefaultApp(url);
            if (result.exit_code != 0) {
                noex::string msg = noex::string("Failed to open a ") +
                                   type + " by an unexpected error.";
                PrintFmt("%sError: %s\n", tag, msg.c_str());
                ShowErrorDialog(msg.c_str());
            }
        }
    }
}

static void OnClicked(uiButton *sender, void *data) noexcept {
    MainFrame* main_frame = static_cast<MainFrame*>(data);

    if (!main_frame->Validate())
        return;
    main_frame->SaveConfig();
    main_frame->RunCommand();
    UNUSED(sender);
}

void MainFrame::UpdatePanel(unsigned definition_id) noexcept {
    m_definition_id = definition_id;
    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];
    if (m_definition["gui"].Size() > 1) {
        const char* label = sub_definition["label"].GetString();
        PrintFmt("[UpdatePanel] Label: %s\n", label);
    }
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
            m_components.emplace_back(new_comp);
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

void MainFrame::Fit(bool keep_width) noexcept {
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
bool MainFrame::Validate() noexcept {
    bool validate = true;
    bool redraw_flag = false;
    noex::string val_first_err;
    for (Component* comp : m_components) {
        if (!comp->Validate(&redraw_flag)) {
            const noex::string& val_err = comp->GetValidationError();
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
noex::string MainFrame::GetCommand() noexcept {
    noex::vector<noex::string> cmd_ary;
    rapidjson::Value& sub_definition = m_definition["gui"][m_definition_id];
    for (rapidjson::Value& c : sub_definition["command_splitted"].GetArray())
        cmd_ary.emplace_back(c.GetString());
    noex::vector<int> cmd_ids;
    for (rapidjson::Value& c : sub_definition["command_ids"].GetArray())
        cmd_ids.emplace_back(c.GetInt());

    noex::vector<noex::string> comp_strings;
    for (Component* comp : m_components) {
        comp_strings.emplace_back(comp->GetString());
    }

    if (noex::get_error_no() != noex::OK)
        return "";

    noex::string cmd = cmd_ary[0];
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

void MainFrame::RunCommand() noexcept {
    noex::string cmd = GetCommand();
    PrintFmt("[RunCommand] Command: %s\n", cmd.c_str());

    if (IsSafeMode()) {
        noex::string msg = "The command was not executed because the safe mode is enabled.\n"
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

    const char* codepage = json_utils::GetString(sub_definition, "codepage", "");
    bool use_utf8_on_windows = strcmp(codepage, "utf8") == 0 || strcmp(codepage, "utf-8") == 0;

    ExecuteResult result = Execute(cmd, use_utf8_on_windows);
    uiButtonSetText(m_run_button, text);

    bool check_exit_code = json_utils::GetBool(sub_definition, "check_exit_code", false);
    int exit_success = json_utils::GetInt(sub_definition, "exit_success", 0);
    bool show_last_line = json_utils::GetBool(sub_definition, "show_last_line", false);
    bool show_success_dialog = json_utils::GetBool(sub_definition, "show_success_dialog", true);

    if (noex::get_error_no() != noex::OK) {
        const char* msg = "Fatal error has occurred while editing strings or vectors. "
                          "Please reboot the application.";
        PrintFmt("[RunCommand] Error: %s\n", msg);
        ShowErrorDialog(msg);
        return;
    }

    if (!result.err_msg.empty()) {
        PrintFmt("[RunCommand] Error: %s\n", result.err_msg.c_str());
        ShowErrorDialog(result.err_msg);
        return;
    }

    if (check_exit_code && result.exit_code != exit_success) {
        noex::string err_msg;
        if (show_last_line)
            err_msg = result.last_line;
        else
            err_msg = noex::string("Invalid exit code (") + result.exit_code + ")";
        PrintFmt("[RunCommand] Error: %s\n", err_msg.c_str());
        ShowErrorDialog(err_msg);
        return;
    }

    if (!show_success_dialog) {
        PrintFmt("[RunCommand] Done\n");
        return;
    }

    if (show_last_line && !result.last_line.empty()) {
        ShowSuccessDialog(result.last_line);
        return;
    }

    ShowSuccessDialog("Success!");
}

// read gui_definition.json
json_utils::JsonResult MainFrame::CheckDefinition(rapidjson::Document& definition) noexcept {
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

void MainFrame::JsonLoadFailed(const noex::string& msg) noexcept {
    PrintFmt("[LoadDefinition] Error: %s\n", msg.c_str());
    ShowErrorDialog(msg);
}

void MainFrame::UpdateConfig() noexcept {
    for (Component *c : m_components)
        c->GetConfig(m_config);
    if (m_config.HasMember("_mode"))
        m_config.RemoveMember("_mode");
    m_config.AddMember("_mode", m_definition_id, m_config.GetAllocator());
}

void MainFrame::SaveConfig() noexcept {
    UpdateConfig();
    json_utils::JsonResult result = json_utils::SaveJson(m_config, "gui_config.json");
    if (result.ok) {
        PrintFmt("[SaveConfig] Saved gui_config.json\n");
    } else {
        PrintFmt("[SaveConfig] Error: %s\n", result.msg.c_str());
    }
}

static bool g_no_dialog = false;

void MainFrame::ShowSuccessDialog(const char* msg, const char* title) noexcept {
    if (g_no_dialog) return;
    uiMsgBox(m_mainwin, title, msg);
}

void MainFrame::ShowErrorDialog(const char* msg, const char* title) noexcept {
    if (g_no_dialog) return;
    uiMsgBoxError(m_mainwin, title, msg);
}

void MainFrameDisableDialog() noexcept {
    g_no_dialog = true;
}

void MainFrame::GetDefinition(rapidjson::Document& json) noexcept {
    json.CopyFrom(m_definition, json.GetAllocator());
}
