#include "main_frame.h"
#include "json.h"
#include "exe_container.h"
#include "env_utils.h"
#include "exec.h"
#include "string_utils.h"
#include "tuw_constants.h"
#ifdef __TUW_UNIX__
#include <gtk/gtk.h>
#endif

#define DEFAULT_JSON_NAME "gui_definition"
const char* GetDefaultJsonPath() noexcept {
    if (envuFileExists(DEFAULT_JSON_NAME ".jsonc"))
        return DEFAULT_JSON_NAME ".jsonc";
    if (envuFileExists(DEFAULT_JSON_NAME ".tuw"))
        return DEFAULT_JSON_NAME ".tuw";
    return DEFAULT_JSON_NAME ".json";
}

// Main window
void MainFrame::Initialize(const tuwjson::Value& definition,
                           const tuwjson::Value& config,
                           noex::string json_path) noexcept {
    PrintFmt("%s v%s by %s\n", tuw_constants::TOOL_NAME,
              tuw_constants::VERSION, tuw_constants::AUTHOR);
    PrintFmt(tuw_constants::LOGO);

    m_grid = NULL;
    m_menu_item = NULL;
    noex::string exe_path = envuStr(envuGetExecutablePath());

    m_definition.CopyFrom(definition);
    m_config.CopyFrom(config);

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
            Log("LoadDefinition", "Failed to set a path as CWD", workdir);
        }
    }

    if (json_path.empty()) {
        // Find gui_definition.json
        json_path = GetDefaultJsonPath();
    }

    bool ignore_external_json = false;
    bool exists_external_json = envuFileExists(json_path.c_str());
    bool loaded = m_definition.IsObject() && !m_definition.IsEmpty();
    noex::string err;

    if (!loaded) {
        ExeContainer exe;

        err = exe.Read(exe_path);
        if (err.empty() && exe.HasJson()) {
            Log("LoadDefinition", "Found JSON in the executable.");
            exe.GetJson(m_definition);
            ignore_external_json = exists_external_json;
            loaded = true;
        } else {
            if (err.empty())
                Log("LoadDefinition", "Embedded JSON not found.");
            else
                Log("LoadDefinition", "Error", err);

            if (exists_external_json) {
                Log("LoadDefinition", "Load", json_path);
                err = json_utils::LoadJson(json_path, m_definition);
                if (err.empty())
                    loaded = true;
                else
                    m_definition.SetObject();
            } else {
                err = json_path + " not found.";
            }
        }
    }

    if (!config.IsObject() || config.IsEmpty()) {
        noex::string cfg_err =
            json_utils::LoadJson("gui_config.json", m_config);
        if (!cfg_err.empty()) {
            m_config.SetObject();
        }
    }

    if (loaded) {
        json_utils::JsonResult result = CheckDefinition(m_definition);
        loaded = result.ok;
        err = result.msg;
    }

    if (!loaded)
        json_utils::GetDefaultDefinition(m_definition);

    unsigned definition_id = json_utils::GetInt(m_config, "_mode", 0);
    m_gui_json = &m_definition["gui"];
    if (definition_id >= m_gui_json->Size())
        definition_id = 0;

    CreateMenu();
    CreateFrame();
#ifdef __TUW_UNIX__
    uiMainStep(1);  // Need uiMainStep before using uiMsgBox
