#pragma once
#include <vector>
#include <string>
#include "rapidjson/document.h"
#include "component.h"
#include "json_utils.h"
#include "ui.h"

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

    std::vector<Component*> m_components;
    uiGrid* m_grid;
    uiButton* m_run_button;
    uiMenuItem* m_menu_item;

    void CreateFrame();
    void CreateMenu();
    json_utils::JsonResult CheckDefinition(rapidjson::Document& definition);
    void UpdateConfig();
    void ShowSuccessDialog(const std::string& msg, const std::string& title = "Success");
    void ShowErrorDialog(const std::string& msg, const std::string& title = "Error");
    void JsonLoadFailed(const std::string& msg);

 public:
    explicit MainFrame(const rapidjson::Document& definition =
                           rapidjson::Document(rapidjson::kObjectType),
                       const rapidjson::Document& config =
                           rapidjson::Document(rapidjson::kObjectType));
    void UpdatePanel(unsigned definition_id);
    void OpenURL(int id);
    bool Validate();
    std::string GetCommand();
    void RunCommand();
    void GetDefinition(rapidjson::Document& json);
    void SaveConfig();
    void Fit();
    void Close() {
        if (m_mainwin != NULL)
            uiControlDestroy(uiControl(m_mainwin));
    #ifdef __TUW_UNIX__
        if (m_logwin != NULL)
            uiControlDestroy(uiControl(m_logwin));
    #endif
    }
    int IsSafeMode() { return uiMenuItemChecked(m_menu_item); }
};

void MainFrameDisableDialog();
