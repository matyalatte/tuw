#pragma once
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "wx/wx.h"
#include "wx/filepicker.h"
#include "wx/dnd.h"
#include "wx/spinctrl.h"
#include "custom_wx_obj.h"

enum ComponentType: int {
    COMP_UNKNOWN = 0,
    COMP_EMPTY,
    COMP_STATIC_TEXT,
    COMP_FILE,
    COMP_FOLDER,
    COMP_CHOICE,
    COMP_CHECK,
    COMP_CHECK_ARRAY,
    COMP_TEXT,
    COMP_INT,
    COMP_FLOAT,
    COMP_MAX
};

// Base class for GUI components (file picker, combo box, etc.)
class Component {
 protected:
    void* m_widget;
    wxString m_label;
    std::string m_id;

 private:
    bool m_has_string;
    bool m_add_quotes;

 public:
    Component(const rapidjson::Value& j, bool has_string);
    ~Component() {}
    virtual wxString GetRawString() { return "";}
    wxString GetString();
    std::string const GetID();

    virtual void SetConfig(const rapidjson::Value& config) {}
    virtual void GetConfig(rapidjson::Document& config) {}

    bool HasString(){ return m_has_string; }

    static Component* PutComponent(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
};

// containers for Choice and CheckArray
class MultipleValuesContainer {
 protected:
    wxArrayString m_values;

 public:
    void SetValues(wxArrayString values){
        m_values = values;
    }
};

class EmptyComponent : public Component {
 public:
    EmptyComponent(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
        : Component(j, false) {}
};

class StaticText : public Component {
 public:
    StaticText(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
};

class StringComponentBase : public Component {
 public:
    StringComponentBase(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
};

class FilePicker : public StringComponentBase {
 public:
    wxString GetRawString() override;
    FilePicker(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
};

class DirPicker : public StringComponentBase {
 public:
    wxString GetRawString() override;
    DirPicker(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
};

class Choice : public StringComponentBase, MultipleValuesContainer {
 public:
    wxString GetRawString() override;
    Choice(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class CheckBox : public Component {
 private:
    wxString m_value;
 public:
    wxString GetRawString() override;
    CheckBox(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class CheckArray : public StringComponentBase, MultipleValuesContainer {
 public:
    wxString GetRawString() override;
    CheckArray(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class TextBox : public StringComponentBase {
 public:
    wxString GetRawString() override;
    TextBox(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
};

class NumPickerBase : public StringComponentBase {
 protected:
    wxSpinCtrlDouble* m_picker;
    void SetOptions(const rapidjson::Value& j);
 public:
    NumPickerBase(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
    wxString GetRawString() override;
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class IntPicker : public NumPickerBase {
 public:
    IntPicker(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
};

class FloatPicker : public NumPickerBase {
 public:
    FloatPicker(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j);
};
