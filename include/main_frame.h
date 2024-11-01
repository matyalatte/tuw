#pragma once
#include <vector>
#include "rapidjson/document.h"
#include "component.h"
#include "json_utils.h"
#include "string_utils.h"
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
    void ShowSuccessDialog(const tuwString& msg, const tuwString& title = "Success");
    void ShowErrorDialog(const tuwString& msg, const tuwString& title = "Error");
    void JsonLoadFailed(const tuwString& msg);

 public:
    explicit MainFrame(const rapidjson::Document& definition =
                           rapidjson::Document(rapidjson::kObjectType),
                       const rapidjson::Document& config =
                           rapidjson::Document(rapidjson::kObjectType));
    void UpdatePanel(unsigned definition_id);
    void OpenURL(int id);
    bool Validate();
    tuwString GetCommand();
    void RunCommand();
    void GetDefinition(rapidjson::Document& json);
    void SaveConfig();
    void Fit(bool keep_width = false);
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
