#pragma once
#include "json.h"
#include "component.h"
#include "json_utils.h"
#include "string_utils.h"
#include "noex/vector.hpp"
#include "ui.h"

class MainFrame;

#define EMPTY_JSON tuwjson::Value()

// Get "gui_definition.*"
const char* GetDefaultJsonPath() noexcept;

// Main window
class MainFrame {
 private:
    tuwjson::Value m_definition;
    tuwjson::Value* m_gui_json;
    size_t m_definition_id;
    tuwjson::Value m_config;
    uiWindow* m_mainwin;
#ifdef __TUW_UNIX__
    uiWindow* m_logwin;
#endif

    noex::vector<Component*> m_components;
    uiGrid* m_grid;
    uiButton* m_run_button;
    uiMenuItem* m_menu_safe_mode;

    void CreateFrame() noexcept;
    void CreateMenu() noexcept;
    noex::string CheckDefinition(tuwjson::Value& definition) noexcept;
    void UpdateConfig() noexcept;
    void ShowSuccessDialog(const char* msg, const char* title = "Success") noexcept;
    void ShowErrorDialog(const char* msg, const char* title = "Error") noexcept;
    void ShowErrorDialogWithLog(
        const char* func, const char* msg, const char* title = "Error") noexcept;
    inline void ShowSuccessDialog(
            const noex::string& msg,
            const char* title = "Success") noexcept {
        ShowSuccessDialog(msg.c_str(), title);
    }
    inline void ShowErrorDialog(
            const noex::string& msg,
            const char* title = "Error") noexcept {
        ShowErrorDialog(msg.c_str(), title);
    }
    inline void ShowErrorDialogWithLog(
            const char* func,
            const noex::string& msg,
            const char* title = "Error") noexcept {
        ShowErrorDialogWithLog(func, msg.c_str(), title);
    }
    inline void Log(const char* func, const char* msg) noexcept {
        PrintFmt("[%s] %s\n", func, msg);
    }
    inline void Log(const char* func, const char* label, const char* msg) noexcept {
        PrintFmt("[%s] %s: %s\n", func, label, msg);
    }
    inline void Log(const char* func, const noex::string&msg) {
        Log(func, msg.c_str());
    }
    inline void Log(const char* func, const char* label, const noex::string&msg) {
        Log(func, label, msg.c_str());
    }

 public:
    explicit MainFrame(const tuwjson::Value& definition = EMPTY_JSON,
                       const tuwjson::Value& config = EMPTY_JSON,
                       const char* json_path = nullptr) noexcept {
        Initialize(definition, config, json_path);
    }

    explicit MainFrame(const char* json_path) noexcept {
        Initialize(EMPTY_JSON, EMPTY_JSON, json_path);
    }

    void Initialize(const tuwjson::Value& definition,
                    const tuwjson::Value& config,
                    noex::string json_path) noexcept;

    void UpdatePanel(size_t definition_id) noexcept;
    noex::string OpenURLBase(size_t id) noexcept;
    void OpenURL(size_t id) noexcept;
    bool Validate() noexcept;
    noex::string GetCommand() noexcept;
    void RunCommand() noexcept;
    void GetDefinition(tuwjson::Value& json) noexcept;
    void SaveConfig() noexcept;
    void Fit(bool keep_width = false) noexcept;
    void Close() noexcept {
        if (m_mainwin != NULL)
            uiControlDestroy(uiControl(m_mainwin));
    #ifdef __TUW_UNIX__
        if (m_logwin != NULL)
            uiControlDestroy(uiControl(m_logwin));
    #endif
    }
    int IsSafeMode() noexcept {
        return uiMenuItemChecked(m_menu_safe_mode);
    }
    size_t GetDefinitionID() noexcept {
        return m_definition_id;
    }
};

void MainFrameDisableDialog() noexcept;
