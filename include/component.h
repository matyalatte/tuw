#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "wx/wx.h"
#include "wx/filepicker.h"
#include "wx/dnd.h"
#include "json_utils.h"
#include "custom_wx_obj.h"

// Base class for GUI components (file picker, combo box, etc.)
class Component {
 protected:
    void* m_widget;

 private:
    bool m_has_string;
    bool m_add_quotes;
    std::string m_label;

 public:
    Component(nlohmann::json j, bool has_string);
    ~Component() {}
    virtual wxString GetRawString() { return "";}
    wxString GetString();
    std::string GetLabel();

    virtual void SetConfig(nlohmann::json config) {}
    virtual nlohmann::json GetConfig();

    bool HasString();

    static Component* PutComponent(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j);
};

// containers for Choice and CheckArray
class MultipleValuesContainer {
 protected:
    std::vector<std::string> m_values;

 public:
    void SetValues(std::vector<std::string> values){
        m_values = values;
    }
};

#ifdef _WIN32
// char code converter for Windows system
std::string WStringToUTF8(const std::wstring& str);
std::wstring UTF8ToWString(const std::string& str);
#endif

class StaticText : public Component {
 public:
    StaticText(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j);
};

class StringComponentBase : public Component {
 public:
    StringComponentBase(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j);
    nlohmann::json GetConfig() override;
};

class FilePicker : public StringComponentBase {
 public:
    wxString GetRawString() override;
    FilePicker(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j);
    void SetConfig(nlohmann::json config) override;
};

class DirPicker : public StringComponentBase {
 public:
    wxString GetRawString() override;
    DirPicker(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j);
    void SetConfig(nlohmann::json config) override;
};

class Choice : public Component, MultipleValuesContainer {
 public:
    wxString GetRawString() override;
    Choice(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j);
    nlohmann::json GetConfig() override;
    void SetConfig(nlohmann::json config) override;
};

class CheckBox : public Component {
 private:
    std::string m_value;
 public:
    wxString GetRawString() override;
    CheckBox(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j);
    nlohmann::json GetConfig() override;
    void SetConfig(nlohmann::json config) override;
};

class CheckArray : public Component, MultipleValuesContainer {
 public:
    wxString GetRawString() override;
    CheckArray(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j);
    nlohmann::json GetConfig() override;
    void SetConfig(nlohmann::json config) override;
};

class TextBox : public StringComponentBase {
 public:
    wxString GetRawString() override;
    TextBox(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j);
    void SetConfig(nlohmann::json config) override;
};
