#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "wx/wx.h"
#include "wx/filepicker.h"
#include "wx/dnd.h"
#include "json_utils.h"
#include "custom_wx_obj.h"

#ifdef _WIN32
#include <codecvt>  // char code converter for Windows system
#endif

// component for GUI
class Component {
 protected:
    void* m_widget;

 private:
    int m_height;
    bool m_has_string;
    bool m_add_quotes;
    std::string m_label;

 public:
    Component(nlohmann::json j, int height, bool has_string);
    ~Component() {}
    virtual wxString GetRawString() { return "";}
    wxString GetString();
    std::string GetLabel();
    int GetHeight();
    virtual nlohmann::json GetConfig();
    virtual void SetConfig(nlohmann::json config) {}

    void SetHeight(int h);

    bool HasString();

    static Component* PutComponent(wxPanel* panel, nlohmann::json j, int y);
};

class MultipleValuesContainer {
 protected:
    std::vector<std::string> m_values;

 public:
    void SetValues(std::vector<std::string> values){
        m_values = values;
    }
};

#ifdef _WIN32
std::string WStringToUTF8(const std::wstring& str);
std::wstring UTF8ToWString(const std::string& str);
#endif

class StaticText : public Component {
 public:
    StaticText(wxPanel* panel, nlohmann::json j, int y);
};

class FilePicker : public Component {
 public:
    wxString GetRawString() override;
    FilePicker(wxPanel* panel, nlohmann::json j, int y);
    nlohmann::json GetConfig() override;
    void SetConfig(nlohmann::json config) override;
};

class DirPicker : public Component {
 public:
    wxString GetRawString() override;
    DirPicker(wxPanel* panel, nlohmann::json j, int y);
    nlohmann::json GetConfig() override;
    void SetConfig(nlohmann::json config) override;
};

class Choice : public Component, MultipleValuesContainer {
 public:
    wxString GetRawString() override;
    Choice(wxPanel* panel, nlohmann::json j, int y);
    nlohmann::json GetConfig() override;
    void SetConfig(nlohmann::json config) override;
};

class CheckBox : public Component {
 private:
    std::string m_value;
 public:
    wxString GetRawString() override;
    CheckBox(wxPanel* panel, nlohmann::json j, int y);
    nlohmann::json GetConfig() override;
    void SetConfig(nlohmann::json config) override;
};

class CheckArray : public Component, MultipleValuesContainer {
 public:
    wxString GetRawString() override;
    CheckArray(wxPanel* panel, nlohmann::json j, int y);
    nlohmann::json GetConfig() override;
    void SetConfig(nlohmann::json config) override;
};

class TextBox : public Component {
 public:
    wxString GetRawString() override;
    TextBox(wxPanel* panel, nlohmann::json j, int y);
    nlohmann::json GetConfig() override;
    void SetConfig(nlohmann::json config) override;
};