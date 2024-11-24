#pragma once
#include "rapidjson/document.h"
#include "ui.h"
#include "string_utils.h"
#include "noex/vector.hpp"
#include "validator.h"

#define UNUSED(x) (void)(x)

// Base class for GUI components (file picker, combo box, etc.)
class Component {
 protected:
    void* m_widget;
    noex::string m_label;
    noex::string m_id;
    bool m_has_string;
    bool m_is_wide;
    Validator m_validator;
    uiLabel* m_error_widget;
    bool m_optional;
    noex::string m_prefix;
    noex::string m_suffix;

 private:
    bool m_add_quotes;

 public:
    explicit Component(const rapidjson::Value& j);
    virtual ~Component();
    virtual noex::string GetRawString() { return "";}
    noex::string GetString();
    const noex::string& GetID() const { return m_id; }

    virtual void SetConfig(const rapidjson::Value& config) { UNUSED(config); }
    virtual void GetConfig(rapidjson::Document& config) { UNUSED(config); }

    bool HasString() const { return m_has_string; }
    bool IsWide() const { return m_is_wide; }

    bool Validate(bool* redraw_flag);
    const noex::string& GetValidationError() const;
    void PutErrorWidget(uiBox* box);

    static Component* PutComponent(uiBox* box, const rapidjson::Value& j);
};

// containers for Combo and CheckArray
class MultipleValuesContainer {
 protected:
    noex::vector<noex::string> m_values;

 public:
    void SetValues(noex::vector<noex::string> values){
        m_values = values;
    }
};

class EmptyComponent : public Component {
 public:
    EmptyComponent(uiBox* box, const rapidjson::Value& j)
        : Component(j) { UNUSED(box); }
};

class StaticText : public Component {
 public:
    StaticText(uiBox* box, const rapidjson::Value& j);
};

class StringComponentBase : public Component {
 public:
    StringComponentBase(uiBox* box, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
};

class FilePicker : public StringComponentBase {
 private:
    noex::string m_ext;
 public:
    noex::string GetRawString() override;
    FilePicker(uiBox* box, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
    void OpenFile();
};

class DirPicker : public StringComponentBase {
 public:
    noex::string GetRawString() override;
    DirPicker(uiBox* box, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
    void OpenFolder();
};

class ComboBox : public StringComponentBase, MultipleValuesContainer {
 public:
    noex::string GetRawString() override;
    ComboBox(uiBox* box, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class RadioButtons : public StringComponentBase, MultipleValuesContainer {
 public:
    noex::string GetRawString() override;
    RadioButtons(uiBox* box, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class CheckBox : public Component {
 private:
    noex::string m_value;
 public:
    noex::string GetRawString() override;
    CheckBox(uiBox* box, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class CheckArray : public StringComponentBase, MultipleValuesContainer {
 public:
    noex::string GetRawString() override;
    CheckArray(uiBox* box, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class TextBox : public StringComponentBase {
 public:
    noex::string GetRawString() override;
    TextBox(uiBox* box, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
};

class IntPicker : public StringComponentBase {
 public:
    IntPicker(uiBox* box, const rapidjson::Value& j);
    noex::string GetRawString() override;
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

// This is the same as IntPicker cause uiSpinboxDouble is not supported yet.
class FloatPicker : public StringComponentBase {
 public:
    FloatPicker(uiBox* box, const rapidjson::Value& j);
    noex::string GetRawString() override;
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};
