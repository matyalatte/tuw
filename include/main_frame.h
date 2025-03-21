#pragma once
#include "rapidjson/document.h"
#include "component.h"
#include "json_utils.h"
#include "string_utils.h"
#include "noex/vector.hpp"
#include "ui.h"

class MainFrame;

struct MenuData {
    MainFrame* main_frame;
    int menu_id;
};

#define EMPTY_DOCUMENT rapidjson::Document(rapidjson::kObjectType)

// Get "gui_definition.*"
noex::string GetDefaultJsonPath() noexcept;

// Main window
class MainFrame {
 private:
    rapidjson::Document m_definition;
    unsigned m_definition_id;
    rapidjson::Document m_config;
    uiWindow* m_mainwin;
#ifdef __TUW_UNIX__
    uiWindow* m_logwin;
#endif

    noex::vector<Component*> m_components;
    uiGrid* m_grid;
    uiButton* m_run_button;
    uiMenuItem* m_menu_item;
    noex::vector<MenuData> m_menu_data_vec;

    void CreateFrame() noexcept;
    void CreateMenu() noexcept;
    json_utils::JsonResult CheckDefinition(rapidjson::Document& definition) noexcept;
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
    explicit MainFrame(const rapidjson::Document& definition = EMPTY_DOCUMENT,
                       const rapidjson::Document& config = EMPTY_DOCUMENT,
                       const char* json_path = nullptr) noexcept {
        Initialize(definition, config, json_path);
    }

    explicit MainFrame(const char* json_path) noexcept {
        Initialize(EMPTY_DOCUMENT, EMPTY_DOCUMENT, json_path);
    }

    void Initialize(const rapidjson::Document& definition,
                    const rapidjson::Document& config,
                    noex::string json_path) noexcept;

    void UpdatePanel(unsigned definition_id) noexcept;
    noex::string OpenURLBase(int id) noexcept;
    void OpenURL(int id) noexcept;
    bool Validate() noexcept;
    noex::string GetCommand() noexcept;
    void RunCommand() noexcept;
    void GetDefinition(rapidjson::Document& json) noexcept;
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
        return uiMenuItemChecked(m_menu_item);
    }
};

void MainFrameDisableDialog() noexcept;