#endif

    {
        // Show CWD
        char* cwd = envuGetCwd();
        Log("LoadDefinition", "CWD", cwd);
        envuFree(cwd);
    }

    if (ignore_external_json) {
        noex::string msg = noex::concat_cstr("Warning: Using embedded JSON. ",
                            json_path.c_str(), " was ignored.\n");
        Log("LoadDefinition", msg);
        ShowSuccessDialog(msg, "Warning");
    }

    if (!loaded)
        ShowErrorDialogWithLog("LoadDefinition", err);

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
static uiWindow* CreateLogWindowForGtk() noexcept {
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

MainFrame* g_main_frame = nullptr;

static void OnUpdatePanel(uiMenuItem *item, uiWindow *w, void *data) noexcept {
    g_main_frame->UpdatePanel(reinterpret_cast<size_t>(data));
    g_main_frame->Fit();
    UNUSED(item);
    UNUSED(w);
}

static void OnOpenURL(uiMenuItem *item, uiWindow *w, void *data) noexcept {
    g_main_frame->OpenURL(reinterpret_cast<size_t>(data));
    UNUSED(item);
    UNUSED(w);
}

void MainFrame::CreateMenu() noexcept {
    g_main_frame = this;
    uiMenuItem* item;
    uiMenu* menu = NULL;

    // Note: We should reserve the buffer to prevent realloc,
    //       or MenuData* pointers can be broken after using push_back()
    size_t menu_item_count = m_gui_json->Size();
    if (m_definition.HasMember("help"))
        menu_item_count += m_definition["help"].Size();

#ifdef __APPLE__
    // No need the menu for the quit item on macOS.
    if (m_gui_json->Size() > 1) {
        menu = uiNewMenu("Menu");
#else
    menu = uiNewMenu("Menu");
    if (m_gui_json->Size() > 1) {
#endif  // __APPLE__
        size_t i = 0;
        for (const tuwjson::Value& j : *m_gui_json) {
            item = uiMenuAppendItem(menu, j["label"].GetString());
            uiMenuItemOnClicked(item, OnUpdatePanel, reinterpret_cast<void*>(i));
            i++;
        }
    }
    item = uiMenuAppendQuitItem(menu);

    // put help urls to menu bar
    if (m_definition.HasMember("help") && m_definition["help"].Size() > 0) {
        menu = uiNewMenu("Help");

        size_t i = 0;
        for (const tuwjson::Value& j : m_definition["help"]) {
            item = uiMenuAppendItem(menu, j["label"].GetString());
            uiMenuItemOnClicked(item, OnOpenURL, reinterpret_cast<void*>(i));
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

noex::string MainFrame::OpenURLBase(size_t id) noexcept {
    tuwjson::Value& help = m_definition["help"][id];
    const char* type = help["type"].GetString();
    noex::string url;

    if (strcmp(type, "url") == 0) {
        url = help["url"].GetString();

        size_t pos = url.find("://");
        if (pos != noex::string::npos) {
            noex::string scheme = url.substr(0, pos);
            // scheme should be http or https
            if (scheme == "file") {
                return noex::concat_cstr(
                    "Use 'file' type for a path, not 'url' type. (", url.c_str(), ")");
            } else if (scheme != "https" && scheme != "http") {
                return noex::concat_cstr("Unsupported scheme detected. "
                        "It should be http or https. (", scheme.c_str(), ")");
            }
        } else {
            url = "https://" + url;
        }

    } else if (strcmp(type, "file") == 0) {
        char *url_cstr = envuGetRealPath(help["path"].GetString());
        int exists = envuFileExists(url_cstr);
        url = envuStr(url_cstr);

        if (!exists)
            return noex::concat_cstr("File does not exist. (", url.c_str(), ")");
    }

    Log("OpenURL", url);

    if (strcmp(type, "file") == 0) {
        url = "file:" + url;
    }

    if (!IsValidURL(url)) {
        return "URL should NOT contains ' ', ';', '|', '&', '\\r', nor '\\n'.\n"
                "URL: " + url;
    }

    if (IsSafeMode()) {
        noex::string msg = "The URL was not opened since the safe mode is enabled.\n"
                            "You can disable it from the menu bar (Debug > Safe Mode.)\n"
                            "\n"
                            "URL: " + url;
        ShowSuccessDialog(msg, "Safe Mode");
    } else {
        if (noex::get_error_no() != noex::OK) {
            // Reject the URL as it might have an unexpected value.
            return "The URL was not opened "
                    "since a fatal error has occurred while editing strings or vectors. "
                    "Please reboot the GUI application.";
        } else {
            ExecuteResult result = LaunchDefaultApp(url);
            if (result.exit_code != 0) {
                return noex::concat_cstr("Failed to open a ",
                        type, " by an unexpected error.");
            }
        }
    }
    return "";
}

void MainFrame::OpenURL(size_t id) noexcept {
    noex::string err = OpenURLBase(id);
    if (!err.empty())
        ShowErrorDialogWithLog("OpenURL", err);
}

static void OnClicked(uiButton *sender, void *data) noexcept {
    MainFrame* main_frame = static_cast<MainFrame*>(data);

    if (!main_frame->Validate())
        return;
    main_frame->SaveConfig();
    main_frame->RunCommand();
    UNUSED(sender);
}

void MainFrame::UpdatePanel(size_t definition_id) noexcept {
    m_definition_id = definition_id;
    tuwjson::Value& sub_definition = m_gui_json->At(m_definition_id);
    if (m_gui_json->Size() > 1) {
        const char* label = sub_definition["label"].GetString();
        Log("UpdatePanel", "Label", label);
    }
    const char* cmd_str = sub_definition["command_str"].GetString();
    Log("UpdatePanel", "Command", cmd_str);
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
    tuwjson::Value& comp_json = sub_definition["components"];
    if (comp_json.Size() > 0) {
        for (tuwjson::Value& c : comp_json) {
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
            Log("RunCommand", "Error", val_err);
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
    tuwjson::Value& sub_definition = m_gui_json->At(m_definition_id);
    tuwjson::Value& cmd_ary = sub_definition["command_splitted"];
    tuwjson::Value& cmd_ids = sub_definition["command_ids"];

    noex::string cmd = cmd_ary[0].GetString();
    for (size_t i = 0; i < cmd_ids.Size(); i++) {
        int id = cmd_ids[i].GetInt();
        if (id == CMD_ID_PERCENT) {
            cmd.push_back('%');
        } else if (id == CMD_ID_CURRENT_DIR) {
            char* cwd = envuGetCwd();
            cmd += cwd;
            envuFree(cwd);
        } else if (id == CMD_ID_HOME_DIR) {
            char* home = envuGetHome();
            cmd += home;
            envuFree(home);
        } else {
            cmd += m_components[id]->GetString();
        }
        if (i + 1 < cmd_ary.Size()) {
            cmd += cmd_ary[i + 1].GetString();
        }
    }
    return cmd;
}

void MainFrame::RunCommand() noexcept {
    noex::string cmd = GetCommand();
    Log("RunCommad", "Command", cmd);

    if (IsSafeMode()) {
        noex::string msg = "The command was not executed since the safe mode is enabled.\n"
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
    tuwjson::Value& sub_definition = m_gui_json->At(m_definition_id);

    const char* codepage = json_utils::GetString(sub_definition, "codepage", "");
    bool use_utf8_on_windows = strcmp(codepage, "utf8") == 0 || strcmp(codepage, "utf-8") == 0;

#ifdef __TUW_UNIX__
    // Disable the main window on Unix
    // since we call the main loop while running commands
    GtkWidget* widget = reinterpret_cast<GtkWidget*>(uiControlHandle(uiControl(m_mainwin)));
    gtk_widget_set_sensitive(widget, FALSE);
#endif
    ExecuteResult result = Execute(cmd, use_utf8_on_windows);
#ifdef __TUW_UNIX__
    gtk_widget_set_sensitive(widget, TRUE);
#endif
    uiButtonSetText(m_run_button, text);

    bool check_exit_code = json_utils::GetBool(sub_definition, "check_exit_code", false);
    int exit_success = json_utils::GetInt(sub_definition, "exit_success", 0);
    bool show_last_line = json_utils::GetBool(sub_definition, "show_last_line", false);
    bool show_success_dialog = json_utils::GetBool(sub_definition, "show_success_dialog", true);

    if (noex::get_error_no() != noex::OK) {
        const char* msg = "The command was not executed "
            "since a fatal error has occurred while editing strings or vectors. "
            "Please reboot the GUI application.";
        ShowErrorDialogWithLog("RunCommand", msg);
        return;
    }

    if (!result.err_msg.empty()) {
        ShowErrorDialogWithLog("RunCommand", result.err_msg);
        return;
    }

    if (check_exit_code && result.exit_code != exit_success) {
        noex::string err_msg;
        if (show_last_line)
            err_msg = result.last_line;
        else
            err_msg = noex::string("Invalid exit code (") + result.exit_code + ")";
        ShowErrorDialogWithLog("RunCommand", err_msg);
        return;
    }

    if (!show_success_dialog) {
        Log("RunCommand", "Done");
        return;
    }

    if (show_last_line && !result.last_line.empty()) {
        ShowSuccessDialog(result.last_line);
        return;
    }

    ShowSuccessDialog("Success!");
}

// read gui_definition.json
json_utils::JsonResult MainFrame::CheckDefinition(tuwjson::Value& definition) noexcept {
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

void MainFrame::UpdateConfig() noexcept {
    for (Component *c : m_components)
        c->GetConfig(m_config);
    m_config["_mode"].SetInt(static_cast<int>(m_definition_id));
}

void MainFrame::SaveConfig() noexcept {
    UpdateConfig();
    noex::string err = json_utils::SaveJson(m_config, "gui_config.json");
    if (err.empty())
        Log("SaveConfig", "Saved gui_config.json");
    else
        Log("SaveConfig", "Error", err);
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

void MainFrame::ShowErrorDialogWithLog(
        const char* func, const char* msg, const char* title) noexcept {
    Log(func, "Error", msg);
    ShowErrorDialog(msg, title);
}

void MainFrameDisableDialog() noexcept {
    g_no_dialog = true;
}

void MainFrame::GetDefinition(tuwjson::Value& json) noexcept {
    json.CopyFrom(m_definition);
}
